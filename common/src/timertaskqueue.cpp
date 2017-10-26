
#include <exception>
#include <typeinfo>
#include <boost/thread/locks.hpp>
using namespace std;

#include "timertaskqueue.h"
#include "log.h"

namespace BackCom
{

void CTimerTaskQueue::Execute(void */*ptr*/)
{
    boost::unique_lock<boost::mutex> lock(m_mutex);
    while (!GetStopFlag()) {
        time_t now = time(NULL);
        //
        for(canceled_type::const_iterator it = m_setCanceled.begin() ; it != m_setCanceled.end() ; ++it )
        {
            int intTaskID = *it;
            task_queue_type::iterator it_find_task = m_mapTaskQueue.find(intTaskID);
            if (it_find_task != m_mapTaskQueue.end()) {
                _TaskNode & cur_task = it_find_task->second;
                delete cur_task.m_pTask;
                //
                m_mapTaskQueue.erase(it_find_task);
            }
            //
            for(run_time_type::iterator it_tmp = m_mapNextRun.begin() ; it_tmp != m_mapNextRun.end() ; ++it_tmp )
            {
                if( it_tmp->second == intTaskID )
                {
                    m_mapNextRun.erase(it_tmp);
                    break;
                }
            }
        }

        if (m_mapNextRun.empty()) {
            m_cond.wait(lock);
            if (m_mapNextRun.empty()) {
                continue;
            }
        }

        run_time_type::iterator it = m_mapNextRun.begin();
        if (it->first > now) {
            boost::system_time abstime = boost::get_system_time() + boost::posix_time::seconds(it->first - now);
            if (m_cond.timed_wait(lock, abstime)) {
                continue;
            }
        } else {
            int intTaskID = it->second;
            m_mapNextRun.erase(it);

            task_queue_type::iterator it_find_task = m_mapTaskQueue.find(intTaskID);
            if (it_find_task == m_mapTaskQueue.end()) {
                continue;
            }

            _TaskNode & cur_task = it_find_task->second;
            time_t next = now + cur_task.m_intInterval;
            bool blnFinished = (next > cur_task.m_to);
            if (blnFinished == false) {// task not finished,need scheduled more
                cur_task.m_tmNextDo = next;
                m_mapNextRun.insert(pair<time_t,int>(next,intTaskID));
            } else {
                m_setCanceled.insert(intTaskID);
                cur_task.m_tmNextDo = 0;
            }
            // 打印日志：一个任务将被执行
            Log(CLog::Info,__func__,"one task will be do!{now=%d,next=%d,blnFinished=%d,cur_task=(pTask=%p('%s'),pArg=%p,from=%d,to=%d,intInterval=%ds)}",now,next,blnFinished,cur_task.m_pTask,typeid(*cur_task.m_pTask).name(),cur_task.m_pArg,cur_task.m_from,cur_task.m_to,cur_task.m_intInterval);
            //
            cur_task.m_pTask->Do(cur_task.m_pArg);
        }
    }
    lock.unlock();
    Log(CLog::Status,__func__,"Thread is stoped! [GetStopFlag()==true]{GetStopFlag()=%d,this=%08p}",GetStopFlag(),this);
    return;
}

// 提交一个需要定时周期执行的任务
int  CTimerTaskQueue::AddTask(_Task * pTask          // [in ]指定任务对象指针，必须为有效对象指针不得为NULL
            ,void * pArg            // [in ]指定执行任务时调用 Do 函数的参数
            ,const int intInterval  // [in ]指定任务的执行间隔，单位：秒；
                                    //      0表示在任务有效时间范围内仅执行一次，
                                    //      非零值表示在任务有效时间范围内每经过指定时间间隔即执行一次指定任务；
            ,const time_t from      // [in ]指定任务的开始时间
            ,const time_t to        // [in ]指定任务的结束时间
            )
{
    if( (pTask == NULL) || (from > to) || (intInterval < 0) )
    {// 参数非法
        Log(CLog::Error,__func__,"Paramter invalid[(pTask == NULL) || (from > to) || (intInterval < 0)]!{pTask=%p,from=%d,to=%d,intInterval=%d}",pTask,from,to,intInterval);
        return 0;
    }
    //
    _TaskNode cur_task;
    cur_task.m_pTask = pTask;
    cur_task.m_pArg = pArg;
    cur_task.m_from = from;
    cur_task.m_to = to;
    cur_task.m_intInterval = intInterval;
    int intTaskID = 0;
    {
        boost::lock_guard<boost::mutex> guard(m_mutex);
        intTaskID = ++m_intCurTaskID;
        m_mapTaskQueue[intTaskID] = cur_task;
        m_mapNextRun.insert(pair<time_t,int>(from,intTaskID));
        m_cond.notify_one();
    }
    //
    Log(CLog::Info,__func__,"succeed info{pTask=%p('%s'),pArg=%p,from=%d,to=%d,intInterval=%ds}",pTask,typeid(*pTask).name(),pArg,from,to,intInterval);
    return intTaskID;
}

bool CTimerTaskQueue::CancelTask(const int intTaskID
                   )
{
    boost::lock_guard<boost::mutex> guard(m_mutex);
    if (m_mapTaskQueue.find(intTaskID) != m_mapTaskQueue.end() &&
        m_setCanceled.find(intTaskID) == m_setCanceled.end()) {
        m_setCanceled.insert(intTaskID);
        m_cond.notify_one();
    }
    return true;
}

}// namespace BackCom
