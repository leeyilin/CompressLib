#ifndef CBASELOG_H
#define CBASELOG_H

#include <typeinfo>
using namespace std;
#include <cxxabi.h>

#include "log.h"

namespace BackCom
{
class CBaseLog
{
protected:
    CBaseLog(CLog * pLog = NULL  // [in ]指定打印日志的对象指针，缺省为NULL；
                                        //      取值为NULL表示不打印日志，非NULL表示通过该对象打印必要的日志信息
                   )
        : m_pLog(pLog)
    {
    }

    virtual ~CBaseLog()
    {
    }

    bool Init(CLog * pLog
             )
    {
        m_pLog = pLog;
        return true;
    }
protected:
    void Log(CLog::Level enuLevel, const char * pchFunc, const char *pFormat, ...) const {
        if (m_pLog != NULL) {
            int status = 0;
            char pchClassName[256] = {0};
            size_t intSize = sizeof(pchClassName);
            abi::__cxa_demangle(typeid(*this).name(), pchClassName, &intSize, &status);
            va_list arglist;
            va_start(arglist, pFormat);
            if (status == 0) {
                m_pLog->Log(enuLevel, pchClassName,pchFunc, pFormat, arglist);
            } else {
                m_pLog->Log(enuLevel, typeid(*this).name(), pchFunc, pFormat, arglist);
            }
            va_end(arglist);
        }
    }
    void DumpBuf(const CLog::Level enuLevel
                ,const char *pchFunc
                ,const char *pchTitle
                ,const char *pchBuf
                ,const int intBufSize
                ,const int intDumpSize
                )
    {
        if( m_pLog != NULL )
        {
            int status;
            char pchClassName[256];
            size_t intSize = sizeof(pchClassName);
            abi::__cxa_demangle(typeid(*this).name(), pchClassName, &intSize, &status);
            if( status == 0 )
            {
                m_pLog->DumpBuf(enuLevel,pchClassName,pchFunc, pchTitle,pchBuf,intBufSize,intDumpSize);
            }
            else
            {
                m_pLog->DumpBuf(enuLevel,typeid(*this).name(),pchFunc, pchTitle,pchBuf,intBufSize,intDumpSize);
            }
        }
    }

private:
    CLog* m_pLog;         // 用于打印日志的对象指针，取值为NULL表示不打印日志，非NULL表示通过该对象打印必要的日志信息
};

}// namespace BackCom
#endif
