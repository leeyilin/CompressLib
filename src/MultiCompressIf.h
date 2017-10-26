#pragma once
#include <string.h>
#include <vector>
#include <exception>
#include <string>
using namespace std;
#include "QtTypeDef.h"
#include "arraydata.h"
#pragma pack(1)

/* There will be only one macro of version here,
 * which must be the latest version of mcm and should be updated
 * upon every release.
 */
#define MCM_VERSION 161

class CRawMultiQt
{
public:
    UINT32        m_dwTradeSequence;
    UINT32        m_dwDate;               //交易日YYYYMMDD
    UINT32        m_dwTime;               //交易时间hhmmss
    char          m_pcName[32];           //股票名称
    char          m_pchCode[16];          //股票代码
    unsigned char m_bytMarket;
    unsigned char m_bytType;
    unsigned char m_bytTradeFlag;         // 交易标志，0交易中，1已收盘

    UINT32        m_dwClose;              // 前日收盘价
    UINT32        m_dwOpen;               //开盘价
    UINT32        m_dwHigh;               //最高价
    UINT32        m_dwLow;                //最低价
    UINT32        m_dwPrice;              //最新价

    UINT64        m_xVolume;              //成交量
    UINT64        m_xAmount;              //成交额
    UINT64        m_xTradeNum;            //成交笔数

    UINT32        m_pdwMMP[10];           //买卖盘价格
    UINT64        m_pxMMPVol[10];         //买卖盘量

    UINT64        m_xWaiPan;              // 外盘
    UINT64        m_xCurVol;              // 现手
    BYTE          m_cCurVol;              // 现手方向
    INT64         m_xCurOI;               // 持仓量变化
    UINT32        m_dwAvg;                /// 均价

    UINT64        m_xOpenInterest;        //持仓量
    UINT32        m_dwSettlementPrice;    //今结算价
    UINT64        m_xPreOpenInterest;     ///昨持仓量
    UINT32        m_dwPreSettlementPrice; //昨结算价

    UINT32        m_dwPriceZT;            // 涨停价,美股为52周最高价
    UINT32        m_dwPriceDT;            // 跌停价,美股为52周最低价
    CRawMultiQt()
    {
        memset(this, 0, sizeof(CRawMultiQt));
    }
};

class CRawMultiQtS
{
public:
    UINT32        m_dwTradeSequence;
    UINT32        m_dwDate;               //交易日YYYYMMDD
    UINT32        m_dwTime;               //交易时间hhmmss
    char          m_pcName[32];           //股票名称
    char          m_pchCode[16];          //股票代码
    unsigned char m_bytMarket;
    unsigned char m_bytType;
    unsigned char m_bytTradeFlag;         // 交易标志，0交易中，1已收盘

    UINT32        m_dwClose;              // 前日收盘价
    UINT32        m_dwOpen;               //开盘价
    UINT32        m_dwHigh;               //最高价
    UINT32        m_dwLow;                //最低价
    UINT32        m_dwPrice;              //最新价

    UINT64        m_xVolume;              //成交量
    UINT64        m_xAmount;              //成交额
    UINT64        m_xTradeNum;            //成交笔数

    UINT32        m_pdwMMP[20];           //买卖盘价格
    UINT64        m_pxMMPVol[20];         //买卖盘量

    UINT64        m_xWaiPan;              // 外盘
    UINT64        m_xCurVol;              // 现手
    BYTE          m_cCurVol;              // 现手方向
    INT64         m_xCurOI;               // 持仓量变化
    UINT32        m_dwAvg;                /// 均价

    UINT64        m_xOpenInterest;        //持仓量
    UINT32        m_dwSettlementPrice;    //今结算价
    UINT64        m_xPreOpenInterest;     ///昨持仓量
    UINT32        m_dwPreSettlementPrice; //昨结算价

