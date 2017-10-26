#include "comapi.h"

#include <string.h>     //char *strerror(int errnum);
#include <stdio.h>

namespace BackCom {

CComApi::CComApi() {
}

CComApi::~CComApi() {
}

bool CComApi::Log(CLog::Level enuLevel, const int intLineNo, const char *pFormat, ...) {
    bool bRet(true);
    va_list arglist;
    va_start(arglist, pFormat);
    if (CLog::s_pLog != NULL) {
        bRet = CLog::s_pLog->Log(enuLevel,"CComApi", intLineNo, pFormat, arglist);
    }
    va_end(arglist);
    return bRet;
}

bool CComApi::_MakeAddr(struct sockaddr_in & addr,const char *hostname, const unsigned short int nPort,int *pErrCode)
{
    struct hostent *hostinfo;

    hostinfo = ::gethostbyname (hostname);
    if (hostinfo == NULL)
    {
        int     err_no = h_errno;
        char    err_msg[1024];
        // set return errcode
        if( pErrCode != NULL )
            *pErrCode = err_no;
        // check errcode
        switch( err_no )
        {
        case HOST_NOT_FOUND:
            strcpy(err_msg,"The specified host is unknown.");
            break;
        case NO_ADDRESS:
            //case  NO_DATA:// == NO_ADDRESS
            strcpy(err_msg,"The requested name is valid but does not have an IP address.");
            break;
        case NO_RECOVERY:
            strcpy(err_msg,"A non-recoverable name server error occurred.");
            break;
        case TRY_AGAIN:
            strcpy(err_msg,"A temporary error occurred on an authoritative name server.  Try again later.");
            break;
        default:
            strcpy(err_msg,"Unknown error.");
            break;
        }
        Log(CLog::Error,__LINE__,"gethostbyname(hostname)==NULL{hostname=%s,h_errno=%d,err_msg=%s}",hostname,err_no,err_msg);
        return false;
    }
    struct in_addr *ptr = (struct in_addr *)(hostinfo->h_addr_list[0]);
    return MakeAddr(addr,*ptr,nPort,pErrCode);
}

bool CComApi::INetAddr(struct sockaddr_in & addr,const char *pchIp, const unsigned short int nPort,int *pErrCode)
{
    int nRet = -1;
    nRet = ::inet_addr(pchIp);
    if (nRet == -1)
    {
        Log(CLog::Error,__LINE__,"inet_addr(pchIp)==-1{nRet=%d,pchIp=%s}",nRet,pchIp);
        return false;
    }
    struct in_addr sin_addr;
    sin_addr.s_addr = nRet;
    return MakeAddr(addr,sin_addr,nPort,pErrCode);
}

bool CComApi::MakeAddr(struct sockaddr_in & addr,const struct in_addr sin_addr, const unsigned short int nPort,int */*pErrCode*/)
{
    addr.sin_family = AF_INET;
    addr.sin_port = htons (nPort);
    addr.sin_addr = sin_addr;
    return true;
}

bool CComApi::Socket(int & nSocket,const int domain,const int type,const int protocol,int *pErrCode)
{
    nSocket = ::socket(domain,type,protocol);
    if (nSocket == -1)
    {
        int     err_no = errno;
        const char * err_msg = strerror(err_no);
        if( err_msg == NULL )
            err_msg = "Unknown error.";
        // set return errcode
        if( pErrCode != NULL )
            *pErrCode = err_no;
        //
        Log(CLog::Error,__LINE__,"socket(domain,type,protocol)==-1{domain=%d,type=%d,protocol=%d,errno=%d,err_msg=%s}"
            ,domain,type,protocol,err_no,err_msg);
        return false;
    }
    return true;
}

bool CComApi::Connect(const struct sockaddr_in & addr,const int nSocket,int *pErrCode)
{
    while(true)
    {
        int nRet = ::connect(nSocket, (const struct sockaddr *) &addr, sizeof(sockaddr_in));
        if( nRet == -1 )
        {
            int     err_no = errno;
            if( err_no == EINTR )
            {
                Log(CLog::Warn,__LINE__,"err_no == EINTR,Api will be re-started!{errno=%d}",errno);
                continue;
            }
            const char * err_msg = strerror(err_no);
            if( err_msg == NULL )
                err_msg = "Unknown error.";
            // set return errcode
            if( pErrCode != NULL )
                *pErrCode = err_no;
            Log(CLog::Warn,__LINE__,"connect(nSocket,&addr,size)==-1{nSocket=%d,&addr=%08p,size=%d,errno=%d,err_msg=%s}"
                ,nSocket,&addr,sizeof(sockaddr_in),err_no,err_msg);
            return false;
        }
        return true;
    }
}

bool CComApi::Bind(const int nSocket,const struct sockaddr * pAddr,const int nAddSize,int *pErrCode)
{
    int nRet = ::bind(nSocket, pAddr, nAddSize);
    if( nRet == -1 )
    {
        int     err_no = errno;
        const char * err_msg = strerror(err_no);
        if( err_msg == NULL )
            err_msg = "Unknown error.";
        // set return errcode
        if( pErrCode != NULL )
            *pErrCode = err_no;
        //
        Log(CLog::Error,__LINE__,"bind(nSocket, pAddr, nAddSize)==-1{nSocket=%d,pAddr=%08p,nAddSize=%d,errno=%d,err_msg=%s}"
            ,nSocket,pAddr,nAddSize,err_no,err_msg);
        return false;
    }
    return true;
}

bool CComApi::Listen(const int nSocket,const int nBacklog,int *pErrCode)
{
    int nRet = ::listen(nSocket, nBacklog);
    if( nRet == -1 )
    {
        int     err_no = errno;
        const char * err_msg = strerror(err_no);
        if( err_msg == NULL )
            err_msg = "Unknown error.";
        // set return errcode
        if( pErrCode != NULL )
            *pErrCode = err_no;
        //
        Log(CLog::Error,__LINE__,"listen(nSocket, nBacklog)==-1{nSocket=%d,nBacklog=%d,errno=%d,err_msg=%s}"
            ,nSocket,nBacklog,err_no,err_msg);
        return false;
    }
    return true;
}

bool CComApi::Accept(int & nNewSocket,const int nSocket,struct sockaddr* pAddr,int * pAddSize,int *pErrCode)
{
    while(true)
    {
        socklen_t socklen = (socklen_t)*pAddSize;
        int nRet = ::accept(nSocket, pAddr, &socklen);
        if( nRet == -1 )
        {
            int     err_no = errno;
            if( err_no == EINTR )
            {
                Log(CLog::Warn,__LINE__,"err_no == EINTR,Api will be re-started!{err_no=%d,EINTR=%d}",err_no,EINTR);
                continue;
            }
            if( err_no == ECONNABORTED )
            {
                Log(CLog::Warn,__LINE__,"err_no == ECONNABORTED,Api will be re-started!{err_no=%d,ECONNABORTED=%d}",err_no,ECONNABORTED);
                continue;
            }
            const char * err_msg = strerror(err_no);
            if( err_msg == NULL )
                err_msg = "Unknown error.";
            // set return errcode
            if( pErrCode != NULL )
                *pErrCode = err_no;
            *pAddSize = (int)socklen;
            if(err_no != EMFILE && err_no != ENFILE) {
                Log(CLog::Error,__LINE__,"::accept(nSocket,pAddr,pAddSize)==-1{nSocket=%d,pAddr=%08p,pAddSize=%08p,errno=%d,err_msg=%s}"
                        ,nSocket,pAddr,pAddSize,err_no,err_msg);
            }
            return false;
        }
        *pAddSize = (int)socklen;
        nNewSocket = nRet;
        return true;
    }
}

bool CComApi::SetSockOpt(const int nSocket,const int nLevel,const int nOptName,const void *pOptVal,const int nOptLen,int *pErrCode)
{
    int nRet = ::setsockopt(nSocket,nLevel,nOptName,pOptVal,nOptLen);
    if( nRet == -1 )
    {
        int     err_no = errno;
        const char * err_msg = strerror(err_no);
        if( err_msg == NULL )
            err_msg = "Unknown error.";
        // set return errcode
        if( pErrCode != NULL )
            *pErrCode = err_no;
        //
        Log(CLog::Error,__LINE__,"setsockopt(nSocket,nLevel,nOptName,pOptVal,nOptLen)==-1{nSocket=%d,nLevel=%d,nOptName=%d,pOptVal=%08p,nOptLen=%d,errno=%d,err_msg=%s}"
            ,nSocket,nLevel,nOptName,pOptVal,nOptLen,errno,err_msg);
        return false;
    }
    return true;
}

bool CComApi::Close(const int nFds,int *pErrCode)
{
    while(true)
    {
        int nRet = ::close(nFds);
        if( nRet == -1 )
        {
            int     err_no = errno;
            if( err_no == EINTR )
            {
                Log(CLog::Warn,__LINE__,"err_no == EINTR,Api will be re-started!{errno=%d}",errno);
                continue;
            }
            const char * err_msg = strerror(err_no);
            if( err_msg == NULL )
                err_msg = "Unknown error.";
            // set return errcode
            if( pErrCode != NULL )
                *pErrCode = err_no;
            Log(CLog::Error,__LINE__,"close(nFds)==-1{nFds=%d,errno=%d,err_msg=%s}"
                ,nFds,err_no,err_msg);
            return false;
        }
        return true;
    }
}

bool CComApi::Recv(const int nSocket,void *pBuf,int & nLen,const int nFlags,int *pErrCode)
{
    while(true)
    {
        int nRet = ::recv(nSocket,pBuf,nLen,nFlags);
        if( nRet == -1 )
        {
            int     err_no = errno;
            if( err_no == EINTR )
            {
                Log(CLog::Warn,__LINE__,"err_no == EINTR,Api will be re-started!{errno=%d}",errno);
                continue;
            }
            const char * err_msg = strerror(err_no);
            if( err_msg == NULL )
                err_msg = "Unknown error.";
            // set return errcode
            if( pErrCode != NULL )
                *pErrCode = err_no;
            Log(CLog::Error,__LINE__,"recv(nSocket,pBuf,nLen,nFlags)==-1{nSocket=%d,pBuf=%08p,nLen=%d,nFlags=%d,errno=%d,err_msg=%s}"
                ,nSocket,pBuf,nLen,nFlags,err_no,err_msg);
            return false;
        }
        nLen = nRet;
        return true;
    }
}

bool CComApi::Send(const int nSocket,const void *pBuf,int & nLen,const int nFlags,int *pErrCode)
{
    while(true)
    {
        int nRet = ::send(nSocket,pBuf,nLen,nFlags);
        if( nRet == -1 )
        {
            int     err_no = errno;
            if( err_no == EINTR )
            {
                Log(CLog::Warn,__LINE__,"err_no == EINTR,Api will be re-started!{errno=%d}",errno);
                continue;
            }
            if( err_no == EWOULDBLOCK )
            {
                int intFcntlRet = ::fcntl(nSocket,F_GETFL,0);
                if( intFcntlRet == -1 )
                {
                    int     fcntl_err_no = errno;
                    const char * err_msg = strerror(fcntl_err_no);
                    if( err_msg == NULL )
                        err_msg = "Unknown error.";
                    Log(CLog::Error,__LINE__,"::fcntl(nSocket,F_GETFL,0)==-1{nSocket=%d,errno=%d,err_msg=%s}"
                                            ,nSocket,fcntl_err_no,err_msg);
                }
                else
                {// succeed
                    if( (intFcntlRet&O_NONBLOCK) == O_NONBLOCK )
                    {// in nonblocking mode ,ignore it
                        Log(CLog::Info,__LINE__,"send()==-1,and errno==EWOULDBLOCK,because now in nonblocking mode ,we ignore it,and return true('nLen' will set to 0),more info see below!{nSocket=%d}"
                                                ,nSocket);
                        Log(CLog::Info,__LINE__,"send(nSocket,pBuf,nLen,nFlags)==-1{nSocket=%d,pBuf=%08p,nLen=%d,nFlags=%d,errno=%d}"
                                                ,nSocket,pBuf,nLen,nFlags,err_no);
                        nLen = 0;
                        return true;
                    }
                    // otherwise,return false,
                }
            }
            const char * err_msg = strerror(err_no);
            if( err_msg == NULL )
                err_msg = "Unknown error.";
            // set return errcode
            if( pErrCode != NULL )
                *pErrCode = err_no;
            Log(CLog::Error,__LINE__,"send(nSocket,pBuf,nLen,nFlags)==-1{nSocket=%d,pBuf=%08p,nLen=%d,nFlags=%d,errno=%d,err_msg=%s}"
                                    ,nSocket,pBuf,nLen,nFlags,err_no,err_msg);
            return false;
        }
        nLen = nRet;
        return true;
    }
}

bool CComApi::Select(int & nRetCount,const int nFds,fd_set *pRead,fd_set *pWrite,fd_set *pError,struct timeval *pTimeout,int *pErrCode)
{
    while(true)
    {
        int nRet = ::select(nFds,pRead,pWrite,pError,pTimeout);
        if( nRet == -1 )
        {
            int     err_no = errno;
            if( err_no == EINTR )
            {
                Log(CLog::Warn,__LINE__,"err_no == EINTR,Api will be re-started!{errno=%d}",errno);
                continue;
            }
            const char * err_msg = strerror(err_no);
            if( err_msg == NULL )
                err_msg = "Unknown error.";
            // set return errcode
            if( pErrCode != NULL )
                *pErrCode = err_no;
            Log(CLog::Error,__LINE__,"select(nFds,pRead,pWrite,pError,pTimeout)==-1{nFds=%d,pRead=%08p,pWrite=%08p,pError=%08p,pTimeout=%08p,errno=%d,err_msg=%s}"
                ,nFds,pRead,pWrite,pError,pTimeout,err_no,err_msg);
            return false;
        }
        nRetCount = nRet;
        return true;
    }
}

bool CComApi::Open(int & nFds,const char *pPath,const int nFlag,const mode_t nMode,int *pErrCode)
{
    while(true)
    {
        int nRet = ::open(pPath,nFlag,nMode);
        if( nRet == -1 )
        {
            int     err_no = errno;
            if( err_no == EINTR )
            {
                Log(CLog::Warn,__LINE__,"err_no == EINTR,Api will be re-started!{errno=%d}",errno);
                continue;
            }
            const char * err_msg = strerror(err_no);
            if( err_msg == NULL )
                err_msg = "Unknown error.";
            // set return errcode
            if( pErrCode != NULL )
                *pErrCode = err_no;
            Log(CLog::Error,__LINE__,"open(pPath,nFlag,nMode)==-1{pPath='%s',nFlag=%x,nMode=%x,errno=%d,err_msg=%s}"
                ,pPath,nFlag,nMode,errno,err_msg);
            return false;
        }
        nFds = nRet;
        return true;
    }
}

bool CComApi::Write(const int nFds,const void* pBuf,int & nSize,int *pErrCode)
{
    while(true)
    {
        int nRet = ::write(nFds,pBuf,nSize);
        if( nRet == -1 )
        {
            int     err_no = errno;
            if( err_no == EINTR )
            {
                Log(CLog::Warn,__LINE__,"err_no == EINTR,Api will be re-started!{errno=%d}",errno);
                continue;
            }
            const char * err_msg = strerror(err_no);
            if( err_msg == NULL )
                err_msg = "Unknown error.";
            // set return errcode
            if( pErrCode != NULL )
                *pErrCode = err_no;
            Log(CLog::Error,__LINE__,"write(nFds,pBuf,nSize)==-1{nFds=%d,pBuf=%08p,nSize=%d,errno=%d,err_msg=%s}"
                ,nFds,pBuf,nSize,errno,err_msg);
            return false;
        }
        nSize = nRet;
        return true;
    }
}

bool CComApi::Read(const int nFds,void *pBuf,int & nLen,int *pErrCode)
{
    while(true)
    {
        int nRet = ::read(nFds,pBuf,nLen);
        if( nRet == -1 )
        {
            int     err_no = errno;
            if( err_no == EINTR )
            {
                Log(CLog::Warn,__LINE__,"err_no == EINTR,Api will be re-started!{errno=%d}",errno);
                continue;
            }
            const char * err_msg = strerror(err_no);
            if( err_msg == NULL )
                err_msg = "Unknown error.";
            // set return errcode
            if( pErrCode != NULL )
                *pErrCode = err_no;
            Log(CLog::Error,__LINE__,"read(nFds,pBuf,nLen)==-1{nFds=%d,pBuf=%08p,nLen=%d,errno=%d,err_msg=%s}"
                ,nFds,pBuf,nLen,err_no,err_msg);
            return false;
        }
        nLen = nRet;
        return true;
    }
}

bool CComApi::Fcntl_Int(int & nRet,const int nFds,const int nCmd, const int nArg,int *pErrCode)
{
    while(true)
    {
        nRet = ::fcntl(nFds,nCmd,nArg);
        if( nRet == -1 )
        {
            int     err_no = errno;
            if( err_no == EINTR )
            {
                Log(CLog::Warn,__LINE__,"err_no == EINTR,Api will be re-started!{errno=%d}",errno);
                continue;
            }
            const char * err_msg = strerror(err_no);

            if( err_msg == NULL )
                err_msg = "Unknown error.";
            // set return errcode
            if( pErrCode != NULL )
                *pErrCode = err_no;
            Log(CLog::Error,__LINE__,"fcntl(nFds,nCmd,nArg)==-1{nFds=%d,nCmd=%x,nArg=%x,errno=%d,err_msg=%s}"
                ,nFds,nCmd,nArg,errno,err_msg);
            return false;
        }
        return true;
    }
}

bool CComApi::Fcntl_FLockP(int & nRet,const int nFds,const int nCmd,struct flock * pLock,int *pErrCode)
{
    while(true)
    {
        nRet = ::fcntl(nFds,nCmd,pLock);
        if( nRet == -1 )
        {
            int     err_no = errno;
            if( err_no == EINTR )
            {
                Log(CLog::Warn,__LINE__,"err_no == EINTR,Api will be re-started!{errno=%d}",errno);
                continue;
            }
            const char * err_msg = strerror(err_no);
            if( err_msg == NULL )
                err_msg = "Unknown error.";
            // set return errcode
            if( pErrCode != NULL )
                *pErrCode = err_no;
            Log(CLog::Error,__LINE__,"fcntl(nFds,nCmd,pLock)==-1{nFds=%d,nCmd=%x,pLock=%08p,errno=%d,err_msg=%s}"
                ,nFds,nCmd,pLock,errno,err_msg);
            return false;
        }
        return true;
    }
}

bool CComApi::Poll(int & nRetCount,struct pollfd pFds[], const nfds_t nFdsCount, const int nTimeout,int *pErrCode)
{
    while(true)
    {
        int nRet = ::poll(pFds,nFdsCount,nTimeout);
        if( nRet == -1 )
        {
            int     err_no = errno;
            if( err_no == EINTR )
            {
                Log(CLog::Warn,__LINE__,"err_no == EINTR,Api will be re-started!{errno=%d}",errno);
                continue;
            }
            const char * err_msg = strerror(err_no);
            if( err_msg == NULL )
                err_msg = "Unknown error.";
            // set return errcode
            if( pErrCode != NULL )
                *pErrCode = err_no;
            Log(CLog::Error,__LINE__,"poll(pFds,nFdsCount,nTimeout)==-1{pFds=%08p,nFdsCount=%d,nTimeout=%dms,errno=%d,err_msg=%s}"
                ,pFds,nFdsCount,nTimeout,err_no,err_msg);
            return false;
        }
        nRetCount = nRet;
        return true;
    }
}

bool CComApi::GetSockName(const int nSocket,struct sockaddr* pAddr,int * pAddSize,int *pErrCode)
{
    while(true)
    {
        socklen_t socklen = (socklen_t)*pAddSize;
        int nRet = ::getsockname(nSocket, pAddr, &socklen);
        if( nRet == -1 )
        {
            int     err_no = errno;
            const char * err_msg = strerror(err_no);
            if( err_msg == NULL )
                err_msg = "Unknown error.";
            // set return errcode
            if( pErrCode != NULL )
                *pErrCode = err_no;
            *pAddSize = (int)socklen;
            Log(CLog::Error,__LINE__,"::getsockname(nSocket,pAddr,pAddSize)==-1{nSocket=%d,pAddr=%08p,pAddSize=%08p,errno=%d,err_msg=%s}"
                ,nSocket,pAddr,pAddSize,err_no,err_msg);
            return false;
        }
        *pAddSize = (int)socklen;
        return true;
    }
}

bool CComApi::Input(const char* pchSrc, int & intVal)
{
    char *pch = (char*)&intVal;
#if defined(_BIG_ENDIAN)
    *(pch+0) = *(pchSrc+0);
    *(pch+1) = *(pchSrc+1);
    *(pch+2) = *(pchSrc+2);
    *(pch+3) = *(pchSrc+3);
#else
    *(pch+3) = *(pchSrc+0);
    *(pch+2) = *(pchSrc+1);
    *(pch+1) = *(pchSrc+2);
    *(pch+0) = *(pchSrc+3);
#endif
    return true;
}
}// namespace BackCom
