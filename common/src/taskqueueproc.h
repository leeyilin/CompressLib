

#ifndef CTASKQUEUEPROC_H
#define CTASKQUEUEPROC_H


#include <exception>
#include <typeinfo>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <algorithm>
using namespace std;

#include "thread.h"
#include "baselog.h"


namespace BackCom
{
template < typename group_type      
          ,typename sub_id_type     
          ,typename info_type  
          ,bool blnGroupIntercurrent = false
         >
class CTaskQueueProc : private CBaseLog
{
public:
    typedef CTaskQueueProc<group_type,sub_id_type,info_type>  my_type;
    typedef long long int                               task_no_type;   
    
    struct  _TaskInfo
    {
        group_type      m_group;    
        sub_id_type     m_sub_id;   
        info_type       m_info;     
        bool            m_running;  
    };
    
    struct  _SubTaskInfo
    {
        sub_id_type     m_sub_id;   
        task_no_type    m_task_no;  
        
        bool operator==(const _SubTaskInfo & v) const
        {
            return m_sub_id==v.m_sub_id;
        }
        bool operator<(const _SubTaskInfo & v) const
        {
            return m_sub_id<v.m_sub_id;
        }
    };
    
    struct _GroupInfo
    {
        bool                m_blnIsRunning; 
        unsigned int        m_intPauseCount;
        deque<_SubTaskInfo> m_listSubTasks; 
    };
    
    class _Thread : public CThread
    {
        virtual void Execute(void *ptr)
        {
             ((my_type *)ptr)->Execute();
        };
    };

private:
    list<_Thread>       m_listThread;       
    
    mutable pthread_mutex_t     m_mutex;    
    pthread_cond_t      m_cond;             
    
    int                 m_intSleepThreadNum;
    bool                m_blnStop;          
    
    task_no_type                m_next_task_no; 
    map<task_no_type,_TaskInfo> m_mapTaskNos;
    map<group_type,_GroupInfo>  m_mapGroups;

    friend class _Thread;
public:
    
    
    
    CTaskQueueProc()
     : CBaseLog()
      ,m_intSleepThreadNum(0)
      ,m_blnStop(false)
      ,m_next_task_no(1)
    {
        pthread_mutex_init(&m_mutex,NULL);
        pthread_cond_init(&m_cond,NULL);
    };
    virtual ~CTaskQueueProc()
    {

        pthread_cond_destroy(&m_cond);
        pthread_mutex_destroy(&m_mutex);
    };
    
    bool Init(const int intThreadNum            
             ,CLog * pLog = NULL                
                                                
             )
    {
        CBaseLog::Init(pLog);
        
        pthread_mutex_init(&m_mutex,NULL);
        pthread_cond_init(&m_cond,NULL);
        
        m_intSleepThreadNum = 0;
        m_listThread.insert(m_listThread.begin(),intThreadNum,_Thread());
        Log(CLog::Info,__func__,"Init finished{intThreadNum=%d,m_listThread.size()=%d}",intThreadNum,m_listThread.size());
        return true;
    };

    
public:
    
    
    
    bool SubmitTask(const group_type & group    
                   ,const sub_id_type & sub_id  
                   ,const info_type & info      
                   )
    {
        int intRet = -1;    
        pthread_mutex_lock(&m_mutex);
        
        
        typename map<group_type,_GroupInfo>::iterator it_find_group = m_mapGroups.lower_bound(group);
        if( (it_find_group == m_mapGroups.end()) || (it_find_group->first!=group) )
        {
            _GroupInfo  info;
            info.m_intPauseCount = 0;
            info.m_blnIsRunning = false;
            it_find_group = m_mapGroups.insert(it_find_group,pair<group_type,_GroupInfo>(group,info));
        }
        _GroupInfo & group_info = it_find_group->second;
        deque<_SubTaskInfo> & task_list = group_info.m_listSubTasks;
        
        _SubTaskInfo tmp_sub = {sub_id,0};
        typename deque<_SubTaskInfo>::const_iterator it_find = find(task_list.begin(),task_list.end(),tmp_sub);
        if( it_find == task_list.end() )
        {
            
            intRet = 0;
            
            _SubTaskInfo sub_task;
            sub_task.m_sub_id = sub_id;
            sub_task.m_task_no = m_next_task_no++;
            
            task_list.push_back(sub_task);
            
            _TaskInfo & task_info = m_mapTaskNos[sub_task.m_task_no];
            task_info.m_group = group;
            task_info.m_sub_id = sub_id;
            task_info.m_info = info;
            task_info.m_running = false;
            
            if(  (m_intSleepThreadNum > 0)
              && (group_info.m_intPauseCount == 0)
              && (group_info.m_blnIsRunning == false)
              )
            {
                WakeOne();
            }
        }
        else
        {
            Log(CLog::Warn,__func__,"it_find != task_list.end()�{m_mapGroups.size()=%d,task_list.size()=%d}",m_mapGroups.size(),task_list.size());
        }
        
        pthread_mutex_unlock(&m_mutex);
        return intRet==0;
    };

    
    
    
    bool CancelTask(bool & blnTaskRunned        
                   ,const group_type & group    
                   ,const sub_id_type & sub_id  
                   )
    {
        pthread_mutex_lock(&m_mutex);
        
        int intRet = _DeleteTask(group,sub_id);
        
        pthread_mutex_unlock(&m_mutex);
        blnTaskRunned = (intRet==1);
        return ((intRet==0)||(intRet==1));
    };

