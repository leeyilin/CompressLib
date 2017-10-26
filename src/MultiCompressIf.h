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
    UINT32        m_dwDate;               //������YYYYMMDD
    UINT32        m_dwTime;               //����ʱ��hhmmss
    char          m_pcName[32];           //��Ʊ����
    char          m_pchCode[16];          //��Ʊ����
    unsigned char m_bytMarket;
    unsigned char m_bytType;
    unsigned char m_bytTradeFlag;         // ���ױ�־��0�����У�1������

    UINT32        m_dwClose;              // ǰ�����̼�
    UINT32        m_dwOpen;               //���̼�
    UINT32        m_dwHigh;               //��߼�
    UINT32        m_dwLow;                //��ͼ�
    UINT32        m_dwPrice;              //���¼�

    UINT64        m_xVolume;              //�ɽ���
    UINT64        m_xAmount;              //�ɽ���
    UINT64        m_xTradeNum;            //�ɽ�����

    UINT32        m_pdwMMP[10];           //�����̼۸�
    UINT64        m_pxMMPVol[10];         //��������

    UINT64        m_xWaiPan;              // ����
    UINT64        m_xCurVol;              // ����
    BYTE          m_cCurVol;              // ���ַ���
    INT64         m_xCurOI;               // �ֲ����仯
    UINT32        m_dwAvg;                /// ����

    UINT64        m_xOpenInterest;        //�ֲ���
    UINT32        m_dwSettlementPrice;    //������
    UINT64        m_xPreOpenInterest;     ///��ֲ���
    UINT32        m_dwPreSettlementPrice; //������

    UINT32        m_dwPriceZT;            // ��ͣ��,����Ϊ52����߼�
    UINT32        m_dwPriceDT;            // ��ͣ��,����Ϊ52����ͼ�
    CRawMultiQt()
    {
        memset(this, 0, sizeof(CRawMultiQt));
    }
};

class CRawMultiQtS
{
public:
    UINT32        m_dwTradeSequence;
    UINT32        m_dwDate;               //������YYYYMMDD
    UINT32        m_dwTime;               //����ʱ��hhmmss
    char          m_pcName[32];           //��Ʊ����
    char          m_pchCode[16];          //��Ʊ����
    unsigned char m_bytMarket;
    unsigned char m_bytType;
    unsigned char m_bytTradeFlag;         // ���ױ�־��0�����У�1������

    UINT32        m_dwClose;              // ǰ�����̼�
    UINT32        m_dwOpen;               //���̼�
    UINT32        m_dwHigh;               //��߼�
    UINT32        m_dwLow;                //��ͼ�
    UINT32        m_dwPrice;              //���¼�

    UINT64        m_xVolume;              //�ɽ���
    UINT64        m_xAmount;              //�ɽ���
    UINT64        m_xTradeNum;            //�ɽ�����

    UINT32        m_pdwMMP[20];           //�����̼۸�
    UINT64        m_pxMMPVol[20];         //��������

    UINT64        m_xWaiPan;              // ����
    UINT64        m_xCurVol;              // ����
    BYTE          m_cCurVol;              // ���ַ���
    INT64         m_xCurOI;               // �ֲ����仯
    UINT32        m_dwAvg;                /// ����

    UINT64        m_xOpenInterest;        //�ֲ���
    UINT32        m_dwSettlementPrice;    //������
    UINT64        m_xPreOpenInterest;     ///��ֲ���
    UINT32        m_dwPreSettlementPrice; //������

    UINT32        m_dwPriceZT;            // ��ͣ��,����Ϊ52����߼�
    UINT32        m_dwPriceDT;            // ��ͣ��,����Ϊ52����ͼ�

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

    // 0����������  2����ͣ����  3���ָ�����
    BYTE m_bTradingStatus;

    // 'HKD','USD','EUR','JPY','GBP','CAD','SGD','CNY'
    char m_cCurrencyCode[3];