    UINT32        m_dwPriceZT;            // 涨停价,美股为52周最高价
    UINT32        m_dwPriceDT;            // 跌停价,美股为52周最低价

    CRawMultiQtS()
    {
        memset(this, 0, sizeof(CRawMultiQtS));
    }
};

struct CRawMultiQtEx
{
    CRawMultiQtEx()
    {
        m_bTradingStatus = 0;
        m_cCurrencyCode[0] = '\0';
        m_dwLotSize = 0;
        m_cSpreadTable[0] = '\0';
        m_cCASFlag = 0;
        m_cVCMFlag = 0;
        m_cShortSellFlag = 0;
        m_cCCASSFlag = 0;
        m_cDummySecurityFlag = 0;
        m_cStampDutyFlag = 0;
        m_dwListingDate = 0;
        m_dwDelistingDate = 0;
        m_cFiller[0] = '\0';
        m_cEFNFalg = 0;
        m_dwCouponRate = 0;
        m_dwAccruedInt = 0;
        m_dwYield = 0;
        m_cCallPutFlag = 0;
        m_dwConversionRatio = 0;
        m_dwStrikePrice = 0;
        m_dwMaturityDate = 0;
        m_pchUnderlyingCode[0] = '\0';
        m_cStyle = 0;
        m_dwIEPrice = 0;
        m_xIEVolume = 0;
    }

    // 0：正常交易  2：暂停交易  3：恢复交易
    BYTE m_bTradingStatus;

    // 'HKD','USD','EUR','JPY','GBP','CAD','SGD','CNY'
    char m_cCurrencyCode[3];

    // 该股每手的股数(每只代码不一样)
    UINT32 m_dwLotSize;

    //代码价差表 - '01':Part A   '03':Part B
    char   m_cSpreadTable[2];

    // 是否适用于CAS机制 - 'Y'：是 'N'：否
    char   m_cCASFlag;

    // 是否适用于VCM机制 - 'Y'：是 'N'：否
    char   m_cVCMFlag;

    // short-sell authorization  取值：'Y'：是   'N'：否
    char   m_cShortSellFlag;

    // CCASS security        取值：'Y'：是   'N'：否
    char   m_cCCASSFlag;

    // Dummy Security            取值：'Y'：是   'N'：否
    char   m_cDummySecurityFlag;

    // stamp duty requirement    取值：'Y'：是   'N'：否
    char   m_cStampDutyFlag;

    UINT32 m_dwListingDate;   // 上市日期
    UINT32 m_dwDelistingDate; // 退市日期/摘牌日期
    char   m_cFiller[4];      // 预留字段

    //债券BOND
    //代码是否是EFN   取值：'Y'：是   'N'：否
    char        m_cEFNFalg;
    UINT32         m_dwCouponRate;            //票面利率

    //应计利息  注：应计利息＝票面利率÷365天×已计息天数。
    UINT32      m_dwAccruedInt;

    //当前收益率
    INT32       m_dwYield;

    //窝轮和结构性产品(牛熊证)WRNT/篮子窝轮BWRT
    //
    // (1) 窝轮/篮子窝轮
    //     取值：'C':看涨 'P':看跌 'O'：其他
    // (2) 牛熊证
    //     取值： 'C':牛证 'P':熊证  'O'：其他
    char        m_cCallPutFlag;
    UINT32      m_dwConversionRatio;                 //换股比率
    UINT32      m_dwStrikePrice;                     //行使价

    //到期日 YYYYMMDD
    UINT32      m_dwMaturityDate;

    // 对应的标的市场及代码:
    //   窝轮和牛熊证只有1个标的;
    //   篮子窝轮可能有多个标的，暂只提供1个
    unsigned char m_bytUnderlyingMarket;

    char         m_pchUnderlyingCode[16];

    //只用于篮子窝轮  取值：'A'：美式  'E':欧式  ''：其他
    char        m_cStyle;