    bool CancelGroup(const group_type & group    
                     ,bool forceCancel = false
                    )
    {
        int intRet = -1;
        pthread_mutex_lock(&m_mutex);
        //Log(CLog::Status,__func__,"Task info{group_count=%d, task_count=%d}", m_mapGroups.size(), m_mapTaskNos.size());
        typename map<group_type,_GroupInfo>::iterator it_find_group = m_mapGroups.find(group);
        if( it_find_group != m_mapGroups.end() )
        {
            if(forceCancel) {
                it_find_group->second.m_blnIsRunning = false;
            }
            if(it_find_group->second.m_blnIsRunning)
            {
                intRet = 1;
                //Log(CLog::Status,__func__,"Cancel group have task process");
            }
            else
            {
                deque<_SubTaskInfo> & task_list = it_find_group->second.m_listSubTasks;
                //Log(CLog::Status,__func__,"Cancel group task{task_count=%d}", task_list.size());
                typename deque<_SubTaskInfo>::const_iterator it = task_list.begin();
                for( ; it != task_list.end() ; ++it )
                {
                    m_mapTaskNos.erase(it->m_task_no);
                }

                m_mapGroups.erase(it_find_group);
                intRet = 0;
            }
        }
        else 
        {
            intRet = 0;
        }
        
        pthread_mutex_unlock(&m_mutex);
        return intRet==0;
    };

    bool PauseGroup(const group_type & group    
                   )
    {
        pthread_mutex_lock(&m_mutex);
        
        typename map<group_type,_GroupInfo>::iterator it_find_group = m_mapGroups.lower_bound(group);
        if( (it_find_group == m_mapGroups.end()) || (it_find_group->first!=group) )
        {
            _GroupInfo  info;
            info.m_intPauseCount = 0;
            info.m_blnIsRunning = false;
            it_find_group = m_mapGroups.insert(it_find_group,pair<group_type,_GroupInfo>(group,info));
        }
        _GroupInfo & group_info = it_find_group->second;
        ++group_info.m_intPauseCount;
        Log(CLog::Status,__func__,"Pause group info{pause_count=%d, task_count=%d, group_count=%d}", group_info.m_intPauseCount, group_info.m_listSubTasks.size(), m_mapGroups.size());
        pthread_mutex_unlock(&m_mutex);
        return true;
    }
    
    bool ResumeGroup(const group_type & group    
                    )
    {
        bool blnRet = false;
        pthread_mutex_lock(&m_mutex);
        
        typename map<group_type,_GroupInfo>::iterator it_group = m_mapGroups.find(group);
        if( it_group != m_mapGroups.end() )
        {
            _GroupInfo & group_info = it_group->second;
            --group_info.m_intPauseCount;
            blnRet = true;
            Log(CLog::Status,__func__,"Resume group info{pause_count=%d, task_count=%d, group_count=%d}", group_info.m_intPauseCount, group_info.m_listSubTasks.size(), m_mapGroups.size());
            if(  (m_intSleepThreadNum > 0)
              && (group_info.m_intPauseCount == 0)
              && (group_info.m_blnIsRunning == false)
              )
            {
                WakeOne();
            }

            if(group_info.m_intPauseCount == 0 
                && group_info.m_listSubTasks.empty() == true)
            {
                m_mapGroups.erase(it_group);
            }            
        }
        
        pthread_mutex_unlock(&m_mutex);
        return blnRet;
    }

