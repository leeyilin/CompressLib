


#ifndef CTHREAD_H
#define CTHREAD_H

#include <pthread.h>
#include <list>
using namespace std;

namespace BackCom
{
class CThread
{
public:
    CThread();
    virtual ~CThread();
    bool Start(void * arg, bool bJoinable = true);
    bool IsRunning();
    virtual void Stop();
    pthread_t GetThreadId(){return m_thread;}
    int Join(void **ppRetval);
protected:
    void Run(void * arg);
    static void * EntryPoint (void *);
    virtual void Setup();
    virtual void Execute(void *) = 0;
    void * GetArg() const {return m_arg;}
    void SetArg(void * a) {m_arg = a;}
    bool GetStopFlag()const;
    void SetStopFlag();
    void ResetStopFlag();
private:
    void * m_arg;
    bool m_bStop;
    mutable pthread_t m_thread;
    mutable pthread_mutex_t m_mutexStop;
private:
    void AddSelfToList();
    void RemoveSelfFromList();
    static pthread_mutex_t m_mutexRun;
    static list<void*> m_ThreadList;
};

class CThreadStep : public CThread
{
public:
    CThreadStep();
    virtual ~CThreadStep();
    bool IsIdle();
    void NextStep();
    virtual void Stop();
protected:
    virtual void Execute(void *arg);
    virtual void StepTask(void *arg) = 0;
private:
    pthread_mutex_t m_mutexStep;
    pthread_cond_t m_cond;
};

class CThreadLoop : public CThread
{
public:
    CThreadLoop();
    virtual ~CThreadLoop();
    virtual void Stop();
    void Pause();
    void Resume();
    bool GetPauseStatus();
    bool IsIdle();
protected:
    virtual void Execute(void *arg);
    virtual void LoopTask(void *arg) = 0;
    void SetSleepTime(int nMilliSec){m_nSleepTime = nMilliSec;}
private:
    pthread_mutex_t m_mutexPause;
    pthread_mutex_t m_mutexLoop;
    bool m_bPause;
    int m_nSleepTime; //milli-seconds
};

}// namespace BackCom
#endif //_THREAD_H__
