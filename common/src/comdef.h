#ifndef COMDEF_H
#define COMDEF_H
#include <string.h>
#include "stdqt.h"
#include "DateTime.h"
using namespace BackCom;
#include "QtTypeDef.h"
enum MARKET_TYPE{MARKTYPE_DEFAULT = 0, MARKTYPE_FOREX};
#pragma pack(push, 1)

struct SRegInfo
{
    int             m_intConID;
    int             m_intPackNo;
    unsigned short  m_shtVer;   
    unsigned short  m_shtType;
    unsigned short  m_shtMarket;
    SRegInfo()
    {
        memset(this, 0, sizeof(SRegInfo));
    }
};

struct SMingXi
{
    //成交量的单位为股
    UINT32       m_dwDate;//交易日YYYYMMDD
    UINT32       m_dwTime;//交易时间hhmmss
    double       m_dblPrice;    //价格
    UINT64       m_xVolume; //成交量
    UINT32      m_dwTradeNum;   //成交笔数
    INT64        m_xOI;     //增仓
    UINT32       m_cCurVol; //交易类型
    SMingXi()
    {
        memset(this, 0, sizeof(SMingXi));
    }
};

#define DETAIL_SIDE_FLAG_SELL           1                   // 卖出成交单
#define DETAIL_SIDE_FLAG_BUY            2                   // 买入成交单
#define DETAIL_SIDE_FLAG_MIDDLE         3                   // 混合

struct SMultiQt:public SStdQt
{
    char   m_pchUniqueID[STOCK_ID_LEN]; //唯一标识
    unsigned short m_wMarket;
    unsigned short m_wType;
    UINT64 m_xWaiPan;       // 内盘
    UINT64 m_xCurVol;       // 现手
    double m_dblCurAmt;     // 现额
    char   m_cCurVol;   // 现手方向
    INT64  m_xCurOI;            // 持仓量变化
    char   m_cTradeFlag;    //交易标志, 0交易中, 1收盘
    SMultiQt()
    {
        memset(this, 0, sizeof(SMultiQt));
    }
};

struct SMultiQtNew:public SMultiQt
{
    int m_nScale;
    SMultiQtNew()
    {
        memset(this, 0, sizeof(SMultiQtNew));
    }
};

struct BlockQt
{
    UINT32      m_dwDate;   //交易日YYYYMMDD
    UINT32      m_dwTime;   //交易时间hhmmss
    
    char        m_pcName[32];   //股票名称
    char        m_pchCode[16];  //股票代码

    double      m_dblOpen;//开盘价
    double      m_dblHigh;//最高价
    double      m_dblLow;//最低价
    double      m_dblPrice;//最新价 （（（个股最新－个股昨收）／个股昨收＊股本）之和／股本之和 ＋1）＊昨收
    double      m_dblClose;//前日收盘价(算昨涨跌幅)
    UINT64      m_xVolume;    //成交量
    double      m_dblAmount;//成交额

    char        m_pchTopStockCode[32];//领涨
    UINT32      m_dwStockNum;
    UINT32      m_dwUpNum;//涨家数
    UINT32      m_dwDownNum;
    UINT32      m_dwStrongNum;//走强    >5％
    UINT32      m_dwWeakNum;
    double      m_dblZGB;//总股本
    double      m_dblZSZ;//总市值
    float       m_fAvgProfit;//平均赢利 
    float       m_fPeRatio;//市盈率 最新价总和/每股收益总和
    BlockQt()
    {
        memset(this, 0, sizeof(BlockQt));
    }
};

struct SBlockQt:public BlockQt
{
    char        m_pchUniqueID[STOCK_ID_LEN]; //唯一标识
    unsigned short m_wMarket;
    unsigned short m_wType;
    UINT64 m_xCurVol;       // 现手
    double m_dblCurAmt;     // 现额
    
    SBlockQt()
    {
        memset(this, 0, sizeof(SBlockQt));
    }
};

struct SBlockQtEx:public SBlockQt
{
    int m_nScale;
    SBlockQtEx()
    {
        memset(this, 0, sizeof(SBlockQtEx));
    }
};

