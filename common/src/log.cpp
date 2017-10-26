
#define _FILE_OFFSET_BITS 64

#include "log.h"

#include <stdio.h>
#include <fcntl.h>                  //O_WRONLY|O_CREAT
#include <errno.h>                  // errno
#include <string.h>                 //char *strerror(int errnum);
#include <sys/time.h>

namespace BackCom
{
CLog * CLog::s_pLog = NULL;

CLog::CLog()
    :    m_bOutputScreen(false)
        ,m_bPrintMicroseconds(false)
        ,m_enuLogLevel(Info)
        ,m_bNameUseDate(false)
        ,m_nOpenFlag(0)
        ,m_LastOpenTime(1970,1,1,0,0,0)
        ,m_nFds(-1)
        ,m_bInited(false)
{
    m_pchFile[0] = '\0';
}

CLog::~CLog()
{
    if( m_bInited )
    {
        Log(System,__FILE__,__LINE__,"Logging System Stopped!");
        ::close(m_nFds);
        //
        int nRet = pthread_mutex_destroy(&m_mutexLock);
        if( nRet != 0 )
        {
            printf("{CLog::~CLog()}pthread_mutex_destroy(&m_mutexLock) != 0{nRet=%d}\n",nRet);
        }
        m_bInited = false;
    }
}

bool CLog::Init(const char * pchFile        // log file name(when bNameUseDate=true,this is not all)
               ,const bool bOutputScreen    // wheather mess output to screen
               ,const Level enuLogLevel     // log to file's lowwest level,valid is:Info,Warn,Error(Info is all)
               ,const bool bNameUseDate     // wheather use date as part of filename,'true' cause use anew file per day
               ,const int nOpenFlag         // append open flag, valid is:O_SYNC,O_APPEND
               )
{
    // m_mutexLock
    int nRet = pthread_mutex_init(&m_mutexLock, NULL);
    if( nRet != 0 )
    {//log
        printf("pthread_mutex_init(&m_mutexLock, NULL) != 0 {nRet=%d,&m_mutexLock=%8p}\n",nRet,&m_mutexLock);
        return false;
    }
    // save paramter
    sprintf(m_pchFile,pchFile);
    m_bOutputScreen = bOutputScreen;
    m_enuLogLevel = enuLogLevel;
    m_bNameUseDate = bNameUseDate;
    m_nOpenFlag = O_WRONLY|O_CREAT|((O_SYNC|O_APPEND)&nOpenFlag);   // O_WRONLY|O_CREAT|O_SYNC|O_APPEND
    // succeed
    m_bInited = true;
    bool bRet = Log(System,__FILE__,__LINE__,"Logging System Started!");
    return bRet;
}

bool CLog::Lock()
{
    if( m_bInited == false )
    {
        printf("{CLog::Lock()}m_blnInited == false\n");
        return false;
    }
    int nRet = pthread_mutex_lock(&m_mutexLock);
    if( nRet != 0 )
    {
        printf("{CLog::Lock()}pthread_mutex_lock(&m_mutexLock) != 0(=%d)\n",nRet);
        return false;
    }
    else
        return true;
}

bool CLog::Unlock()
{
    if( m_bInited == false )
    {
        printf("{CLog::Unlock()}m_blnInited == false\n");
        return false;
    }
    int nRet = pthread_mutex_unlock(&m_mutexLock);
    if( nRet != 0 )
    {
        printf("{CLog::Unlock()}pthread_mutex_unlock(&m_mutexLock) != 0(=%d)\n",nRet);
        return false;
    }
    else
        return true;
}

bool CLog::SetOutputScreen(bool bOut)
{
    if( m_bInited == false )
    {
        printf("{CLog::SetOutputScreen()}m_blnInited == false\n");
        return false;
    }
    if( Lock() == false )
    {
        printf("{CLog::SetOutputScreen()}Lock() == false\n");
        return false;
    }
    //
    m_bOutputScreen = bOut;
    //
    if( Unlock() == false )
    {
        printf("{CLog::SetOutputScreen()}Unlock() == false\n");
        return false;
    }
    return true;
}

bool CLog::SetPrintMicroseconds(bool bOut)
{
    if( m_bInited == false )
    {
        printf("{CLog::SetOutputScreen()}m_blnInited == false\n");
        return false;
    }
    if( Lock() == false )
    {
        printf("{CLog::SetOutputScreen()}Lock() == false\n");
        return false;
    }
    //
    m_bPrintMicroseconds = bOut;
    //
    if( Unlock() == false )
    {
        printf("{CLog::SetOutputScreen()}Unlock() == false\n");
        return false;
    }
    return true;
}

bool CLog::LockFile(const int nFds,const int nCmd)const
{
    struct flock lock;
    lock.l_type   = nCmd;
    lock.l_whence = SEEK_SET;
    lock.l_start  = 0;
    lock.l_len    = 0;
    if(::fcntl(nFds,F_SETLK,&lock) == -1)
    {
        printf("[CLog::LockFile()] ::fcntln(Fds,F_SETLK,&lock) == -1{nFds=%d,F_SETLK=0x%x,&lock=%8p(lock.l_type=%d)}\n",nFds,F_SETLK,&lock,lock.l_type);
        //
        int     err_no = errno;
        const char * err_msg = strerror(err_no);

        printf("[CLog::LockFile()] error info {err_no=%d,err_msg='%s'}\n",err_no,err_msg);
        return false;
    }
    return true;
}

bool CLog::ReOpenFile(const CComDateTime & dtCur)
{
    // create pFileName
    int new_fd = -1;
    char pFileName[PATH_MAX];
    sprintf(pFileName,"%s_%02d%02d%02d.log", m_pchFile, (dtCur.GetYear() % 100),
            dtCur.GetMonth(),dtCur.GetDay());

    // open
    new_fd = ::open(pFileName,m_nOpenFlag,0644);
    if (new_fd == -1) {
        printf("::open(pFileName,m_nOpenFlag,0777) == -1 {pFileName='%s',m_nOpenFlag=0x%x}\n",pFileName,m_nOpenFlag);
        return false;
    }

    if (m_nFds >= 0) {
        close(m_nFds);
    }
    m_nFds = new_fd;

    // lock file
    if( LockFile(m_nFds,F_WRLCK) == false )
    {
        printf("[CLog::ReOpenFile] LockFile(m_nFds,F_WRLCK) == false {m_nFds=%d,F_WRLCK=%d}\n",m_nFds,F_WRLCK);
        printf("[CLog::ReOpenFile] possible because:\n");
        printf("[CLog::ReOpenFile]  1.server already running\n");
        printf("[CLog::ReOpenFile]  2.logfile is locked by another process(logfile='%s')\n",pFileName);
        printf("[CLog::ReOpenFile] solution:\n");
        printf("[CLog::ReOpenFile]  1.stop running server first,and retry\n");
        printf("[CLog::ReOpenFile]  2.find locked process,stop it,and retry\n");
        return false;
    }
    if( LockFile(m_nFds,F_UNLCK) == false )
    {
        printf("[CLog::ReOpenFile] LockFile(m_nFds,F_UNLCK) == false {m_nFds=%d,F_UNLCK=%d}\n",m_nFds,F_UNLCK);
        return false;
    }
    //
    int ec = ::lseek(m_nFds,0,SEEK_END);
    if (ec == -1)
    {
        printf("::lseek(m_nFds,0,SEEK_END) == -1 {m_nFds=%d}\n",m_nFds);
        return false;
    }

    realpath(pFileName, m_filepath);

    return true;
}

bool CLog::_Log(const Level enuLevel, const char * pFile, const int intLineNo, const char *pMsg)
{
    if (enuLevel < m_enuLogLevel) {
        return true;
    }

    return _Log(enuLevel, "[%16s][%8d] %s", pFile, intLineNo, pMsg);
}

bool CLog::Log(const Level enuLevel, const char * pFile, const int intLineNo, const char *pFormat,va_list arglist)
{
    if( enuLevel < m_enuLogLevel )
    {// not need log to file(not modfy m_enuLogLevel,so need not lock)
        return true;
    }
    //
    char    buffer[8192];
    vsnprintf(buffer,sizeof(buffer),pFormat,arglist);
    return _Log(enuLevel, pFile, intLineNo, buffer);
}

bool CLog::Log(const Level enuLevel, const char * pFile, const int intLineNo, const char *pFormat, ...) {
    if (enuLevel < m_enuLogLevel) {// not need log to file(not modfy m_enuLogLevel,so need not lock)
        return true;
    }
    va_list arglist;
    va_start(arglist, pFormat);
    char buffer[8192] = {0};
    vsnprintf(buffer, sizeof(buffer), pFormat, arglist);
    bool bRet = _Log(enuLevel, pFile, intLineNo, buffer);
    va_end(arglist);
    return bRet;
}

bool CLog::_Log(const Level enuLevel, const char * pFile, const char * pFunc, const char *pMsg)
{
    if (enuLevel < m_enuLogLevel) {
        return true;
    }

    return _Log(enuLevel, "[%16s][%8s] %s", pFile, pFunc, pMsg);
}

bool CLog::Log(const Level enuLevel, const char * pFile, const char * pFunc, const char *pFormat,va_list arglist)
{
    if (enuLevel < m_enuLogLevel) {
        return true;
    }

    char    buffer[8192];
    vsnprintf(buffer,sizeof(buffer),pFormat,arglist);
    return _Log(enuLevel,pFile,pFunc,buffer);
}

bool CLog::Log(const Level enuLevel, const char * pFile, const char * pFunc, const char *pFormat, ...) {
    if (enuLevel < m_enuLogLevel) {
        return true;
    }
    va_list arglist;
    va_start(arglist, pFormat);
    char buffer[8192] = {0};
    vsnprintf(buffer, sizeof(buffer), pFormat, arglist);
    bool bRet = _Log(enuLevel,pFile,pFunc,buffer);
    va_end(arglist);
    return bRet;
}

bool CLog::_Log(const Level enuLevel, const char *fmt, ...)
{
    if (enuLevel < m_enuLogLevel) {
        return true;
    }

    const char *pLevel = NULL;
    switch (enuLevel)
    {
    case Info:
        pLevel = "I";
        break;
    case Warn:
        pLevel = "W";
        break;
    case Error:
        pLevel = "E";
        break;
    case Status:
        pLevel = "S";
        break;
    default:
        pLevel = "U";
        break;
    }

    CComDateTime dtCur = CComDateTime::GetCurrentTime();
    char buf[8192];
    int offset = 0;

    if (m_bPrintMicroseconds == true) {
        timeval sNow;
        gettimeofday(&sNow,NULL);
        offset = snprintf(buf, sizeof(buf), "[%02d:%02d:%02d:%06d][%s][%8lx]",
            dtCur.GetHour(), dtCur.GetMinute(), dtCur.GetSecond(),
            (int) sNow.tv_usec, pLevel, pthread_self()
        );
    } else {
        offset = snprintf(buf, sizeof(buf), "[%02d:%02d:%02d][%s][%8lx]",
            dtCur.GetHour(), dtCur.GetMinute(), dtCur.GetSecond(),
                pLevel, pthread_self()
        );
    }

    va_list arglist;
    va_start(arglist, fmt);
    int v = vsnprintf(buf + offset, sizeof(buf) - offset, fmt, arglist);
    va_end(arglist);

    if (v < (int) (sizeof(buf) - offset)) {
        snprintf(buf + offset + v, sizeof(buf) - offset + v, "\n");
    } else {
        buf[sizeof(buf) - 2] = '\n';
        buf[sizeof(buf) - 1] = '\0';
    }

    // Limit the lock scope. The following action fails rarely.
    if (Lock() == false) {
        return false;
    }

    if (m_nFds < 0 || dtCur.GetDay() != m_LastOpenTime.GetDay()) {
        if (ReOpenFile(dtCur) == false) {
            Unlock();
            return false;
        } else {
            m_LastOpenTime = dtCur;
        }
    }

    Write(buf);

    return Unlock();
}

bool CLog::DumpBuf(const Level enuLevel
                ,const char *pchFile
                ,const char *pchFunc
                ,const char *pchTitle
                ,const char *pchBuf
                ,const int intBufSize
                ,const int intDumpSize
                )
{
    if( enuLevel < m_enuLogLevel )
    {// not need log to file(not modfy m_enuLogLevel,so need not lock)
        return true;
    }
    //
    if( Lock() == false )
    {
        return false;
    }
    //
    //sleep(10000);       // for test
    // ��¼����
    const char *pLevel = NULL;
    switch( enuLevel )
    {
    case Info:
        pLevel = "I";
        break;
    case Warn:
        pLevel = "W";
        break;
    case Error:
        pLevel = "E";
        break;
    case Status:
        pLevel = "S";
        break;
    //case System:
    //    pLevel = "S";
    //    break;
    }
    // ��¼ʱ��
    CComDateTime dtCur = CComDateTime::GetCurrentTime();

    if (m_nFds < 0 ||
        dtCur.GetDay() != m_LastOpenTime.GetDay())
    {
        if (ReOpenFile(dtCur) == false) {
            return false;
        } else {
            m_LastOpenTime = dtCur;
        }
    }
    //
    char buf[8192];
    if( m_bPrintMicroseconds == true )
    {
        timeval sNow;
        gettimeofday(&sNow,NULL);
        snprintf(buf,sizeof(buf),"[%02d:%02d:%02d:%06d][%s][%8lx][%16s][%8s] %s{pchBuf=%p,intBufSize=%d,intDumpSize=%d}\n",
                            dtCur.GetHour(),
                            dtCur.GetMinute(),
                            dtCur.GetSecond(),
                            (int)sNow.tv_usec,
                            pLevel,
                            /*getpid(),*/pthread_self(),
                            pchFile,
                            pchFunc,
                            pchTitle,
                            pchBuf,
                            intBufSize,
                            intDumpSize
                        );
    }
    else
    {
        snprintf(buf,sizeof(buf),"[%02d:%02d:%02d][%s][%8lx][%16s][%8s] %s{pchBuf=%p,intBufSize=%d,intDumpSize=%d}\n",
                            dtCur.GetHour(),
                            dtCur.GetMinute(),
                            dtCur.GetSecond(),
                            pLevel,
                            /*getpid(),*/pthread_self(),
                            pchFile,
                            pchFunc,
                            pchTitle,
                            pchBuf,
                            intBufSize,
                            intDumpSize
                        );
    }
    Write(buf);
    //
    snprintf(buf,sizeof(buf),"---------------------------------------------------------\n");
    Write(buf);
    snprintf(buf,sizeof(buf),"pchBuf=%8p,nSize=%d\n",pchBuf,intBufSize);
    Write(buf);
    snprintf(buf,sizeof(buf),"---------------------------------------------------------\n");
    Write(buf);
    snprintf(buf,sizeof(buf),"  |0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f--------\n");
    Write(buf);
    //
    int intActualSize = intDumpSize;
    if( intActualSize < 16 )
    {
        intActualSize = 16;
    }
    else if( intActualSize > 4096 )
    {
        intActualSize = 4096;
    }
    //
    char pchLineBuf[1024];
    int i;
    for( i = 0 ; (i < intBufSize) && (i < intActualSize) ; i++ )
    {
        sprintf(pchLineBuf + ((i%16) * 3),"%02x ",(const unsigned char)pchBuf[i]);
        if( i%16 == 15 )
        {
            snprintf(buf,sizeof(buf),"%02x|%s\n",i/16,pchLineBuf);
            Write(buf);
        }
    }
    if( i%16 != 0 )
    {
        snprintf(buf,sizeof(buf),"%02x|%s\n",i/16,pchLineBuf);
        Write(buf);
    }
    snprintf(buf,sizeof(buf),"---------------------------------------------------------\n");
    Write(buf);
    // pMsg
    //Write(pMsg);
    // "\n"
    //const char* pNewLine = "\n";
    //Write(pNewLine);
    //
    if( Unlock() == false )
    {
        return false;
    }
    return true;
}

bool CLog::Write(const char* pBuf)
{
    int nRet = -1;
    nRet = ::write(m_nFds,pBuf,strlen(pBuf));
    if( nRet == -1 )
    {
        printf("::write(m_nFds,pBuf,strlen(pBuf)) == -1 {errno=%d,m_nFds=%d,pBuf=%8p,strlen(pBuf)=%d}\n",errno,m_nFds,pBuf,strlen(pBuf));
        return false;
    }
    if( nRet != (int)strlen(pBuf) )
    {
        printf("Not all write to file {nRet=%d,strlen(pBuf)=%d}\n",nRet,strlen(pBuf));
        return false;
    }
    if( m_bOutputScreen == true )
        ::write(1,pBuf,strlen(pBuf));
    return true;
}

CLock CLog::stdout_lock;

void CLog::Log2Console(const Level enuLevel, const char * pFile, const char *func, const char *pFormat, va_list arglist)
{
    const char *pLevel = NULL;
    switch (enuLevel) {
    case Info:
        pLevel = "I";
        break;
    case Warn:
        pLevel = "W";
        break;
    case Error:
        pLevel = "E";
        break;
    case Status:
        pLevel = "S";
        break;
    }

    CAutoLock guard(stdout_lock);

    CComDateTime dtCur = CComDateTime::GetCurrentTime();

    char buf[8192];
    timeval sNow;
    gettimeofday(&sNow, NULL);
    int len = snprintf(buf,sizeof(buf),"[%02d:%02d:%02d:%06d][%s][%8lx][%16s][%s] ",
                           dtCur.GetHour(),
                           dtCur.GetMinute(),
                           dtCur.GetSecond(),
                           (int)sNow.tv_usec,
                           pLevel,
                           pthread_self(),
                           pFile,
                           func);

    len += vsnprintf(buf + len, sizeof(buf) - len, pFormat, arglist);
    len += snprintf(buf + len, sizeof(buf) - len, "\n");

    write(STDOUT_FILENO, buf, len);
}

void CLog::Log2Console(const Level enuLevel, const char * pFile, int lineno, const char *pFormat, va_list arglist)
{
    const char *pLevel = NULL;
    switch (enuLevel) {
    case Info:
        pLevel = "I";
        break;
    case Warn:
        pLevel = "W";
        break;
    case Error:
        pLevel = "E";
        break;
    case Status:
        pLevel = "S";
        break;
    }

    CAutoLock guard(stdout_lock);

    CComDateTime dtCur = CComDateTime::GetCurrentTime();

    char buf[8192];
    timeval sNow;
    gettimeofday(&sNow, NULL);
    int len = snprintf(buf,sizeof(buf),"[%02d:%02d:%02d:%06d][%s][%8lx][%16s][%d] ",
                           dtCur.GetHour(),
                           dtCur.GetMinute(),
                           dtCur.GetSecond(),
                           (int)sNow.tv_usec,
                           pLevel,
                           pthread_self(),
                           pFile,
                           lineno);

    len += vsnprintf(buf + len, sizeof(buf) - len, pFormat, arglist);
    len += snprintf(buf + len, sizeof(buf) - len, "\n");

    write(STDOUT_FILENO, buf, len);
}

int CLog::GetLogLevel()
{
    if (s_pLog) {
        return s_pLog->m_enuLogLevel;
    } else {
        return CLog::Info;
    }
}

}// namespace BackCom
