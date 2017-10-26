#ifndef CCOMAPI_H
#define CCOMAPI_H

#include <netinet/in.h>
#include <unistd.h>                 // close()
#include <errno.h>                  // errno
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <poll.h>

#include "log.h"

namespace BackCom
{
class CComApi
{
public:
    CComApi();
    ~CComApi();
    
    // log functions
private:
    static bool Log(CLog::Level enuLevel, const int intLineNo, const char *pFormat,...);
    
    // static function
public:
    static bool INetAddr(struct sockaddr_in & addr,const char *pchIp, const unsigned short int nPort,int *pErrCode=NULL);
    static bool _MakeAddr(struct sockaddr_in & addr,const char *hostname, const unsigned short int nPort,int *pErrCode=NULL);
    static bool MakeAddr(struct sockaddr_in & addr,const struct in_addr sin_addr, const unsigned short int nPort,int *pErrCode=NULL);
    static bool Socket(int & nSocket,const int domain,const int type,const int protocol,int *pErrCode=NULL);
    static bool Connect(const struct sockaddr_in & addr,const int nSocket,int *pErrCode=NULL) ;
    static bool Bind(const int nSocket,const struct sockaddr * pAddr,const int nAddSize,int *pErrCode=NULL) ;
    static bool Listen(const int nSocket,const int nBacklog,int *pErrCode=NULL) ;
    static bool Accept(int & nNewSocket,const int nSocket,struct sockaddr* pAddr,int * pAddSize,int *pErrCode=NULL) ;
    static bool Recv(const int nSocket,void *pBuf,int & nLen,const int nFlags,int *pErrCode=NULL);
    static bool Send(const int nSocket,const void *pBuf,int & nLen,const int nFlags,int *pErrCode=NULL);
    static bool SetSockOpt(const int nSocket,const int nLevel,const int nOptName,const void *pOptVal,const int nOptLen,int *pErrCode=NULL);
    //
    static bool Select(int & nRetCount,const int nFds,fd_set *pRead,fd_set *pWrite,fd_set *pError,struct timeval *pTimeout,int *pErrCode=NULL);
    static bool Poll(int & nRetCount,struct pollfd pFds[], const nfds_t nFdsCount, const int nTimeout,int *pErrCode=NULL);
    // file op
    static bool Open(int & nFds,const char *pPath,const int nFlag,const mode_t nMode,int *pErrCode=NULL);
    static bool Write(const int nFds,const void* pBuf,int & nSize,int *pErrCode=NULL);
    static bool Close(const int nFds,int *pErrCode=NULL);
    static bool Fcntl_Int(int & nRet,const int nFds,const int nCmd, const int nArg,int *pErrCode=NULL);
    static bool Fcntl_FLockP(int & nRet,const int nFds,const int nCmd,struct flock * pLock,int *pErrCode=NULL);
    static bool GetSockName(const int nSocket,struct sockaddr* pAddr,int * pAddSize,int *pErrCode=NULL) ;
    static bool Input(const char* pchSrc, int & intVal);
    static bool Read(const int nFds,void *pBuf,int & nLen,int *pErrCode=NULL);
};

}// namespace BackCom

#endif //#ifndef __APIFUNC_H