struct SMultiMmp:public SStdMmp
{
    char   m_pchUniqueID[STOCK_ID_LEN]; //唯一标识
    unsigned short m_wMarket;
    unsigned short m_wType;
    int intScale;
    SMultiMmp()
    {
        memset(this, 0, sizeof(SMultiMmp));
    }
};

struct SMultiQtEx:public SStdQtEx
{
    char   m_pchUniqueID[STOCK_ID_LEN]; //唯一标识
    unsigned short m_wMarket;
    unsigned short m_wType;
    int intScale;
    SMultiQtEx()
    {
        memset(this, 0, sizeof(SMultiQtEx));
    }
};

struct SMultiCAS:public SStdCAS
{
    char   m_pchUniqueID[STOCK_ID_LEN]; //唯一标识
    unsigned short m_wMarket;
    unsigned short m_wType;
    int intScale;
    SMultiCAS()
    {
        memset(this, 0, sizeof(SMultiCAS));
    }
};

struct SMultiVCM:public SStdVCM
{
    char   m_pchUniqueID[STOCK_ID_LEN]; //唯一标识
    unsigned short m_wMarket;
    unsigned short m_wType;
    int intScale;
    SMultiVCM()
    {
        memset(this, 0, sizeof(SMultiVCM));
    }
};

struct SMultiBq : public SStdBrokerQueue
{
    char m_pchUniqueID[STOCK_ID_LEN]; //唯一标识
    unsigned short m_wMarket;
    unsigned short m_wType;
    SMultiBq()
    {
        memset(this, 0, sizeof(SMultiBq));
    }
};

struct SMultiMingXi:public SStdMingXi
{
    char   m_pchUniqueID[STOCK_ID_LEN]; //唯一标识
    unsigned short m_wMarket;
    unsigned short m_wType;
    char   m_cCurVol;   // 现手方向
    double m_dblPrice;
    
    SMultiMingXi()
    {
        memset(this, 0, sizeof(SMultiMingXi));
    }
};


struct SMultiNewMingXi:public SStdNewMingXi
{
    char   m_pchUniqueID[STOCK_ID_LEN]; //唯一标识
    unsigned short m_wMarket;
    unsigned short m_wType;
    char   m_cCurVol;   // 现手方向
    double m_dblPrice;
    
    SMultiNewMingXi()
    {
        memset(this, 0, sizeof(SMultiNewMingXi));
    }
};


struct SNewRtMin
{
    unsigned int m_dwTime;          //时间

    double m_dblOpen;               //开盘价
    double m_dblHigh;               //最高价
    double m_dblLow;                //最低价
    double m_dblClose;              //收盘价
    double m_dblAve;                //均价

    UINT64 m_xVolume;               // 成交量
    double m_dblAmount;         // 成交额
    UINT64 m_xWaiPan;           // 内盘

    UINT64 m_xOpenInterest;     // 期货持仓量
    double m_dblSettlementPrice;    // 期货结算价
    INT64 m_xCurOpenInterest;   // 期货增仓, 外汇为跳动量
    SNewRtMin()
    {
        memset(this, 0, sizeof(SNewRtMin));
    }
};

typedef SNewRtMin SMultiDayKLine;

struct SStockDict
{
    char   m_pchUniqueID[STOCK_ID_LEN]; //唯一标识
    unsigned short m_wMarket;
    unsigned short m_wType;
    
    char        m_pcName[STOCK_NAME_LEN]; //股票名称
    char        m_pchCode[STOCK_CODE_LEN];//股票代码
    SStockDict()
    {
        memset(this, 0, sizeof(SStockDict));
    }

    bool operator == (const SStockDict & data)const
    {
        return memcmp(this, &data, sizeof(data)) == 0;
    }
};

struct SStockDictEx:public SStockDict
{
    char m_pcFullName[64];
    unsigned short m_wScale;
    char m_pcReserve[10];
    
    SStockDictEx()
    {
        memset(this, 0, sizeof(SStockDictEx));
    }
    
    bool operator == (const SStockDictEx & data)const
    {
        return memcmp(this, &data, sizeof(data)) == 0;
    }
};

#pragma pack(pop)

struct STradePeriod
{
    SDateTime m_beg;
    SDateTime m_end;
    STradePeriod()
    {
        
    }
};

#endif