    INT32        m_dwIEPrice;             //参考平衡价
    INT64        m_xIEVolume;             //参考平衡量
};

//CAS(收市竞价时段)
struct CRawMultiCAS
{
    CRawMultiCAS()
    {
        m_cOrderImbalanceDirection = ' ';
        m_xOrderImbalanceQuantity = 0;
        m_dwReferencePrice = 0;
        m_dwLowerPrice = 0;
        m_xUpperPrice = 0;
    }

    // 订单不平衡方向
    // 取值：'N': Buy = Sell
    //       'B': Buy Surplus
    //       'S': Sell Surplus
    //       ' ': when IEP is not available
    char    m_cOrderImbalanceDirection;

    // 订单不平衡量
    // 取值：Value should be ignored, if Order Imbalance Direction is space
    INT64    m_xOrderImbalanceQuantity;

    //收市竞价参考价
    INT32   m_dwReferencePrice;

    //收市竞价价格下限
    INT32   m_dwLowerPrice;

    //收市竞价价格上限
    INT32    m_xUpperPrice;
};

//VCM(市场波动调节机制)
struct CRawMultiVCM
{
    CRawMultiVCM()
    {
        m_dwVCMDate = 0;
        m_dwVCMStartTime = 0;
        m_dwVCMEndTime = 0;
        m_dwVCMReferencePrice = 0;
        m_xVCMLowerPrice = 0;
        m_xVCMUpperPrice = 0;
    }

    UINT32 m_dwVCMDate;           // 熔断日期
    UINT32 m_dwVCMStartTime;      // 熔断开始时间
    UINT32 m_dwVCMEndTime;        // 熔断结束时间
    INT32  m_dwVCMReferencePrice; // 熔断参考价
    INT32  m_xVCMLowerPrice;      // 熔断价格下限
    INT32  m_xVCMUpperPrice;      // 熔断价格上限
};

class CRawMultiBlockQt
{
public:
    UINT32                 m_dwTradeSequence;
    UINT32                 m_dwDate;                 //交易日YYYYMMDD
    UINT32        m_dwTime;                //交易时间hhmmss
    char         m_pcName[32];
    char         m_pchCode[16];
    unsigned char       m_wMarket;
    unsigned char       m_wType;

    UINT32                 m_dwClose;
    UINT32        m_dwOpen;
    UINT32        m_dwHigh;
    UINT32        m_dwLow;
    UINT32        m_dwPrice;

    UINT64        m_xVolume;            //成交量
    UINT64        m_xAmount;                      //成交额

    char        m_pchTopStockCode[32];   //领涨股
    unsigned char      m_pchTopStockMarketID; //领涨股所在市场
    UINT32        m_dwStockNum;
    UINT32         m_dwUpNum;                 //涨家数
    UINT32         m_dwDownNum;              //跌家数
    UINT64                 m_xZGB;                        //总股本
    UINT64                 m_xZSZ;                        //总市值
    UINT64                 m_xAvgProfit;             //平均盈利
    INT32                   m_xPeRatio;                 //市盈率

    UINT64                 m_xTurnover2Day;         //前2天总成交量   （用于计算3日换手）
    UINT32                 m_xPercent3Day;           //前3日收盘价       （用于计算3日涨幅）
    UINT32                    m_dwPre5MinPrice[5];   // 5分钟价格          （用于计算涨速）

    CRawMultiBlockQt()
    {
            memset(this, 0, sizeof(CRawMultiBlockQt));
    };
};

struct SRawBlockOverView {        //板块纵览接口
    char                   m_pchCode[16];
    unsigned short         market;
    unsigned short         type;
    INT64                  m_xNetFlow;
    INT32                  m_dwPercent; // 板块涨幅, 4位小数