    // �ù�ÿ�ֵĹ���(ÿֻ���벻һ��)
    UINT32 m_dwLotSize;

    //����۲�� - '01':Part A   '03':Part B
    char   m_cSpreadTable[2];

    // �Ƿ�������CAS���� - 'Y'���� 'N'����
    char   m_cCASFlag;

    // �Ƿ�������VCM���� - 'Y'���� 'N'����
    char   m_cVCMFlag;

    // short-sell authorization  ȡֵ��'Y'����   'N'����
    char   m_cShortSellFlag;

    // CCASS security        ȡֵ��'Y'����   'N'����
    char   m_cCCASSFlag;

    // Dummy Security            ȡֵ��'Y'����   'N'����
    char   m_cDummySecurityFlag;

    // stamp duty requirement    ȡֵ��'Y'����   'N'����
    char   m_cStampDutyFlag;

    UINT32 m_dwListingDate;   // ��������
    UINT32 m_dwDelistingDate; // ��������/ժ������
    char   m_cFiller[4];      // Ԥ���ֶ�

    //ծȯBOND
    //�����Ƿ���EFN   ȡֵ��'Y'����   'N'����
    char        m_cEFNFalg;
    UINT32         m_dwCouponRate;            //Ʊ������

    //Ӧ����Ϣ  ע��Ӧ����Ϣ��Ʊ�����ʡ�365����Ѽ�Ϣ������
    UINT32      m_dwAccruedInt;

    //��ǰ������
    INT32       m_dwYield;

    //���ֺͽṹ�Բ�Ʒ(ţ��֤)WRNT/��������BWRT
    //
    // (1) ����/��������
    //     ȡֵ��'C':���� 'P':���� 'O'������
    // (2) ţ��֤
    //     ȡֵ�� 'C':ţ֤ 'P':��֤  'O'������
    char        m_cCallPutFlag;
    UINT32      m_dwConversionRatio;                 //���ɱ���
    UINT32      m_dwStrikePrice;                     //��ʹ��

    //������ YYYYMMDD
    UINT32      m_dwMaturityDate;

    // ��Ӧ�ı���г�������:
    //   ���ֺ�ţ��ֻ֤��1�����;
    //   �������ֿ����ж����ģ���ֻ�ṩ1��
    unsigned char m_bytUnderlyingMarket;

    char         m_pchUnderlyingCode[16];

    //ֻ������������  ȡֵ��'A'����ʽ  'E':ŷʽ  ''������
    char        m_cStyle;

    INT32        m_dwIEPrice;             //�ο�ƽ���
    INT64        m_xIEVolume;             //�ο�ƽ����
};

//CAS(���о���ʱ��)
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

    // ������ƽ�ⷽ��
    // ȡֵ��'N': Buy = Sell
    //       'B': Buy Surplus
    //       'S': Sell Surplus
    //       ' ': when IEP is not available
    char    m_cOrderImbalanceDirection;

    // ������ƽ����
    // ȡֵ��Value should be ignored, if Order Imbalance Direction is space
    INT64    m_xOrderImbalanceQuantity;

    //���о��۲ο���
    INT32   m_dwReferencePrice;

    //���о��ۼ۸�����
    INT32   m_dwLowerPrice;

    //���о��ۼ۸�����
    INT32    m_xUpperPrice;
};

//VCM(�г��������ڻ���)
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

    UINT32 m_dwVCMDate;           // �۶�����
    UINT32 m_dwVCMStartTime;      // �۶Ͽ�ʼʱ��
    UINT32 m_dwVCMEndTime;        // �۶Ͻ���ʱ��
    INT32  m_dwVCMReferencePrice; // �۶ϲο���
    INT32  m_xVCMLowerPrice;      // �۶ϼ۸�����
    INT32  m_xVCMUpperPrice;      // �۶ϼ۸�����
};

class CRawMultiBlockQt
{
public:
    UINT32                 m_dwTradeSequence;
    UINT32                 m_dwDate;                 //������YYYYMMDD
    UINT32        m_dwTime;                //����ʱ��hhmmss
    char         m_pcName[32];
    char         m_pchCode[16];
    unsigned char       m_wMarket;
    unsigned char       m_wType;

