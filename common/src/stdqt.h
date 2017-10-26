#ifndef STDQT_H
#define STDQT_H
#include <string.h>
#include "QtTypeDef.h"

#define VSELL5      0       //卖五
#define VSELL4      1       //卖四
#define VSELL3      2       //卖三
#define VSELL2      3       //卖二
#define VSELL1      4       //卖一

#define VBUY1       5       //买一
#define VBUY2       6       //买二
#define VBUY3       7       //买三
#define VBUY4       8       //买四
#define VBUY5       9       //买五

#define MARKET_CODE_LEN 16
#define STOCK_NAME_LEN 32
#define STOCK_CODE_LEN 16
#define STOCK_ID_LEN 32

#pragma pack(1)
struct SStdQt
{
    UINT32      m_dwDate;//交易日YYYYMMDD
    UINT32      m_dwTime;//交易时间hhmmss

    char        m_pcName[STOCK_NAME_LEN]; //股票名称
    char        m_pchCode[STOCK_CODE_LEN];//股票代码
    char        m_pchMarket[MARKET_CODE_LEN];//市场代码

    double      m_dblOpen;//开盘价
    double      m_dblHigh;//最高价
    double      m_dblLow;//最低价
    double      m_dblPrice;//最新价
    double      m_dblClose;// 前日收盘价

    UINT64      m_xVolume;  //成交量
    double      m_dblAmount;//成交额  注：dbf中没有
    UINT64      m_xTradeNum;//成交笔数 注：dbf中没有

    double      m_pdwMMP[10];//买卖盘价格 卖五、卖四。。。买一。。。买五
    UINT64      m_pxMMPVol[10];//买卖盘量

    double      m_dblAvgPrice;  //均价

    UINT64      m_xOpenInterest;    //持仓量
    double      m_dblSettlementPrice; //今结算价
    UINT64      m_xPreOpenInterest;///昨持仓量
    double      m_dblPreSettlementPrice;///昨结算价

    double      m_dblPriceZT;       // 涨停价，美股为52周最高价
    double      m_dblPriceDT;       // 跌停价，美股为52周最低价
    SStdQt()
    {
        memset(this, 0, sizeof(SStdQt));
    }
};

//pre-hour and post-close data
struct SStdQtSlim {
    UINT32      m_dwDate;//trade date, YYYYMMDD
    UINT32      m_dwTime;//trade time, hhmmss

    char        m_pcName[STOCK_NAME_LEN]; //stock name
    char        m_pchCode[STOCK_CODE_LEN];//stock code
    char        m_pchMarket[MARKET_CODE_LEN];//market code

    double      m_dblPrice;//newest price
    UINT64      m_xVolume;  //trade volume

    double      m_pdwMMP[10];//sell5 to buy5 price
    UINT64      m_pxMMPVol[10];//sell5 to buy5 volume

    SStdQtSlim() {
        memset(this, 0, sizeof(SStdQtSlim));
    }   
};

struct SStdMmp
{
    char        m_pchCode[STOCK_CODE_LEN];
    char        m_pchMarket[MARKET_CODE_LEN];
    double      m_pdwMMP[20];
    UINT64      m_pxMMPVol[20];
    SStdMmp()
    {
        memset(this, 0, sizeof(SStdMmp));
    }
};

struct SStdBrokerQueue
{
    SStdBrokerQueue()
    {
        memset(this, 0 , sizeof(SStdBrokerQueue));
    }
    char        m_pchCode[STOCK_CODE_LEN];           //股票代码    #define STOCK_CODE_LEN 16
    char        m_pchMarket[MARKET_CODE_LEN];        //市场代码    #define MARKET_CODE_LEN 16
    char        m_pcBuyBQMoreFlag;           //是否存在更多买盘经纪队列标志位  取值： 'Y'/'N'  
    char        m_pcSellBQMoreFlag;              //是否存在更多卖盘经纪队列标志位  取值：'Y'/'N'  
    BYTE        m_btBuyItemCount;            //买盘经纪队列数量     取值：0~40
    BYTE        m_btSellItemCount;           //卖盘经纪队列数量     取值：0~40

    struct BuyBroker    //买盘经纪队列                     
    {
        char        m_pcBuyType;    //取值：'B': broker number   'S':number of spread   
        WORD        m_wBuyBrokerNo;     
    }ItemBuy[40];

    struct SellBroker   //卖盘经纪队列                    
    {
        char        m_pcSellType;   //取值：'B': broker number   'S':number of spread
        WORD        m_wSellBrokerNo;
    }ItemSell[40];      
};

struct SStdMingXi
{
    SStdMingXi()
    {
        memset(this, 0,sizeof(SStdMingXi));
    }
    char        m_pchCode[STOCK_CODE_LEN];//股票代码
    char        m_pchMarket[MARKET_CODE_LEN];//市场代码
    UINT32      m_dwDate;       //交易日YYYYMMDD
    UINT32      m_dwMxTime;     //交易时间hhmmss
    INT16       m_sMxTrdType;   //成交单标识
    UINT32      m_dwMxPrice;    //最新价
    UINT64      m_xMxVolume;    //成交量
};

