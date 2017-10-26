#pragma once
#include "MultiCompressIf.h"
#include "stdstructdef.h"

#define MCM_VERSION 161


class CMultiCompressObj:public CMultiCompressIf {
private:
    unsigned short m_wVer;
    CStdCacheQt m_qt;
    CStdCacheQtEx m_qtex;
    CStdCacheCAS m_cas;
    CStdCacheVCM m_vcm;
    CStdCacheQtS m_qts;
    CStdCacheBlockQt m_blockqt;
    CStdCacheBrokerQueue m_bq;
    CStdCacheBrokerTrace m_trace;
    CStdCacheDayFF cacheDayFF_;
    std::map<int, std::map<std::string, CStdCacheMarketTurnover> > marketTurnovers_;
    std::vector<SRawStockMultiMin> m_rtmin;
    std::vector<SRawStockMultiDayKline> m_kline;
    std::vector<SRawMultiMinuteFF> minuteFF_;
    std::vector<CStdCacheQt> m_qtlist;
    std::vector<CRawMultiQt> m_newqtlist;
    
    std::vector<CStdCacheListFFItem> cacheListFFItems_;
    std::vector<SRawListMultiFFItem> rawListFFItems_;
    std::vector<CStdCacheListBuyinRank> cacheListBuyinRankItems_;
    std::vector<SRawListMultiBuyinRankItem> rawListBuyinRankItems_;

    std::vector<CStdCacheBlockQt> m_qtblocklist;
    std::vector<CRawMultiBlockQt> m_newqtblocklist;
    std::vector<CStdCacheBlockOverView> m_qtviewlist;
    std::vector<SRawBlockOverView> m_newqtviewlist;
    std::vector<SRawMultiRtMin> m_rtmins;
    std::vector<SRawMultiKline> m_klines;
    std::vector<SRawMultiMx> m_mxs;
    std::vector<SRawMultiMx> m_newmxs;
    
    std::vector<RawMultiStockMinuteFundFlow> rawMultiStockMinuteFundFlows_;
public:
    CMultiCompressObj(void);
    ~CMultiCompressObj(void);
    virtual bool Uncompress(const char * pBuf, const int intLen, CRawMultiQt & data);
    virtual bool Uncompress(const char * pBuf, const int intLen, CRawMultiQtS & data);
    virtual bool Uncompress(const char * pBuf, const int intLen, CRawMultiBlockQt & data);
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMultiRtMin *& pNew, int & intSize);
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMultiMx *& pNew, int & intSize);
    virtual bool Uncompress2(const char * pBuf, const int intLen, SRawMultiMx *& pNew, int & intSize);
    virtual bool Uncompress(const char * pBuf, const int intLen, CRawMultiQt *& pNew, int & intSize, bool & blnIsSnap);
    virtual bool Uncompress(const char * pBuf, const int intLen, CRawMultiBlockQt *& pNew, int & intSize, bool & blnIsSnap);
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawBlockOverView *& pNew, \
        int& intSize, bool& blnIsSnap, const unsigned short version);
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMultiBaseBQ & data);
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMultiBQTrace & data);
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawStockMultiMin *& pNew, int & intSize);
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawStockMultiDayKline *& pNew, int & intSize);
    virtual bool Uncompress(const char* pBuf, const int intLen, CRawMultiQtEx& data);
    virtual bool Uncompress(const char* pBuf, const int intLen, CRawMultiCAS& data);
    virtual bool Uncompress(const char* pBuf, const int intLen, CRawMultiVCM& data);
    // Series of Fund Flow Structs.
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMultiDayFF& data,
        const unsigned short version);
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMultiMinuteFF*& data, int& size,
        const unsigned short version);
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawListMultiFFItem*& data, int& size, \
        bool& isSnapped, const unsigned short version);
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawListMultiBuyinRankItem*& data, int& size, \
        bool& isSnapped, const unsigned short version);
    virtual bool Uncompress(const char * pBuf, const int intLen, CRawMultiBlockQt& data, 
        const unsigned short version);
    virtual bool Uncompress(const char * pBuf, const int intLen, RawMultiStockMinuteFundFlow*& ptrData, int& dataSize,
        const unsigned short version);
    virtual bool Uncompress(const char * pBuf, const int intLen, SRawMarketTurnover& data,
        int marketID, const char* stockID, unsigned short version);
};
