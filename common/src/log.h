
#ifndef CLOG_H
#define CLOG_H

#include <stdarg.h>         // va_list
#include <pthread.h>        // pthread_mutexattr_t
#include <unistd.h>
#include <string>
#include <limits.h>
#include "lock.h"
#include "comdatetime.h"

namespace BackCom
{
class CLog
{
public:
    enum Level  {   Info = 1, Warn, Error, Status, System = Status };

public:
    CLog();
    virtual ~CLog();
    //
    bool Init(const char * pchFile              // log file name(when bNameUseDate=true,this is not all)
            ,const bool bOutputScreen = true    // wheather mess output to screen
            ,const Level enuLogLevel = Info     // log to file's lowwest level,valid is:Info,Warn,Error(Info is all)
            ,const bool bNameUseDate = true     // wheather use date as part of filename,'true' cause use anew file per day
            ,const int nOpenFlag = 0            // append open flag, valid is:O_SYNC,O_APPEND
              );
    //
    bool Log(const Level enuLevel, const char * pFile, const int intLineNo, const char *pFormat,...);
    bool Log(const Level enuLevel, const char * pFile, const int intLineNo, const char *pFormat,va_list arglist);
    bool Log(const Level enuLevel, const char * pFile, const char * pFunc, const char *pFormat,...);
    bool Log(const Level enuLevel, const char * pFile, const char * pFunc, const char *pFormat,va_list arglist);
    bool DumpBuf(const Level enuLevel
                ,const char *pchFile
                ,const char *chpFunc
                ,const char *pchTitle
                ,const char *pchBuf
                ,const int intBufSize
                ,const int intDumpSize
                );
    //
    bool SetOutputScreen(bool bOut);
    bool SetPrintMicroseconds(bool bOut);
    bool LockFile(const int nCmd) {   return LockFile(m_nFds,nCmd);    };

    std::string GetLogFilePath() {
        Lock();
        std::string path = m_filepath;
        Unlock();
        return path;
    }

    static void Log2Console(const Level enuLevel,
                            const char * pFile,
                            const char * pchFunc,
                            const char *pFormat,
                            va_list arglist);

    static void Log2Console(const Level enuLevel,
                            const char * pFile,
                            int lineno,
                            const char *pFormat,
                            va_list arglist);

    static int GetLogLevel();

private:
    bool _Log(const Level enuLevel, const char * pFile, const int intLineNo, const char *pMsg);
    bool _Log(const Level enuLevel, const char * pFile, const char * pFunc, const char *pMsg);

    bool _Log(const Level enuLevel, const char *fmt, ...);

    bool ReOpenFile(const CComDateTime & dtCur);
    bool Write(const char* pBuf);
    bool Lock();
    bool Unlock();
    bool LockFile(const int nFds,const int nCmd)const;

private:
    static CLock stdout_lock;

    char            m_pchFile[PATH_MAX];        // see Init()
    char            m_filepath[PATH_MAX];
    bool            m_bOutputScreen;        // see Init()
    bool            m_bPrintMicroseconds;   //
    Level           m_enuLogLevel;          // see Init()
    bool            m_bNameUseDate;         // see Init()
    int             m_nOpenFlag;            // see Init()
    CComDateTime   m_LastOpenTime;
    //
    int             m_nFds;
    bool            m_bInited;
    pthread_mutex_t m_mutexLock;

    //
public:
    static CLog *   s_pLog;
};

}// namespace BackCom
#endif //#ifndef __LOG_H
