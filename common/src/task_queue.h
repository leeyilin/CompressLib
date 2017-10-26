#pragma once

#include <exception>
#include <typeinfo>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>

#include "thread.h"
#include "comfun.h"
#include "baselog.h"
using namespace BackCom;


template <typename task_type>
class CTaskQueue
{
public:
    class _Thread : public CThread
    {
        virtual void Execute(void *ptr)
        {
             ((CTaskQueue *)ptr)->Execute();
        }
    };

private:
    boost::mutex m_mutex;
    boost::condition_variable m_cond;

    std::list<_Thread>  m_listThread;
    int                 m_intSleepThreadNum;
    bool                m_blnStop;
    std::deque<task_type> m_queue;
    unsigned int        m_timeOut; // timeout in microseconds, zero means no time out

    // Wake up the waiting thread until there's at least a certain count of
    // tasks or timeout if set.
    unsigned int        m_waterMark;

public:
    explicit CTaskQueue(unsigned int waterMark = 1)
     :m_intSleepThreadNum(0)
      ,m_blnStop(false)
      ,m_timeOut(0)
      ,m_waterMark(waterMark)
    {
    }

    virtual ~CTaskQueue() { }

    bool Init(const int intThreadNum)
    {
        m_intSleepThreadNum = 0;
        m_listThread.insert(m_listThread.begin(),intThreadNum,_Thread());
        return true;
    };


public:
    void SubmitTask(const task_type& task)
    {
        boost::lock_guard<boost::mutex> guard(m_mutex);
        size_t size = m_queue.size();
        m_queue.push_back(task);
        if (size < m_waterMark && m_queue.size() >= m_waterMark) {
            m_cond.notify_one();
        }
    }

    void SubmitTask(const vector<task_type>& tasks)
    {
        boost::lock_guard<boost::mutex> guard(m_mutex);
        m_queue.insert(m_queue.end(), tasks.begin(), tasks.end());
        m_cond.notify_all();
    }

    void ClearAllTasks()
    {
        boost::lock_guard<boost::mutex> guard(m_mutex);
        std::deque<task_type>().swap(m_queue);
    }

    bool StartThreads()
    {
        boost::lock_guard<boost::mutex> guard(m_mutex);
        m_blnStop = false;
        typedef typename list<_Thread >::iterator iterator;
        iterator it = m_listThread.begin();
        for( ; it != m_listThread.end() ; ++ it )
        {
            (*it).Start(this,true);
        }
        return true;
    };


    bool StopThreads()
    {
        {
            boost::lock_guard<boost::mutex> guard(m_mutex);
            m_blnStop = true;
            std::deque<task_type>().swap(m_queue);
            m_cond.notify_all();
        }

        typedef typename list< _Thread >::iterator iterator;
        iterator it = m_listThread.begin();
        for( ; it != m_listThread.end() ; ++ it ) {
            void * pRet = NULL;
            it->Join(&pRet);
        }
        return true;
    }

    void SetTimeOut(unsigned int microSeconds) {
        boost::lock_guard<boost::mutex> guard(m_mutex);
        m_timeOut = microSeconds;
        m_cond.notify_all();
    }

    virtual void ExecuteTasks(const std::deque<task_type>& tasks, bool /* timeout */)
    {
        for (size_t i = 0; i < tasks.size(); i++) {
            DoTask(tasks[i]);
        }
    }

private:
    bool Execute()
    {
        boost::unique_lock<boost::mutex> lock(m_mutex);
        while (!m_blnStop) {
            if (m_queue.size() < m_waterMark) {
                m_intSleepThreadNum++;
                if (m_timeOut) {
                    boost::system_time abstime = boost::get_system_time() + boost::posix_time::microseconds(m_timeOut);
                    if (!m_cond.timed_wait(lock, abstime)) {
                        std::deque<task_type> tmp;
                        tmp.swap(m_queue);
                        lock.unlock();
                        ExecuteTasks(tmp, true);
                        lock.lock();
                    }
                } else {
                    m_cond.wait(lock);
                }
                m_intSleepThreadNum--;

                // go to check if we are stopped
                continue;
            }

            std::deque<task_type> tmp;
            tmp.swap(m_queue);
            lock.unlock();

            ExecuteTasks(tmp, false);
            lock.lock();
        }

        lock.unlock();

        return true;
    }


	virtual bool DoTask(const task_type& task) = 0;

	virtual void UndoTask(const task_type& task) {  }
};