    bool StartThreads()
    {
        pthread_mutex_lock(&m_mutex);
        m_blnStop = false;
        pthread_mutex_unlock(&m_mutex);
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
        pthread_mutex_lock(&m_mutex);
        m_blnStop = true;
        pthread_mutex_unlock(&m_mutex);
        
        WakeAll();
        
        typedef typename list< _Thread >::iterator iterator;
        iterator it = m_listThread.begin();
        for( ; it != m_listThread.end() ; ++ it )
        {
            void * pRet = NULL;
            (*it).Join(&pRet);
        }
        return true;
    };

    
public:
    void GetInfo(int & intTotalThreadNum    
                ,int & intSleepThreadNum    
                ,bool & blnStop             
                ,task_no_type & next_task_no
                ,int & intTasksCount        
                ,int & intGroupsCount       
                )const
    {
        pthread_mutex_lock(&m_mutex);
        
        intTotalThreadNum = m_listThread.size();
        intSleepThreadNum = m_intSleepThreadNum;
        blnStop = m_blnStop;
        next_task_no = m_next_task_no;
        intTasksCount = m_mapTaskNos.size();
        intGroupsCount = m_mapGroups.size();
        
        pthread_mutex_unlock(&m_mutex);
    }
    void GetTaskDetailInfo(map<task_no_type,_TaskInfo> & mapTaskNos
                          )const
    {
        pthread_mutex_lock(&m_mutex);
        
        mapTaskNos = m_mapTaskNos;
        
        pthread_mutex_unlock(&m_mutex);
    }
    void GetGroupDetailInfo(map<group_type,_GroupInfo> & mapGroups
                           )const
    {
        pthread_mutex_lock(&m_mutex);
        
        mapGroups = m_mapGroups;
        
        pthread_mutex_unlock(&m_mutex);
    }


    int GetGroupTaskCount(const group_type & group)const
    {
        int intCount = 0;
        pthread_mutex_lock(&m_mutex);
        
        typename map<group_type,_GroupInfo>::const_iterator it_group = m_mapGroups.find(group);
        if( it_group != m_mapGroups.end() )
        {
            intCount = it_group->second.m_listSubTasks.size();
        }
        
        pthread_mutex_unlock(&m_mutex);
        return intCount;
    }
    
private:
    
    bool WakeOne()
    {
        return (pthread_cond_signal(&m_cond)==0);
    }

    
    bool WakeAll()
    {
        return (pthread_cond_broadcast(&m_cond)==0);
    }
    
