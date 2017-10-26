#ifndef MARKET_H
#define MARKET_H
#include "config.h"
#include "transctrlcon.h"
using namespace NetIOOld;
using namespace BackCom;
namespace MultiTrans
{
class CMarket
{
public:
    virtual bool Update(const char * pData, const int intLen) = 0;
    virtual bool ProcReq(const int intConID, const char * pData, const int intSize) = 0;
};
};
#endif
