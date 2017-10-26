

#include "thread.h"
#include <errno.h>
#include <unistd.h>
#include <algorithm>
///////////////////////////////////////////////////////////////////////
//class CThread
//
namespace BackCom
{
pthread_mutex_t CThread::m_mutexRun = PTHREAD_MUTEX_INITIALIZER;
list<void*> CThread::m_ThreadList;

CThread::CThread()
{
    m_bStop = false;
    m_thread = 0;
    m_arg = NULL;
    pthread_mutex_init(&m_mutexStop, NULL);
}

CThread::~CThread()
{
    pthread_mutex_destroy(&m_mutexStop);
}

bool CThread::Start(void * arg, bool bJoinable)
{
    bool bResult = false;
    pthread_mutex_lock(&m_mutexStop);
    AddSelfToList();
    try
    {
        SetArg(arg);
        pthread_attr_t thread_attr;
        pthread_attr_init(&thread_attr);
        if (bJoinable)
        {
            pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
        }
        else
        {
            pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        }
        
        if (pthread_create(&m_thread, &thread_attr, &(CThread::EntryPoint), this) == 0)
            bResult = true;
        else
        {
            RemoveSelfFromList();
            bResult = false;
        }
    }
    catch(...)
    {
        RemoveSelfFromList();
        bResult = false;
        
    }
    pthread_mutex_unlock(&m_mutexStop);
    return bResult;
}

void CThread::Stop()
{
    SetStopFlag();
}

int CThread::Join(void **ppRetval)
{
    if (m_thread != 0) {
        return pthread_join(m_thread, ppRetval);
    }
    return 0;
}

void CThread::Run(void * arg)
{
    //try
    {
        Setup();
        Execute(arg);
        ResetStopFlag();
        RemoveSelfFromList();
    }
    //catch(...)
    {
    }
}

void * CThread::EntryPoint (void * pthis)
{
    //try
    {
        CThread * pt = (CThread *)pthis;
        pt->Run(pt->GetArg());
    }
    //catch(...)
    {
    }
    return NULL;
}

void CThread::Setup()
{
    //Anything you want to be executed before Execute().
}

bool CThread::GetStopFlag()const
{
    bool bResult;
    pthread_mutex_lock(&m_mutexStop);
    bResult = m_bStop;
    pthread_mutex_unlock(&m_mutexStop);
    return bResult;
}

void CThread::SetStopFlag()
{
    pthread_mutex_lock(&m_mutexStop);
    m_bStop = true;
    pthread_mutex_unlock(&m_mutexStop);
}

void CThread::ResetStopFlag()
{
    pthread_mutex_lock(&m_mutexStop);
    m_bStop = false;
    pthread_mutex_unlock(&m_mutexStop);
}

void CThread::AddSelfToList()
{
    pthread_mutex_lock(&m_mutexRun);
    m_ThreadList.push_back(this);
    pthread_mutex_unlock(&m_mutexRun);
}

void CThread::RemoveSelfFromList()
{
    pthread_mutex_lock(&m_mutexRun);
    m_ThreadList.remove(this);
    pthread_mutex_unlock(&m_mutexRun);
}

bool CThread::IsRunning()
{
    bool bResult;
    list<void*>::iterator iter;
    pthread_mutex_lock(&m_mutexRun);
    iter = find(m_ThreadList.begin(), m_ThreadList.end(), this);
    if (iter == m_ThreadList.end())
        bResult = false;
    else
        bResult = true;
    pthread_mutex_unlock(&m_mutexRun);
    return bResult;
}


///////////////////////////////////////////////////////////////////////
//class CThreadStep
//

CThreadStep::CThreadStep()
{
    pthread_mutex_init(&m_mutexStep, NULL);
    pthread_cond_init(&m_cond, NULL);
}

CThreadStep::~CThreadStep()
{
    pthread_mutex_destroy(&m_mutexStep);
    pthread_cond_destroy(&m_cond);
}

bool CThreadStep::IsIdle()
{
    int nRet = pthread_mutex_trylock(&m_mutexStep);
    if (nRet == EBUSY)
        return false;
    else if (nRet == 0)
    {
        pthread_mutex_unlock(&m_mutexStep);
        return true;
    }
    else
    {
        return false;
    }
}

void CThreadStep::NextStep()
{
    pthread_cond_signal(&m_cond);
}

void CThreadStep::Execute(void *arg)
{
    while(true)
    {
        pthread_cond_wait(&m_cond,&m_mutexStep);
        if (GetStopFlag())
        {
            pthread_mutex_unlock(&m_mutexStep);
            break;
        }
        StepTask(arg);
    }
}

void CThreadStep::Stop()
{
    CThread::Stop();
    pthread_cond_signal(&m_cond);
}


////////////////////////////////////////////////////////////////////
//class CThreadLoop
CThreadLoop::CThreadLoop()
{
    m_bPause = false;
    m_nSleepTime = 0;
    pthread_mutex_init(&m_mutexPause, NULL);
    pthread_mutex_init(&m_mutexLoop, NULL);
}

CThreadLoop::~CThreadLoop()
{
    pthread_mutex_destroy(&m_mutexPause);
    pthread_mutex_destroy(&m_mutexLoop);
}

void CThreadLoop::Stop()
{
    CThread::Stop();
}

void CThreadLoop::Pause()
{
    pthread_mutex_lock(&m_mutexPause);
    m_bPause = true;
    pthread_mutex_unlock(&m_mutexPause);
    while(!IsIdle())
    {
        timeval tv = {0, 200000};
        select(0, NULL, NULL, NULL, &tv);
    }
}

void CThreadLoop::Resume()
{
    pthread_mutex_lock(&m_mutexPause);
    m_bPause = false;
    pthread_mutex_unlock(&m_mutexPause);
}

bool CThreadLoop::GetPauseStatus()
{
    bool bPause;
    pthread_mutex_lock(&m_mutexPause);
    bPause = m_bPause;
    pthread_mutex_unlock(&m_mutexPause);
    return bPause;
}

bool CThreadLoop::IsIdle()
{
    int nRet = pthread_mutex_trylock(&m_mutexLoop);
    if (nRet == EBUSY)
        return false;
    else if (nRet == 0)
    {
        pthread_mutex_unlock(&m_mutexLoop);
        return true;
    }
    else
    {
        return false;
    }
}

void CThreadLoop::Execute(void *arg)
{
    while(!GetStopFlag())
    {
        if (GetPauseStatus())
        {
            //sleep 50ms
            //usleep(50000); //this function in solaris may be not reliable
            timeval tv = {0, 50000};
            select(0, NULL, NULL, NULL, &tv);
            continue;
        }
        
        pthread_mutex_lock(&m_mutexLoop);
        LoopTask(arg);
        pthread_mutex_unlock(&m_mutexLoop);
        // don't sleep too long, replace with sleeping multitimes, 50ms per time
        if (m_nSleepTime > 0)
        {
            for(int i = 0; i <= m_nSleepTime/50 && !GetStopFlag() && !GetPauseStatus(); i++)
            {
                //at least sleep 50ms
                //usleep(50000); //this function in solaris may be not reliable
                timeval tv = {0, 50000};
                select(0, NULL, NULL, NULL, &tv);
            }
        }
    }
}
}// namespace BackCom
