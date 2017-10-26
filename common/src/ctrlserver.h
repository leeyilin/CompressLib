
#ifndef CCTRLSERVER_H
#define CCTRLSERVER_H

#include <vector>
#include <string>
using namespace std;

#include "thread.h"
#include "lock.h"
#include "log.h"

namespace BackCom
{
class CCtrlServer : public CThread
{
public:

public:
    CCtrlServer(CLog * pLog = NULL 
               );
    virtual ~CCtrlServer();
    //
    bool Init(const string & strIP
             ,const unsigned short shtPort
             ,const int intBackLog
             ,const bool blnReuseAddr
             );
    //
    bool StopThread();

    // Overload function
protected:
    virtual void OnCommand(int & intRetErrCode
                          ,string & strMessage
                          ,const vector<string> & vectParams
                          )const=0;
    virtual void Execute(void *);

    //
protected:
    void Log(CLog::Level enuLevel, const char * pchFunc, const char *pFormat,...) const;
    bool ProcessRequest(const int nSocket) const;
    bool RecvWithTimeOut(const int nSocket,char * pBuf,int & nCount,const int nTimeOut/* ms */)const;

    // Member
protected:
    CLog       *m_pLog; 
    //
    int         m_nSocket;
    int         m_nBacklog;
};

}// namespace BackCom

#endif 
