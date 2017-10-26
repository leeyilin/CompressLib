#include "ctrlclient.h"

#include <string.h>     // memset , used by FD_ZERO
#include <arpa/inet.h>  // inet_ntoa

#include "comapi.h"
#include "pack.h"

namespace BackCom
{
bool CCtrlClient::RecvWithTimeOut(const int nSocket,char * pBuf,int & nCount,const int nTimeOut/* ms */ )
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
        //Log(CLog::Error,"RecvWithTimeOut","Poll(nFdsCount,pFds,1,nTimeOut) == false{nFdsCount=%d,pFds=%08p,nTimeOut=%d}",nFdsCount,pFds,nTimeOut);
        return false;
    }
    if( nFdsCount == 0 )
    {// timeout
        //Log(CLog::Error,"RecvWithTimeOut","nFdsCount == 0,Poll timeout{Timeout=(nTimeOut=%d)}",nTimeOut);
        return false;
    }
    // ReadSet
    if( CComApi::Recv(nSocket,pBuf,nCount,0) == false )
    {
        //Log(CLog::Error,"RecvWithTimeOut","Recv(nSocket,pBuf,nCount,0) == false{nSocket=%d,pBuf=%08p,nCount=%d}",nSocket,pBuf,nCount);
        return false;
    }
    if( nCount == 0 )
    {
        //Log(CLog::Error,"RecvWithTimeOut","Recv Count == 0,Socket is closed {nSocket=%d,nCount=%d}",nSocket,nCount);
        return false;
    }
    return true;
}

bool CCtrlClient::IsRun(const char *pServer
                     ,const short shtPort
                     )
{
    // connect
    int nSocket = -1;
    if( Connect(nSocket,pServer,shtPort) == false )
    {
        return false;
    }
    // close
    if( nSocket >= 0 )
    {
        if( CComApi::Close(nSocket) == false )
        {//
            printf("[CCtrlClient::IsRun][Warn]CComApi::Close(nSocket) == false{nSocket=%d}\n",nSocket);
        }
        nSocket = -1;
    }
    return true;
}

bool CCtrlClient::Connect(int & nSocket,const char *pServer,const short shtPort)
{
    // make addr
    struct sockaddr_in  saddr;             //
    if( CComApi::INetAddr(saddr,pServer,shtPort) == false )
    {
        printf("[CClientProc::Connect][Err ]CComApi::INetAddr(saddr,pServer,shtPort) == false{pServer='%s',shtPort=%d}\n",pServer,shtPort);
        return false;
    }
    // new socket
    if(CComApi::Socket(nSocket,PF_INET, SOCK_STREAM, 0) == false)
    {
        printf("[CClientProc::Connect][Err ]CComApi::Socket(nSocket,PF_INET, SOCK_STREAM, 0) == false{nSocket=%d}\n",nSocket);
        return false;
    }
    // connect
    if( CComApi::Connect(saddr,nSocket) == false )
    {// connect fail
        //printf("[CClientProc::Connect][Err ]CComApi::Connect(saddr,nSocket) == false{&saddr=%8p,nSocket=%d}",&saddr,nSocket);
        return false;
    }
    return true;
}

bool CCtrlClient::SendCtrlCommand(int & intErrCode
                        ,string & strMessage
                        ,const vector<string> & vectParams
                        ,const char *pServer
                        ,const short shtPort
                        )
{
    int nSocket = -1;
    if( Connect(nSocket,pServer,shtPort) == false )
    {
        printf("Connect(nSocket,pServer,_const_port) == false{nSocket=%d,pServer='%s',shtPort=%d}\n",nSocket,pServer,shtPort);
        return false;
    }
    //
    const int _const_send_packet_buf_size = 32768;
    char pchBuf[_const_send_packet_buf_size];
    int intPackSize = 0;
    {
        const int _const_pack_head_size = 12;
        int nTmp = _const_pack_head_size + 0;
        //
        int intCount = vectParams.size();
        nTmp += CPack::Output(pchBuf+ nTmp,intCount);
        for( int  i = 0 ; i < intCount ; ++i )
        {
            nTmp += CPack::Output(pchBuf+ nTmp,vectParams[i].c_str());
        }
        if( _const_send_packet_buf_size < nTmp )
        {
            return false;
        }
        //
        int intPackNo = 1;
        unsigned short shtVersion = 100;
        unsigned short shtType = 1;
        CPack::PackHead(pchBuf,intPackSize,nTmp,intPackNo,shtVersion,shtType);
    }
    //
    if( CComApi::Send(nSocket,pchBuf,intPackSize,MSG_NOSIGNAL,NULL) == false )
    {
        //Log(CLog::Error,"ProcessRequest","CComApi::Send(nSocket,pchBuf,intPackSize,MSG_NOSIGNAL,NULL) == false{nSocket=%d,pchBuf=%p,intPackSize=%d}",nSocket,pchBuf,intPackSize);
        return false;
    }
    // 3.recv ret pack
    const int _const_ret_packet_size = 10*1024*1024;
    vector<char> vectTmp(_const_ret_packet_size);
    char * pBuf = &*vectTmp.begin();
    int nContentLen = 0;
    {
        const int _PackHeadSize = 12;   // pasksize(int)+packno(int)+ver(short)+packtype(short)
        int intPackSize = 0;
        while(true)
        {
            int nTimeOut = 5 * 1000 ;
            int nCount = (intPackSize==0)?(_PackHeadSize-nContentLen):(intPackSize-nContentLen);
            if( RecvWithTimeOut(nSocket,pBuf+nContentLen,nCount,nTimeOut) == false )
            {
                printf("RecvWithTimeOut(nSocket,pBuf,nCount,nTimeOut) == false{nSocket=%d,pBuf=%8p,nCount=%d,nTimeOut=%d}\n",nSocket,pBuf,nCount,nTimeOut);
                break;
            }
            nContentLen += nCount;
            if( nContentLen >= _PackHeadSize )
            {
                if( intPackSize == 0 )
                    CPack::Input(pBuf,intPackSize);
                if( nContentLen >= intPackSize )
                break;
            }
        }
    }
    //
    {
        int intPackNo;
        unsigned short shtVersion=0,shtType=0;
        CPack::Unpack2(intPackNo,shtVersion,shtType,intErrCode,strMessage,pBuf,nContentLen);
    }
    //
    if( CComApi::Close(nSocket) == false )
    {
        return false;
    }
    return true;
}

}// namespace BackCom
