
#ifndef CLOCK_H
#define CLOCK_H

#include <pthread.h>

namespace BackCom
{
class CLock
{
public:
    enum LockType{ Normal,ErrorCheck,Recursive  };
public:
    CLock(const LockType type = Recursive);
    ~CLock();

public:
    bool Lock() const;
    bool Unlock() const;
    pthread_mutex_t* GetPthreadMutex() { return &m_mutexLock; }

private:
    bool m_blnInited;
    mutable pthread_mutex_t m_mutexLock;
};


class CRWLock
{

public:
    CRWLock();
    ~CRWLock();
    bool LockWrite() const;
    bool UnlockWrite() const;
    bool LockRead() const;
    bool UnlockRead() const;

private:
    mutable pthread_rwlock_t  m_lock;
};

class CAutoLock
{
    const CLock    *m_pLock;
public:
    CAutoLock(const CLock & lock) : m_pLock(&lock)
    {
        m_pLock->Lock();
    };
    ~CAutoLock()
    {
        m_pLock->Unlock();
        m_pLock = NULL;
    };
};

class CAutoLockRead
{
    const CRWLock    *m_pLock;
public:
    CAutoLockRead(const CRWLock & lock) : m_pLock(&lock)
    {
        m_pLock->LockRead();
    };
    ~CAutoLockRead()
    {
        m_pLock->UnlockRead();
        m_pLock = NULL;
    };
};

class CAutoLockWrite
{
    const CRWLock    *m_pLock;
public:
    CAutoLockWrite(const CRWLock & lock) : m_pLock(&lock)
    {
        m_pLock->LockWrite();
    };
    ~CAutoLockWrite()
    {
        m_pLock->UnlockWrite();
        m_pLock = NULL;
    };
};

}// namespace BackCom
#endif  // #ifndef __LOCK_H