struct SStdNewMingXi
{
    SStdNewMingXi()
    {
        memset(this, 0 , sizeof(SStdNewMingXi));
        m_cTrdCancelFlag = 'N';
        m_sMxTrdType = ' ';
    }
    char        m_pchCode[STOCK_CODE_LEN];   //股票代码
    char        m_pchMarket[MARKET_CODE_LEN];//市场代码
    UINT32      m_dwDate;       //交易日YYYYMMDD
    UINT32      m_dwMxTime;     //交易时间hhmmss
    INT16           m_sMxTrdType;      //成交单标识  港股取值：‘ ’，‘P’，‘M’，‘Y’，‘X’，‘D’，‘U’
    UINT32      m_dwMxPrice;       //最新价
    UINT64      m_xMxVolume;       //成交量
    UINT32      m_dwTrickID;       //成交序号
    char        m_cTrdCancelFlag;  //交易取消标志位 ‘Y’：取消此笔交易  ‘N’：此笔正常交易
};

//盘口补充接口
struct SStdQtEx
{
    SStdQtEx()
    {
        memset(this, 0 , sizeof(SStdQtEx));
    }
    char        m_pchCode[STOCK_CODE_LEN];      //股票代码
    char        m_pchMarket[MARKET_CODE_LEN];   //市场代码
    BYTE        m_bTradingStatus;           //代码交易状态   取值:  0：正常交易  2：暂停交易  3：恢复交易
    char        m_cCurrencyCode[3];         //货币种类       取值: 'HKD','USD','EUR','JPY','GBP','CAD','SGD','CNY'
    UINT32      m_dwLotSize;            //该股每手的股数(每只代码不一样)
    char        m_cSpreadTable[2];          //代码价差表     取值： '01':Part A   '03':Part B
    char        m_cCASFlag;                 //代码是否适用于CAS机制     取值：'Y'：是   'N'：否
    char        m_cVCMFlag;                 //代码是否适用于VCM机制     取值：'Y'：是   'N'：否 
    char        m_cShortSellFlag;               //short-sell authorization  取值：'Y'：是   'N'：否  
    char        m_cCCASSFlag;                   //CCASS security        取值：'Y'：是   'N'：否
    char        m_cDummySecurityFlag;           //Dummy Security            取值：'Y'：是   'N'：否
    char        m_cStampDutyFlag;               //stamp duty requirement    取值：'Y'：是   'N'：否
    UINT32      m_dwListingDate;                //上市日期
    UINT32      m_dwDelistingDate;              //退市日期/摘牌日期
    char        m_cFiller[4];           //预留字段

    //债券BOND
    char        m_cEFNFalg;             //代码是否是EFN   取值：'Y'：是   'N'：否
    UINT32      m_dwCouponRate;         //票面利率
    UINT32      m_dwAccruedInt;         //应计利息  注：应计利息＝票面利率÷365天×已计息天数。
    INT32       m_dwYield;              //当前收益率

    //窝轮和结构性产品(牛熊证)WRNT/篮子窝轮BWRT
    char        m_cCallPutFlag;                      //(1)窝轮/篮子窝轮  取值：'C':看涨 'P':看跌 'O'：其他  (2)牛熊证  取值： 'C':牛证 'P':熊证  'O'：其他
    UINT32      m_dwConversionRatio;                 //换股比率
    UINT32      m_dwStrikePrice;                     //行使价
    UINT32      m_dwMaturityDate;                    //到期日 YYYYMMDD 
    char        m_pchUnderlyingCode[STOCK_CODE_LEN]; //对应的标的代码(窝轮和牛熊证只有1个标的;篮子窝轮可能有多个标的，暂只提供1个)
    char        m_cStyle;                            //只用于篮子窝轮  取值：'A'：美式  'E':欧式  ''：其他 

    INT32       m_dwIEPrice;             //参考平衡价
    UINT64      m_xIEVolume;             //参考平衡量
};

//CAS(收市竞价时段)
struct SStdCAS
{
    SStdCAS()
    {
        memset(this, 0 , sizeof(SStdCAS));
    }
    char        m_pchCode[STOCK_CODE_LEN];     //股票代码
    char        m_pchMarket[MARKET_CODE_LEN];  //市场代码
    char        m_cOrderImbalanceDirection;    //订单不平衡方向    取值：'N':Buy = Sell  'B': Buy Surplus  'S': Sell Surplus   ' ':when IEP is not available 
    UINT64      m_xOrderImbalanceQuantity;     //订单不平衡量      取值：Value should be ignored, if Order Imbalance Direction is space
    INT32       m_dwReferencePrice;            //收市竞价参考价
    INT32       m_dwLowerPrice;                //收市竞价价格下限
    INT32       m_xUpperPrice;         //收市竞价价格上限
};

//VCM(市场波动调节机制)
struct SStdVCM
{
    SStdVCM()
    {
        memset(this, 0 , sizeof(SStdVCM));
    }
    char        m_pchCode[STOCK_CODE_LEN];     //股票代码
    char        m_pchMarket[MARKET_CODE_LEN];  //市场代码
    UINT32          m_dwVCMDate;               //熔断日期
    UINT32      m_dwVCMStartTime;              //熔断开始时间
    UINT32      m_dwVCMEndTime;            //熔断结束时间
    INT32       m_dwVCMReferencePrice;         //熔断参考价
    INT32       m_xVCMLowerPrice;              //熔断价格下限
    INT32       m_xVCMUpperPrice;          //熔断价格上限
};

#pragma pack()

#endif
