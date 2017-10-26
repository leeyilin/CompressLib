

#ifndef CTIMERTASKQUEUE_H
#define CTIMERTASKQUEUE_H

#include <map>
#include <set>
#include <typeinfo>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
using namespace std;

#include "thread.h"
#include "lock.h"
#include "log.h"


namespace BackCom
{
class CTimerTaskQueue : public CThread
{
public:
    class _Task
    {
    public:
        virtual ~_Task()
        {
        }
        virtual void Do(void *ptr) = 0;
    };

private:
    struct _TaskNode
    {
        _Task  *m_pTask;
        void   *m_pArg;
        time_t  m_from;
        time_t  m_to;
        int     m_intInterval;
        time_t  m_tmNextDo;
    };
    typedef multimap<time_t,int>    run_time_type;
    typedef map<int,_TaskNode>      task_queue_type;
    typedef set<int>                canceled_type;

public:
    CTimerTaskQueue(CLog * pLog = NULL
                   )
        : m_pLog(pLog)
         ,m_intCurTaskID(0)
    {
    }

    virtual ~CTimerTaskQueue()
    {
    }

    bool Start(const bool blnJoinable = true)
    {
        CThread::Start(NULL,blnJoinable);
        return true;
    };

    bool Stop(const bool blnJoined = true)
    {
        CThread::Stop();
        if( blnJoined==true )
        {
            void * pRet = NULL;
            CThread::Join(&pRet);
        }
        return true;
    };

    int  AddTask(_Task * pTask
                ,void * pArg
                ,const int intInterval

                ,const time_t from = 0
                ,const time_t to = 0x7fffffff
                );
    bool CancelTask(const int intTaskID
                   );

protected:
    virtual void Execute(void *ptr);

    void Log(CLog::Level enuLevel, const char * pchFunc, const char *pFormat, ...) const {
        if (m_pLog != NULL) {
            va_list arglist;
            va_start(arglist, pFormat);
            m_pLog->Log(enuLevel, typeid(*this).name(), pchFunc, pFormat, arglist);
            va_end(arglist);
        }
    }

private:
    CLog           *m_pLog;
    task_queue_type m_mapTaskQueue;
    run_time_type   m_mapNextRun;
    canceled_type   m_setCanceled;
    int             m_intCurTaskID;
    boost::mutex m_mutex;
    boost::condition_variable m_cond;
};

}// namespace BackCom
#endif
