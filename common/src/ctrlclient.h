#ifndef CCTRLCLIENT_H
#define CCTRLCLIENT_H

#include <vector>
#include <string>
using namespace std;

namespace BackCom
{
class CCtrlClient
{
public:
    static bool SendCtrlCommand(int & intRetErrCode
                        ,string & strMessage
                        ,const vector<string> & vectParams
                        ,const char *pServer
                        ,const short shtPort
                        );

    static bool IsRun(const char *pServer
                     ,const short shtPort
                     );

    //
private:
    static bool Connect(int & nSocket,const char *pServer,const short shtPort);
    static bool RecvWithTimeOut(const int nSocket,char * pBuf,int & nCount,const int nTimeOut/* ms */);
};

}// namespace BackCom

#endif  