    UINT32                 m_dwClose;
    UINT32        m_dwOpen;
    UINT32        m_dwHigh;
    UINT32        m_dwLow;
    UINT32        m_dwPrice;

    UINT64        m_xVolume;            //�ɽ���
    UINT64        m_xAmount;                      //�ɽ���

    char        m_pchTopStockCode[32];   //���ǹ�
    unsigned char      m_pchTopStockMarketID; //���ǹ������г�
    UINT32        m_dwStockNum;
    UINT32         m_dwUpNum;                 //�Ǽ���
    UINT32         m_dwDownNum;              //������
    UINT64                 m_xZGB;                        //�ܹɱ�
    UINT64                 m_xZSZ;                        //����ֵ
    UINT64                 m_xAvgProfit;             //ƽ��ӯ��
    INT32                   m_xPeRatio;                 //��ӯ��

    UINT64                 m_xTurnover2Day;         //ǰ2���ܳɽ���   �����ڼ���3�ջ��֣�
    UINT32                 m_xPercent3Day;           //ǰ3�����̼�       �����ڼ���3���Ƿ���
    UINT32                    m_dwPre5MinPrice[5];   // 5���Ӽ۸�          �����ڼ������٣�

    CRawMultiBlockQt()
    {
            memset(this, 0, sizeof(CRawMultiBlockQt));
    };
};

struct SRawBlockOverView {        //��������ӿ�
    char                   m_pchCode[16];
    unsigned short         market;
    unsigned short         type;
    INT64                  m_xNetFlow;
    INT32                  m_dwPercent; // ����Ƿ�, 4λС��

    // ��ֹ����������������������������������N��Ҫ����m_dwPercent�����м��㡣
    // if m_dwPercent <= 0, N = 0
    // else,                N = m_dwUpDay + 1
    UINT32                 m_dwUpDay;
    char                   m_pchTopStockCode[32];
    unsigned char          m_pchTopStockMarketID;
    INT32                  m_dwTopPercent; // ���ǹ��Ƿ�, 4λС��
    SRawBlockOverView() {
        memset(this, 0, sizeof(SRawBlockOverView));
    }
};

struct SRawMultiBaseBQ         //���Ͷ��нӿ�
{
    struct SBrokerNoItem
    {
        BYTE cTabIndex;             //��λ
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

struct SRawMultiBQTrace         //����׷�ٽӿ�
{
    struct STrace
    {
        char strStockID[16];
        unsigned char ucFlag;        //ȡֵ���뿪�̼۱Ƚϣ�    equal: 0   less: 1   greater: 2
        UINT32 dwPrice;
        STrace()
        {
            memset(this, 0, sizeof(STrace));
        }
    };
    struct SBQTraceItem
    {
        BYTE  bTabIndex;             //��λ
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
    UINT32  m_wTime;                //ʱ��(YYMMDDHHMI)

    UINT32 m_dwOpen;                //���̼�
    UINT32 m_dwHigh;                //��߼�
    UINT32 m_dwLow;                //��ͼ�
    UINT32 m_dwClose;                //���̼�
    UINT32 m_dwAve;                //����

    UINT64 m_xVolume;                // �ɽ���
    UINT64 m_xAmount;                // �ɽ���
    UINT64 m_xTradeNum;            // �ɽ�����
    UINT64 m_xWaiPan;            // ����

    INT64 m_xExt1;                //�ڻ�Ϊ�ֲ�����������
    INT64 m_xExt2;                //�ڻ�Ϊ���֣�������

    UINT32  m_dwExt1;                //�ڻ�Ϊ�����
    UINT32  m_dwExt2;                //��
    SRawMultiRtMin()
    {
        memset(this, 0, sizeof(SRawMultiRtMin));
    }
};
/*
struct SRawMultiKline
{
    UINT32 m_time;                                            // ʱ��(YYYYMMDD)
    UINT32 m_Open;                                            // ���̼�
    UINT32 m_Close;                                            // ���̼�
    UINT32 m_High;                                            // ��߼�
    UINT32 m_Low;                                            // ��ͼ�
    UINT64 m_Volume;                                        // �ɽ�����
    UINT64 m_Value;                                        // �ɽ����
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
    UINT32 m_dwDate;        //������YYYYMMDD
    UINT32 m_dwTime;        //����ʱ��hhmmss
    UINT32 m_dwPrice;        //�۸�
    UINT64 m_xVolume;        //�ɽ���
    UINT32 m_dwTradeNum;    //�ɽ�����
    INT32  m_xOI;            //������
    UINT32 m_cBS;            //��������
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
    UINT64 m_buyAmount;   // ����
    UINT64 m_sellAmount;  // ����
    UINT64 m_buyVolume;
    UINT64 m_sellVolume;

    SRawMultiFFItem()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SRawMultiFFOrder
{
    UINT64 m_xAmtOfBuy;	  // ���̳ɽ���
    UINT64 m_xAmtOfSell;  // ���̳ɽ���
    UINT64 m_xVolOfBuy;	  // ���̳ɽ���
    UINT64 m_xVolOfSell;  // ���̳ɽ���
    UINT32 m_dwNumOfBuy;  // ���̳ɽ�����
    UINT32 m_dwNumOfSell; // ���̳ɽ�����

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
    UINT64 amountOfBuys_[SMultiFFOrderType::MAX_ORDER];             // ���̳ɽ���
    UINT64 amountOfSells_[SMultiFFOrderType::MAX_ORDER];            // ���̳ɽ���
    UINT64 volumeOfBuys_[SMultiFFOrderType::MAX_ORDER];             // ���̳ɽ���
    UINT64 volumeOfSells_[SMultiFFOrderType::MAX_ORDER];            // ���̳ɽ���
    UINT32 numberOfBuys_[SMultiFFOrderType::MAX_ORDER];             // ���̳ɽ�����
    UINT32 numberOfSells_[SMultiFFOrderType::MAX_ORDER];            // ���̳ɽ�����
    RawInferiorFundFlow() {
        memset(this, 0, sizeof(*this));
    }
};

typedef SRawMultiMinuteFF SRawMultiFFTrend;

struct SRawMultiDayFF
{
    struct BuyinItem
    {
        float ratio; // ����ռ��
        INT32 rank;  // ��������
    };

    SRawMultiFFOrder m_orders[SMultiFFOrderType::MAX_ORDER];
    BuyinItem   m_ranks[SMultiFFDayType::MAX_DAY];

	SRawMultiDayFF()
    {
        memset(this, 0, sizeof(*this));
    }
};

// �ʽ�����(�б�)
struct SRawListMultiFFItem
{
    unsigned char marketID;
    char   stockCode[16];
    SRawMultiFFItem m_orders[SMultiFFOrderType::MAX_ORDER];
    UINT64 m_callAuctionAmount;        // ���Ͼ��۳ɽ���
    UINT32 m_latestPrice;              // ���¼�
    UINT32 m_prevClose;                // ����

	SRawListMultiFFItem()
    {
        memset(this, 0, sizeof(*this));
    }
};

// ��������(�б�)
struct SRawListMultiBuyinRankItem
{
    struct Item
    {
        UINT32 m_prevClose;   // ǰN�����̼�
        float  m_buyinRatio;  // ����ռ��
        INT32  m_rank;        // ��������
        INT32  m_prevRank;    // ǰһ������
    };

    unsigned char marketID;
    char   stockCode[16];
    Item   m_days[SMultiFFDayType::MAX_DAY];
    UINT32 m_latestPrice;                     // ���¼�

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
    char m_strStockID[16];    //��Ʊ����
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
    char m_strStockID[16];    //��Ʊ����
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
    //��sdsʹ��
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