    // 截止到昨天的连涨天数。包括当天的连涨天数N需要根据m_dwPercent来进行计算。
    // if m_dwPercent <= 0, N = 0
    // else,                N = m_dwUpDay + 1
    UINT32                 m_dwUpDay;
    char                   m_pchTopStockCode[32];
    unsigned char          m_pchTopStockMarketID;
    INT32                  m_dwTopPercent; // 领涨股涨幅, 4位小数
    SRawBlockOverView() {
        memset(this, 0, sizeof(SRawBlockOverView));
    }
};

struct SRawMultiBaseBQ         //经纪队列接口
{
    struct SBrokerNoItem
    {
        BYTE cTabIndex;             //档位
        vector<unsigned short> vecBrokerNoItem;
        SBrokerNoItem():cTabIndex(0)
        {
        }
    };
    vector<SBrokerNoItem> m_vecBuyBrokerNo;
    vector<SBrokerNoItem> m_vecSellBrokerNo;

    SRawMultiBaseBQ()
    {
    }
};

struct SRawMultiBQTrace         //经纪追踪接口
{
    struct STrace
    {
        char strStockID[16];
        unsigned char ucFlag;        //取值（与开盘价比较）    equal: 0   less: 1   greater: 2
        UINT32 dwPrice;
        STrace()
        {
            memset(this, 0, sizeof(STrace));
        }
    };
    struct SBQTraceItem
    {
        BYTE  bTabIndex;             //档位
        vector<STrace> vecBQTraceItem;
        SBQTraceItem():bTabIndex(0)
        {
        }
    };
    vector<SBQTraceItem> m_vecBuyBQTrace;
    vector<SBQTraceItem> m_vecSellBQTrace;

    SRawMultiBQTrace()
    {
    }
};

struct SRawMultiRtMin
{
    UINT32  m_wTime;                //时间(YYMMDDHHMI)

    UINT32 m_dwOpen;                //开盘价
    UINT32 m_dwHigh;                //最高价
    UINT32 m_dwLow;                //最低价
    UINT32 m_dwClose;                //收盘价
    UINT32 m_dwAve;                //均价

    UINT64 m_xVolume;                // 成交量
    UINT64 m_xAmount;                // 成交额
    UINT64 m_xTradeNum;            // 成交笔数
    UINT64 m_xWaiPan;            // 外盘

    INT64 m_xExt1;                //期货为持仓量，其他无
    INT64 m_xExt2;                //期货为增仓，其他无

    UINT32  m_dwExt1;                //期货为结算价
    UINT32  m_dwExt2;                //无
    SRawMultiRtMin()
    {
        memset(this, 0, sizeof(SRawMultiRtMin));
    }
};
/*
struct SRawMultiKline
{
    UINT32 m_time;                                            // 时间(YYYYMMDD)
    UINT32 m_Open;                                            // 开盘价
    UINT32 m_Close;                                            // 收盘价
    UINT32 m_High;                                            // 最高价
    UINT32 m_Low;                                            // 最低价
    UINT64 m_Volume;                                        // 成交数量
    UINT64 m_Value;                                        // 成交金额
    SRawMultiKline()
    {
        memset(this, 0, sizeof(SRawMultiKline));
    }
};
*/
typedef SRawMultiRtMin SRawMultiKline;
class SRawMultiMx
{
public:
    UINT32 m_dwDate;        //交易日YYYYMMDD
    UINT32 m_dwTime;        //交易时间hhmmss
    UINT32 m_dwPrice;        //价格
    UINT64 m_xVolume;        //成交量
    UINT32 m_dwTradeNum;    //成交笔数
    INT32  m_xOI;            //增仓量
    UINT32 m_cBS;            //交易类型
    SRawMultiMx()
    {
        memset(this, 0, sizeof(SRawMultiMx));
    }
};

// HUGE is a macro in <math.h>...
struct SMultiFFOrderType
{
    enum {
        SMALL_ORDER = 0,
        MIDDLE_ORDER,
        BIG_ORDER,
        HUGE_ORDER,
        MAX_ORDER,
    };
};

