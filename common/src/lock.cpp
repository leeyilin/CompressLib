
#include <stdarg.h>         // va_list
#include <stdio.h>

#include "lock.h"

namespace BackCom
{
//#define pthread_mutexattr_init(pattr) ((((pattr)->__mutexkind = (PTHREAD_MUTEX_ADAPTIVE_NP)) == (PTHREAD_MUTEX_ADAPTIVE_NP)) ? 0 : -1)
//#define pthread_mutexattr_settype(pattr,locktype) ((((pattr)->__mutexkind = (locktype)) == (locktype)) ? 0 : -1)

static void Log(const char *pchFile, const int intLineNo, const char *pFormat, ...) {
    va_list arglist;
    va_start(arglist, pFormat);
    char buffer[32768] = {0};
    vsprintf(buffer, pFormat, arglist);
    printf("[%16s][%4d]%s\n", pchFile, intLineNo, buffer);
    va_end(arglist);
}

CLock::CLock(const LockType type)
    : m_blnInited(false)
{
    pthread_mutexattr_t attr;
    int                 locktype;

    switch(type)
    {
    case Normal:
        locktype = PTHREAD_MUTEX_ADAPTIVE_NP;
        break;
    case ErrorCheck:
        locktype = PTHREAD_MUTEX_ERRORCHECK_NP;
        break;
    case Recursive:
        locktype = PTHREAD_MUTEX_RECURSIVE_NP;
        break;
    default:
        locktype = PTHREAD_MUTEX_RECURSIVE_NP;
        break;
    }

    int nRet = 0;
    nRet = pthread_mutexattr_init(&attr);
    if( nRet != 0 )
    {
        Log(__FILE__,__LINE__,"pthread_mutexattr_init(&attr) != 0{nRet=%d,&attr=%08p}",nRet,&attr);
    }
    else
    {
        nRet = pthread_mutexattr_settype(&attr,locktype);
        if( nRet != 0 )
        {//log
            Log(__FILE__,__LINE__,"pthread_mutexattr_settype(&attr,locktype) != 0{nRet=%d,&attr=%08p,locktype=%d}",nRet,&attr,locktype);
        }
        else
        {
            nRet = pthread_mutex_init(&m_mutexLock, &attr);
            if( nRet != 0 )
            {//log
                Log(__FILE__,__LINE__,"pthread_mutex_init(&m_mutexLock, &attr) != 0{nRet=%d,&m_mutexLock=%08p,&attr=%08p}",nRet,&m_mutexLock,&attr);
            }
            else
            {
                m_blnInited = true;
            }
        }
    }
}

CLock::~CLock()
{
    if( m_blnInited )
    {
        int nRet = pthread_mutex_destroy(&m_mutexLock);
        if( nRet != 0 )
        {//log
            Log(__FILE__,__LINE__,"pthread_mutex_destroy(&m_mutexLock) != 0{nRet=%d,&m_mutexLock=%08p}",nRet,&m_mutexLock);
        }
        m_blnInited = false;
    }
}

bool CLock::Lock() const
{
    if( m_blnInited == false )
    {
        Log(__FILE__,__LINE__,"m_blnInited == false{}");
        return false;
    }
    int nRet = 0;
    nRet = pthread_mutex_lock(&m_mutexLock);
    if( nRet != 0 )
    {
        Log(__FILE__,__LINE__,"pthread_mutex_lock(&m_mutexLock) != 0{nRet=%d,&m_mutexLock=%08p}",nRet,&m_mutexLock);
        return false;
    }
    else
        return true;
}

bool CLock::Unlock() const
{
    if( m_blnInited == false )
    {//log
        Log(__FILE__,__LINE__,"m_blnInited == false{}");
        return false;
    }
    int nRet = 0;
    nRet = pthread_mutex_unlock(&m_mutexLock);
    if( nRet != 0 )
    {//log
        Log(__FILE__,__LINE__,"pthread_mutex_unlock(&m_mutexLock) != 0{nRet=%d,&m_mutexLock=%08p}",nRet,&m_mutexLock);
        return false;
    }
    else
        return true;
}


CRWLock::CRWLock()
{
    int nRet = 0;
    nRet = pthread_rwlock_init(&m_lock,NULL);
    if( nRet != 0 )
    {
        Log(__FILE__,__LINE__,"pthread_rwlock_init(&m_lock,NULL) != 0{nRet=%d,&m_lock=%08p}",nRet,&m_lock);
    }
}

CRWLock::~CRWLock()
{
    int nRet = 0;
    nRet = pthread_rwlock_destroy(&m_lock);
    if( nRet != 0 )
    {
        Log(__FILE__,__LINE__,"pthread_rwlock_destroy(&m_lock) != 0{nRet=%d,&m_lock=%08p}",nRet,&m_lock);
    }
}

bool CRWLock::LockWrite() const
{
    int nRet = 0;
    nRet = pthread_rwlock_wrlock(&m_lock);
    if( nRet != 0 )
    {
        Log(__FILE__,__LINE__,"pthread_rwlock_wrlock(&m_lock) != 0{nRet=%d,&m_lock=%08p}",nRet,&m_lock);
        return false;
    }
    else
        return true;
}

bool CRWLock::UnlockWrite() const
{
    int nRet = 0;
    nRet = pthread_rwlock_unlock(&m_lock);
    if( nRet != 0 )
    {
        Log(__FILE__,__LINE__,"pthread_rwlock_unlock(&m_lock) != 0{nRet=%d,&m_lock=%08p}",nRet,&m_lock);
        return false;
    }
    else
        return true;
}

bool CRWLock::LockRead() const
{
    int nRet = 0;
    nRet = pthread_rwlock_rdlock(&m_lock);
    if( nRet != 0 )
    {
        Log(__FILE__,__LINE__,"pthread_rwlock_rdlock(&m_lock) != 0{nRet=%d,&m_lock=%08p}",nRet,&m_lock);
        return false;
    }
    else
        return true;
}

bool CRWLock::UnlockRead() const
{
    int nRet = 0;
    nRet = pthread_rwlock_unlock(&m_lock);
    if( nRet != 0 )
    {
        Log(__FILE__,__LINE__,"pthread_rwlock_unlock(&m_lock) != 0{nRet=%d,&m_lock=%08p}",nRet,&m_lock);
        return false;
    }
    else
        return true;
}


}// namespace BackCom