    int _DeleteTask(const group_type & group
                   ,const sub_id_type & sub_id
                   )
    {
        int intRet = -1;
        typename map<group_type,_GroupInfo>::iterator it_find_group = m_mapGroups.find(group);
        if( it_find_group != m_mapGroups.end() )
        {
            deque<_SubTaskInfo> & task_list = it_find_group->second.m_listSubTasks;
            _SubTaskInfo tmp = {sub_id,0};
            typename deque<_SubTaskInfo>::iterator it_find_sub_id = find(task_list.begin(),task_list.end(),tmp);
            if( it_find_sub_id != task_list.end() )
            {
                task_no_type task_no = it_find_sub_id->m_task_no;
                typename map<task_no_type,_TaskInfo>::iterator it_find_all = m_mapTaskNos.find(task_no);
                if( it_find_all == m_mapTaskNos.end() )
                {
                    Log(CLog::Error,__func__,"it_find_all == m_mapTaskNos.end() {m_mapGroups.size()=%d,m_mapTaskNos.size()=%d}",m_mapGroups.size(),m_mapTaskNos.size());
                }
                else
                {
                    _TaskInfo & info = it_find_all->second;
                    if( info.m_running == true )
                    {
                        intRet = -2;
                    }
                    else
                    {
                        intRet = 0;   
                        m_mapTaskNos.erase(it_find_all);
                        task_list.erase(it_find_sub_id);
                        
                        if( (it_find_group->second.m_intPauseCount == 0)
                        &&(it_find_group->second.m_blnIsRunning == false)
                        &&(task_list.empty() == true)
                        )
                        {
                            m_mapGroups.erase(it_find_group);
                        }
                    }
                }
            }
        }
        return intRet;
    };

    
    bool Execute()
    {
        pthread_mutex_lock(&m_mutex);
        while(true)
        {
            
            if(m_blnStop==true)
            {
                
                
                static int intRunningThreadNum = -1;   
                if( intRunningThreadNum == -1 )
                {
                    intRunningThreadNum = m_listThread.size();
                }
                if( --intRunningThreadNum != 0 )
                {
                }
                else
                {
                    while(m_mapTaskNos.empty()==false)
                    {
                        const _TaskInfo task = m_mapTaskNos.begin()->second;
                        
                        int intRet = _DeleteTask(task.m_group,task.m_sub_id);
                        if( (intRet != 0) && (intRet != 1) )
                        {
                            Log(CLog::Error,__func__," (intRet != 0) && (intRet != 1) {intRet=%d,m_mapGroups.size()=%d,m_mapTaskNos.size()=%d}",intRet,m_mapGroups.size(),m_mapTaskNos.size());
                        }
                        
                        
                        pthread_mutex_unlock(&m_mutex);
                        
                        UndoTask(task.m_group,task.m_sub_id,task.m_info);
                        
                        pthread_mutex_lock(&m_mutex);
                    }
                }
                
                pthread_mutex_unlock(&m_mutex);
                break;
            }
            
            _TaskInfo taskinfo;
            task_no_type taskno;
            {
                typename map<task_no_type,_TaskInfo>::iterator it_all = m_mapTaskNos.begin();
                for(;it_all != m_mapTaskNos.end();++it_all)
                {
                    _TaskInfo & task = it_all->second;
                    if(it_all->second.m_running)
                    {
                        continue;
                    }
                    
                    typename map<group_type,_GroupInfo>::iterator it_find_group = m_mapGroups.find(task.m_group);
                    if( it_find_group != m_mapGroups.end() )
                    {
                        _GroupInfo & info = it_find_group->second;
                        if( (info.m_intPauseCount == 0)
                          &&(info.m_blnIsRunning == false)
                          )
                        {
                            if(!blnGroupIntercurrent)
                            {
                                info.m_blnIsRunning = true;
                            }
                            
                            break;
                        }
                    }
                    else
                    {
                        Log(CLog::Error,__func__,"it_find_group != m_mapGroups.end(){m_mapGroups.size()=%d,m_mapTaskNos.size()=%d}",m_mapGroups.size(),m_mapTaskNos.size());
                    }
                }
                if(it_all == m_mapTaskNos.end())
                {
                    m_intSleepThreadNum++;
                    pthread_cond_wait(&m_cond,&m_mutex);
                    m_intSleepThreadNum--;
                    continue;
                }
                
                taskinfo = it_all->second;
                taskno = it_all->first;
                it_all->second.m_running = true;
                pthread_mutex_unlock(&m_mutex);
            }
            
            DoTask(taskinfo.m_group,taskinfo.m_sub_id,taskinfo.m_info);
            {
                pthread_mutex_lock(&m_mutex);
                typename map<task_no_type,_TaskInfo>::iterator it_all = m_mapTaskNos.find(taskno);
                if(it_all == m_mapTaskNos.end() )
                {
                    Log(CLog::Error,__func__,"it_all == m_mapTaskNos.end()");
                    continue;
                }
                
                it_all->second.m_running = false;
                
                typename map<group_type,_GroupInfo>::iterator it_find_group = m_mapGroups.find(it_all->second.m_group);
                if( it_find_group == m_mapGroups.end() )
                {
                    Log(CLog::Error,__func__,"it_find_group == m_mapGroups.end(){m_mapGroups.size()=%d}",m_mapGroups.size());
                }
                else
                {
                    _GroupInfo & info = it_find_group->second;
                    info.m_blnIsRunning = false;
                }
                
                {
                    if( it_find_group != m_mapGroups.end() )
                    {
                        _GroupInfo & info = it_find_group->second;
                        _SubTaskInfo tmp = {it_all->second.m_sub_id,0};
                        typename deque<_SubTaskInfo>::iterator it_find_sub_id = find(info.m_listSubTasks.begin(),info.m_listSubTasks.end(),tmp);
                        if( it_find_sub_id != info.m_listSubTasks.end() )
                        {
                            info.m_listSubTasks.erase(it_find_sub_id);
                            
                            if( (info.m_listSubTasks.empty() == true)
                                &&(info.m_intPauseCount == 0)
                                )
                            {
                                m_mapGroups.erase(it_find_group);
                            }
                        }
                        else
                        {
                            Log(CLog::Error,__func__,"it_find_sub_id != info.m_listSubTasks.end(){info.m_listSubTasks.size()=%d}",info.m_listSubTasks.size());
                        }
                    }

                    m_mapTaskNos.erase(it_all);
                }
            }
        }
        
        return true;
    }
        
        
    virtual bool DoTask(const group_type & group    
                          ,const sub_id_type & sub_id   
                          ,const info_type & info               
                          ) = 0;
    
    virtual void UndoTask(const group_type & group  
                               ,const sub_id_type & sub_id  
                               ,const info_type & info      
                               ) = 0;
    
};

}
#endif
