
#include <exception>
#include <typeinfo>
using namespace std;


#include "ctrlserver.h"

#include <string.h>     // memset , used by FD_ZERO
#include <arpa/inet.h>  // inet_ntoa

#include "comapi.h"
#include "pack.h"

namespace BackCom
{

CCtrlServer::CCtrlServer(CLog * pLog)
    :  m_pLog(pLog)
    , m_nSocket(-1)
    , m_nBacklog(0)
{
}

CCtrlServer::~CCtrlServer()
{
}

void CCtrlServer::Log(CLog::Level enuLevel, const char * pchFunc, const char *pFormat, ...) const {
    if (m_pLog != NULL) {
        va_list arglist;
        va_start(arglist, pFormat);
        m_pLog->Log(enuLevel,"CCtrlServer",pchFunc, pFormat, arglist);
        va_end(arglist);
    }
}

bool CCtrlServer::Init(const string & strIP
                      ,const unsigned short shtPort
                      ,const int intBackLog
                      ,const bool blnReuseAddr
                      )
{
    // new socket
    int nSocket = -1;
    if( CComApi::Socket(nSocket,PF_INET, SOCK_STREAM, 0) == false)
    {
        Log(CLog::Error,"Init","Socket(nSocket,PF_INET, SOCK_STREAM, 0) == false{nSocket=%d}",nSocket);
        return false;
    }
    // make addr
    struct sockaddr_in  sAddr;
    if( CComApi::INetAddr(sAddr,strIP.c_str(),shtPort) == false )
    {
        Log(CLog::Error,"Init","INetAddr(saddr,strIP.c_str(),shtPort) == false{strIP.c_str()='%s',shtPort=%d}",strIP.c_str(),shtPort);
        return false;
    }
    // bind
    int nOptVal = blnReuseAddr?1:0;
    if( CComApi::SetSockOpt(nSocket,SOL_SOCKET,SO_REUSEADDR,&nOptVal,sizeof(nOptVal)) == false )
    {
        Log(CLog::Error,"Init","SetSockOpt(nSocket,SOL_SOCKET,SO_REUSEADDR,&nOptVal,sizeof(nOptVal)) == false{nSocket=%d,SOL_SOCKET=%d,SO_REUSEADDR=%d,nOptVal=%d,sizeof(nOptVal)=%d}",nSocket,SOL_SOCKET,SO_REUSEADDR,&nOptVal,sizeof(nOptVal));
        return false;
    }
    if( CComApi::Bind(nSocket,(const struct sockaddr *)&sAddr,sizeof(sAddr)) == false )
    {
        int nPort = ntohs(sAddr.sin_port);
        const char * pchIP = inet_ntoa(sAddr.sin_addr);
        Log(CLog::Error,"Init","Bind(nSocket,&sAddr,nAddSize) == false{nSocket=%d,&sAddr=%08p(%s,%d),nAddSize=%d}",nSocket,&sAddr,pchIP,nPort,sizeof(sAddr));
        return false;
    }
    // set member , must do this before start thread
    m_nSocket = nSocket;
    m_nBacklog = intBackLog;
    //
    Log(CLog::Info,"Init","Succeed Info{m_nSocket=%d,m_nBacklog=%d}",nSocket,m_nBacklog);
    return true;
}

bool CCtrlServer::StopThread()
{
    if( m_nSocket != -1 )
    {
        if( CComApi::Close(m_nSocket) == false)
        {
            Log(CLog::Error,"StopThread","CComApi::Close(m_nSocket) == false{nSocket=%d}",m_nSocket);
        }
        else
        {
            Log(CLog::Info,"StopThread","CComApi::Close(m_nSocket) == true{nSocket=%d}",m_nSocket);
        }
        m_nSocket = -1;
    }
    Stop();
    void *pRet = NULL;
    Join(&pRet);
    Log(CLog::Status,"StopThread","Join(&pRet) return {pRet=%08p,this=%08p}",pRet,this);
    return true;
}

void CCtrlServer::Execute(void *)
{
    try
    {
    const int _const_timeout = 0 * 1000 + 100;    // 0s,100ms
    // listen
    if( CComApi::Listen(m_nSocket,m_nBacklog) == false )
    {
        Log(CLog::Error,"Execute","CComApi::Listen(m_nSocket,m_nBacklog) == false{m_nSocket=%d,m_nBacklog=%d}",m_nSocket,m_nBacklog);
        return;
    }
    // loop for accept
    struct sockaddr_in sClientAddr;
    int     nAddSize;
    int     nNewSocket;
    //
    const int _const_timeout_log = 60 ;                   // 60s
    time_t  tmLastLogActive = 0;
    //
    struct  pollfd pFds[1];
    const short shtReadEvents = POLLIN;
    //const short shtWriteEvents = POLLOUT;
    const short shtErrorEvents = POLLERR | POLLHUP | POLLNVAL;
    //const short shtReadWriteEvents = POLLIN | POLLOUT;
    for(;GetStopFlag() == false;)
    {
        // setup poll paramter
        pFds[0].fd      = m_nSocket;
        pFds[0].events  = shtReadEvents;
        pFds[0].revents = 0;
        int  nFdsCount = 0;
        // poll
        if( CComApi::Poll(nFdsCount,pFds,1,_const_timeout) == false )
        {
            Log(CLog::Error,"Execute","Poll(nFdsCount,pFds,1,_const_timeout) == false{nFdsCount=%d,pFds=%08p,_const_timeout=%d}",nFdsCount,pFds,_const_timeout);
            return ;
        }
        //
        if( true )
        {
            time_t tmNow = time(NULL);
            if( (tmNow - tmLastLogActive) >= _const_timeout_log)
            {//
                Log(CLog::Status,__func__,"I am active[CCtrlServer]{tmNow=%d,tmLastLogActive=%d,_const_timeout_log=%d,this=%08p}",tmNow,tmLastLogActive,_const_timeout_log,this);
                tmLastLogActive = tmNow;
            }
        }
        //
        if( nFdsCount == 0 )
        {// timeout
            continue;
        }
        // not timeout
        // process read ready sockets
        if( (pFds[0].revents & shtReadEvents) != 0 )
        {
            nAddSize = sizeof(sClientAddr);
            if( CComApi::Accept(nNewSocket,m_nSocket,(struct sockaddr*)&sClientAddr,&nAddSize) == false )
            {
                Log(CLog::Error,"Execute","Accept(nNewSocket,m_nSocket,&sClientAddr,&nAddSize)==false{nNewSocket=%d,m_nSocket=%d,&sClientAddr=%08p,&nAddSize=%08p}",nNewSocket,m_nSocket,&sClientAddr,&nAddSize);
                continue;
            }
            Log(CLog::Status,"Execute","Succeed Accept a new conn{IP='%s',port=%hd,nNewSocket=%d}"
                                        ,inet_ntoa(sClientAddr.sin_addr),ntohs(sClientAddr.sin_port),nNewSocket);
            if( ProcessRequest(nNewSocket) == false )
            {
                Log(CLog::Error,"Execute","ProcessRequest(nNewSocket) == false{nNewSocket=%d}",nNewSocket);
            }
            // Close socket
            if( CComApi::Close(nNewSocket) == false )
            {
                Log(CLog::Error,"Execute","CComApi::Close(nNewSocket)==false{nNewSocket=%d}",nNewSocket);
            }
            continue;
        }
        // process error sockets
        if( (pFds[0].revents & shtErrorEvents) != 0 )
        {
            Log(CLog::Error,"Execute","Listen Socket Error{m_nSocket=%d}",m_nSocket);
            return;
        }
    }
    // n. return
    Log(CLog::Status,"Execute","Finished [GetStopFlag()==true]{GetStopFlag()=%d,this=%08p}",GetStopFlag(),this);
    return;
    }
    catch(exception & e)
    {// 捕获 C++  标准异常，记录相关日志
        Log(CLog::Status,__func__,"catch(exception & e)!{type='%s',what='%s',this=%p,pThread=%p,type='%s'}",typeid(e).name(),e.what(),this,(CThread*)this,typeid(*this).name());
        // 我们捕获异常的目的是记录日志，现在我们重新抛出异常，让异常的处理继续进行
        throw;//重新抛出异常
    }
    catch(...)
    {// 捕获所有异常，记录相关日志
        Log(CLog::Status,__func__,"catch(...)!{this=%p,pThread=%p,type='%s'}",this,dynamic_cast<CThread*>(this),typeid(*this).name());
        // 我们捕获异常的目的是记录日志，现在我们重新抛出异常，让异常的处理继续进行
        throw;//重新抛出异常
    }
}

static CPack::ErrorCode
UnpackRequest(vector<string> & vect
             ,int & intPackNo
             ,unsigned short & shtVersion
             ,unsigned short & shtType
             ,const char *pchPack
             ,const int intPackSize
             )
{
    const int _const_pack_head_size = 12;
    int nTmp = _const_pack_head_size + 0;
    //
    int intCount = 0;
    nTmp += CPack::Input(pchPack+ nTmp,intCount);
    vect.resize(intCount);
    for(int i = 0 ; i < intCount ; ++i )
    {
        nTmp += CPack::Input(pchPack+ nTmp,vect[i]);
    }
    if( intPackSize < nTmp )
    {
        return CPack::Err_ContentErr;
    }
    return CPack::UnpackHead(nTmp,intPackNo,shtVersion,shtType,pchPack,intPackSize);
}

bool CCtrlServer::ProcessRequest(const int nSocket) const
{
    const int nTimeOut = 5 * 1000 * 1000;   // 5 second
    const int _const_recv_buf_size = 64*1024;
    char pBuf[_const_recv_buf_size];
    int intContentLen = 0;
    {
        const int _PackHeadSize = 12;   // pasksize(int)+packno(int)+ver(short)+packtype(short)
        int intPackSize = 0;
        while(true)
        {
            int nCount = max(intPackSize,_PackHeadSize) - intContentLen;
            //if( CComApi::Recv(nSocket,pBuf+intContentLen,nCount,0) == false )
            if( RecvWithTimeOut(nSocket,pBuf+intContentLen,nCount,nTimeOut) == false )
            {
                Log(CLog::Error,"ProcessRequest","RecvWithTimeOut(nSocket,pBuf,nCount,nTimeOut) == false{nSocket=%d,pBuf=%08p,nCount=%d,nTimeOut=%d}",nSocket,pBuf,nCount,nTimeOut);
                return false;
            }
            intContentLen += nCount;
            if( intContentLen >= _PackHeadSize )
            {
                if( intPackSize == 0 )
                {
                    CPack::Input(pBuf,intPackSize);
                    if(intPackSize>_const_recv_buf_size)
                    {
                        Log(CLog::Error,"ProcessRequest","[intPackSize>_const_recv_buf_size]{intPackSize=%d,_const_recv_buf_size=%d,intContentLen=%d}",intPackSize,_const_recv_buf_size,intContentLen);
                        return false;
                    }
                }
                if( intContentLen >= intPackSize )
                break;
            }
        }
    }
    // packet format:count(int)+string*count
    vector<string> vectParams;
    int intPackNo = 0;
    unsigned short shtVersion,shtType;
    CPack::ErrorCode errCode = UnpackRequest(vectParams,intPackNo,shtVersion,shtType,pBuf,intContentLen);
    if ( errCode != CPack::Err_NoErr)
    {
        Log(CLog::Error,__func__,"UnpackRequest(vectParams,intPackNo,shtVersion,shtType,pBuf,intContentLen) != Err_NoErr[errCode != CPack::Err_NoErr]!{errCode=%d, CPack::Err_NoErr=%d,vectParams.size()=%d,intPackNo=%d,shtVersion=%d,shtType=%d,pBuf=%p,intContentLen=%d}",errCode, CPack::Err_NoErr,vectParams.size(),intPackNo,shtVersion,shtType,pBuf,intContentLen);
        return false;
    }
    if( true )
    {
        string strCommand;
        for( int i = 0 ; i < (int)vectParams.size() ; ++i )
        {
            strCommand += vectParams[i] + ",";
        }
        Log(CLog::Status,__func__,"Request info!{strCommand='%s',vectParams.size()=%d,intPackNo=%d,shtVersion=%d,shtType=%d,pBuf=%p,intContentLen=%d}",strCommand.c_str(),vectParams.size(),intPackNo,shtVersion,shtType,pBuf,intContentLen);
    }
    //
    int intErrCode = 0;
    string strMessage;
    OnCommand(intErrCode,strMessage,vectParams);
    //
/*    const int _const_pack_head_size = 12;
    int intTmp;
    int intRetPacketSize = _const_pack_head_size + sizeof(int) + strMessage.size()+1;
    CPack::PackHead(pBuf,intTmp,intRetPacketSize,intPackNo,shtVersion,shtType);
    CPack::Output(pBuf+_const_pack_head_size,intErrCode);
    //
    int intCount = _const_pack_head_size + sizeof(int);
    if( CComApi::Send(nSocket,pBuf,intCount,MSG_NOSIGNAL,NULL) == false )
    {
        Log(CLog::Error,"ProcessRequest","CComApi::Send(nSocket,pBuf,intCount,MSG_NOSIGNAL,NULL) == false{nSocket=%d,pBuf=%08p,intCount=%d}",nSocket,pBuf,intCount);
        return false;
    }
    intCount = strMessage.size()+1;
    if( CComApi::Send(nSocket,strMessage.c_str(),intCount,MSG_NOSIGNAL,NULL) == false )
    {
        Log(CLog::Error,"ProcessRequest","CComApi::Send(nSocket,strMessage.c_str(),intCount,MSG_NOSIGNAL,NULL) == false{nSocket=%d,strMessage.c_str()=%08p,intCount=%d}",nSocket,strMessage.c_str(),intCount);
        return false;
    }*/
    char pTmp[4096];
    int intRetPacketSize = 4096;
    CPack::Pack2(pTmp, intRetPacketSize, intPackNo, shtVersion, shtType, intErrCode, strMessage);
    if( CComApi::Send(nSocket,pTmp,intRetPacketSize,MSG_NOSIGNAL,NULL) == false )
    {
        Log(CLog::Error,"ProcessRequest","CComApi::Send(nSocket,pTmp,intCount,MSG_NOSIGNAL,NULL) == false{nSocket=%d,strMessage.c_str()=%08p,intCount=%d}",nSocket,strMessage.c_str(),intRetPacketSize);
        return false;
    }
     

    return true;
}

bool CCtrlServer::RecvWithTimeOut(const int nSocket,char * pBuf,int & nCount,const int nTimeOut/* ms */ )const
{
    struct  pollfd pFds[1];
    const short shtReadEvents = POLLIN;
    //
    pFds[0].fd      = nSocket;
    pFds[0].events  = shtReadEvents;
    pFds[0].revents = 0;
    //
    int  nFdsCount = 0;
    // poll
    if( CComApi::Poll(nFdsCount,pFds,1,nTimeOut) == false )
    {
        Log(CLog::Error,"RecvWithTimeOut","Poll(nFdsCount,pFds,1,nTimeOut) == false{nFdsCount=%d,pFds=%08p,nTimeOut=%d}",nFdsCount,pFds,nTimeOut);
        return false;
    }
    if( nFdsCount == 0 )
    {// timeout
        Log(CLog::Error,"RecvWithTimeOut","nFdsCount == 0,Poll timeout{Timeout=(nTimeOut=%d)}",nTimeOut);
        return false;
    }
    // ReadSet
    if( CComApi::Recv(nSocket,pBuf,nCount,0) == false )
    {
        Log(CLog::Error,"RecvWithTimeOut","Recv(nSocket,pBuf,nCount,0) == false{nSocket=%d,pBuf=%08p,nCount=%d}",nSocket,pBuf,nCount);
        return false;
    }
    if( nCount == 0 )
    {
        Log(CLog::Error,"RecvWithTimeOut","Recv Count == 0,Socket is closed {nSocket=%d,nCount=%d}",nSocket,nCount);
        return false;
    }
    return true;
}

}// namespace BackCom