struct SMultiFFDayType
{
    enum {
        ONE_DAY = 0,
        THREE_DAY,
        FIVE_DAY,
        TEN_DAY,
        MAX_DAY,
    };
};

struct SRawMultiFFItem
{
    UINT64 m_buyAmount;   // 流入
    UINT64 m_sellAmount;  // 流出
    UINT64 m_buyVolume;
    UINT64 m_sellVolume;

    SRawMultiFFItem()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SRawMultiFFOrder
{
    UINT64 m_xAmtOfBuy;	  // 买盘成交额
    UINT64 m_xAmtOfSell;  // 卖盘成交额
    UINT64 m_xVolOfBuy;	  // 买盘成交量
    UINT64 m_xVolOfSell;  // 卖盘成交量
    UINT32 m_dwNumOfBuy;  // 买盘成交笔数
    UINT32 m_dwNumOfSell; // 卖盘成交笔数

    SRawMultiFFOrder()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SRawMultiMinuteFF
{
    SRawMultiFFOrder m_orders[SMultiFFOrderType::MAX_ORDER];
	UINT32 m_dwTime; // Format: YYMMDDHHMM

	SRawMultiMinuteFF()
    {
        memset(this, 0, sizeof(*this));
    }
};

class RawInferiorFundFlow {
public:
    unsigned int tradeTime_;
    UINT64 amountOfBuys_[SMultiFFOrderType::MAX_ORDER];             // 买盘成交额
    UINT64 amountOfSells_[SMultiFFOrderType::MAX_ORDER];            // 卖盘成交额
    UINT64 volumeOfBuys_[SMultiFFOrderType::MAX_ORDER];             // 买盘成交量
    UINT64 volumeOfSells_[SMultiFFOrderType::MAX_ORDER];            // 卖盘成交量
    UINT32 numberOfBuys_[SMultiFFOrderType::MAX_ORDER];             // 买盘成交笔数
    UINT32 numberOfSells_[SMultiFFOrderType::MAX_ORDER];            // 卖盘成交笔数
    RawInferiorFundFlow() {
        memset(this, 0, sizeof(*this));
    }
};

typedef SRawMultiMinuteFF SRawMultiFFTrend;

struct SRawMultiDayFF
{
    struct BuyinItem
    {
        float ratio; // 增仓占比
        INT32 rank;  // 增仓排名
    };

    SRawMultiFFOrder m_orders[SMultiFFOrderType::MAX_ORDER];
    BuyinItem   m_ranks[SMultiFFDayType::MAX_DAY];

	SRawMultiDayFF()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 资金流向(列表)
struct SRawListMultiFFItem
{
    unsigned char marketID;
    char   stockCode[16];
    SRawMultiFFItem m_orders[SMultiFFOrderType::MAX_ORDER];
    UINT64 m_callAuctionAmount;        // 集合竞价成交额
    UINT32 m_latestPrice;              // 最新价
    UINT32 m_prevClose;                // 昨收

	SRawListMultiFFItem()
    {
        memset(this, 0, sizeof(*this));
    }
};

// 增仓排名(列表)
struct SRawListMultiBuyinRankItem
{
    struct Item
    {
        UINT32 m_prevClose;   // 前N日收盘价
        float  m_buyinRatio;  // 增仓占比
        INT32  m_rank;        // 增仓排名
        INT32  m_prevRank;    // 前一日排名
    };

    unsigned char marketID;
    char   stockCode[16];
    Item   m_days[SMultiFFDayType::MAX_DAY];
    UINT32 m_latestPrice;                     // 最新价

	SRawListMultiBuyinRankItem()
    {
        memset(this, 0, sizeof(*this));
    }
};

// Market Name: HKQUOTA
// Stock Code: 
//      CSCSHQ: Shanghai Stock Exchange   
//      CSCSZQ: Shenzhen Stock Exchange
struct SRawMarketTurnover {
    struct Item {
        UINT32  date_in_yymmdd;
        UINT32  time_in_hhmmss;
        INT64 buyin_amount;
        INT64 sold_amount;
        INT64 total_amount;
    };

    SRawMarketTurnover() {
        memset(this, 0, sizeof(*this));
    }

    Item north_turnover; // SH/SZ turnover
    Item south_turnover; // HK turnover;
};

#pragma pack()

struct SRawStockMultiMin
{
    char m_strStockID[16];    //股票代码
    unsigned char m_bytMarket;
    CArrayData<SRawMultiRtMin> m_data;
    SRawStockMultiMin()
    {

    }
};


class RawMultiStockMinuteFundFlow {
public:
    unsigned char marketID_;
    char stockID_[16];
    CArrayData<RawInferiorFundFlow> rawInferiorFundFlows_;
    RawMultiStockMinuteFundFlow(): marketID_(0) {
        memset(stockID_, 0, sizeof(stockID_));
    }
};

struct SRawStockMultiDayKline
{
    char m_strStockID[16];    //股票代码
    unsigned char m_bytMarket;
    CArrayData<SRawMultiKline> m_data;
    SRawStockMultiDayKline()
    {

    }
};

class CMcmException:public exception
{
public:
    enum LCME_TYPE{LCME_DEFAULT = 0, LCME_UZIP, LCME_INCREMENT, LCME_SNAP};
    LCME_TYPE m_wType;
    string m_strMsg;
    CMcmException()
        :m_wType(LCME_DEFAULT)
    {
    }

    virtual ~CMcmException()throw()
    {
    }
};

class CLASSINDLL_CLASS_DECL CMultiCompressIf
{
public:
    static CMultiCompressIf * CreateInstance();
    static void FreeInstance(CMultiCompressIf *);
    virtual ~CMultiCompressIf() {}
    virtual bool Uncompress(const char * pBuf, const int intLen, CRawMultiQt & data) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, CRawMultiQtS & data) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, CRawMultiBlockQt & data) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMultiRtMin *& pNew, int & intSize) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMultiMx *& pNew, int & intSize) = 0;
    virtual bool Uncompress2(const char * pBuf, const int intLen, SRawMultiMx *& pNew, int & intSize) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, CRawMultiQt *& pNew, int & intSize, bool & blnIsSnap) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, CRawMultiBlockQt *& pNew, int & intSize, bool & blnIsSnap) = 0;
    virtual bool Uncompress(const char* pBuf, const int intLen, SRawBlockOverView*& ptrData, \
        int& dataSize, bool& isSnapped, const unsigned short version) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMultiBaseBQ & data) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMultiBQTrace & data) = 0;
    //连sds使用
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawStockMultiMin *& pNew, int & intSize) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawStockMultiDayKline *& pNew, int & intSize) = 0;
    virtual bool Uncompress(const char* pBuf, const int intLen, CRawMultiQtEx& data) = 0;
    virtual bool Uncompress(const char* pBuf, const int intLen, CRawMultiCAS& data) = 0;
    virtual bool Uncompress(const char* pBuf, const int intLen, CRawMultiVCM& data) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMultiDayFF& data,
        const unsigned short version) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMarketTurnover& data,
        int marketID, const char* stockID, unsigned short version) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMultiMinuteFF*& data, int& size,
        const unsigned short version) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawListMultiFFItem*& data, int& size,
        bool& isSnapped, const unsigned short version) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawListMultiBuyinRankItem*& data, int& size,
        bool& isSnapped, const unsigned short version) = 0;
    virtual bool Uncompress(const char * pBuf, const int intLen, CRawMultiBlockQt& data,
		const unsigned short version) = 0;
    virtual bool Uncompress(const char* pBuf, const int intLen, RawMultiStockMinuteFundFlow*& ptrData, int& dataSize,
        const unsigned short version) = 0;
};
