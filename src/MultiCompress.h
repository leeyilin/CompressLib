#ifndef MULTICOMPRESS_H_
#define MULTICOMPRESS_H_

#pragma once
#include <vector>
#include <deque>
#include "stdstructdef.h"
#include "output.h"


#define MCM_VERSION       161

static const int MCM_COMPRESS_VERSION = 1;

class CMultiCompress {
public:
	CMultiCompress(void);
	~CMultiCompress(void);
	static void Compress(const CStdCacheQt * pNew, const CStdCacheQt * pOld, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
	static void Compress(const CStdCacheQt * pNew, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static void Compress(const CStdCacheQt * pNew, const CStdCacheQt * pOld, const std::vector<int> & vectIndex, OutputBuffer& buffer,\
        const unsigned short wVer = MCM_COMPRESS_VERSION);
    static void Compress(const CStdCacheQt * pNew, const int intSize, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static void Compress(const CStdCacheQtS * pNew, const CStdCacheQtS * pOld, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static void Compress(const CStdCacheQtS * pNew, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static void Compress(const CStdCacheBlockQt * pNew, const CStdCacheBlockQt * pOld, const std::vector<int> & vectIndex, OutputBuffer& buffer,\
        const unsigned short wVer = MCM_COMPRESS_VERSION);
    static void Compress(const CStdCacheBlockQt * pNew, const int intSize, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static void Compress(const CStdCacheBlockQt * pNew, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static void Compress(const CStdCacheBrokerTrace * pNew, const CStdCacheBrokerTrace * pOld, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static void Compress(const CStdCacheBrokerTrace * pNew, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static void Compress(const CStdCacheBrokerQueue * pNew, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
    // In order to call function to compress BQ more conveniently, I include this new function.
    static void Compress(const CStdCacheBrokerQueue * pNew, const CStdCacheBrokerQueue * pOld, OutputBuffer& buffer,\
        const unsigned short wVer = 160);

    static void Compress(const CStdCacheBlockOverView * pNew, const CStdCacheBlockOverView * pOld, const std::vector<int> & vectIndex, OutputBuffer& buffer,\
        const unsigned short wVer = MCM_COMPRESS_VERSION);
    static void Compress(const CStdCacheBlockOverView * pNew, const int intSize, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static void Compress(const CStdCacheQtEx* pNew, const CStdCacheQtEx* pOld, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
	static void Compress(const CStdCacheCAS* pNew, const CStdCacheCAS* pOld, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
	static void Compress(const CStdCacheVCM* pNew, const CStdCacheVCM* pOld, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
	static void Compress(const std::vector<SStockMultiMin> & data, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
	static void Compress(const std::vector<SStockMultiDayKLine> & data, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
	static void Compress(const CStdCacheRtMin* data, int size, OutputBuffer& buffer, const unsigned short wVer = MCM_COMPRESS_VERSION);
	static void Compress(const CStdCacheMx* data, int size, OutputBuffer& buffer, const unsigned short type);

	static void Compress(const deque<CStdCacheMx> &data,
                         size_t from,
                         size_t compress_count,
                         OutputBuffer& buffer,
                         const unsigned short type);

    static void Compress(const CStdCacheDayFF* newData, const CStdCacheDayFF* oldData,\
        OutputBuffer& buffer, const unsigned short version);
    static void Compress(const CStdCacheMinuteFF* data, int dataSize, OutputBuffer& buffer,\
        const unsigned short version);
    static void Compress(const CStdCacheFFTrend* data, int dataSize, OutputBuffer& buffer,\
        const unsigned short version);
    static void Compress(const std::vector<MultiStockMinuteFundFlow>& datas, OutputBuffer& buffer, const unsigned short version);

    // Intends to compress data of type list for convenient usage in the near future.
    template <typename T>
    static void Compress(const T* ptrNewData, const T* ptrOldData, const std::vector<int>& indexs, OutputBuffer& buffer,\
        const unsigned short version);
    static void Compress(const CStdCacheMarketTurnover* newData, const CStdCacheMarketTurnover* oldData,\
        OutputBuffer& buffer, const unsigned short version);

	static bool Uncompress(const char * pBuf, const int intLen, CStdCacheQt & data, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static bool Uncompress(const char * pBuf, const int intLen, CStdCacheQtS & data, const unsigned short wVer = MCM_COMPRESS_VERSION);
	static bool Uncompress(const char * pBuf, const int intLen, CStdCacheBlockQt & data, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static bool Uncompress(const char * pBuf, const int intLen, CStdCacheBrokerTrace & data, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static bool Uncompress(const char * pBuf, const int intLen, std::vector<CStdCacheRtMin> & data, const unsigned short wVer = MCM_COMPRESS_VERSION);
	static bool Uncompress(const char * pBuf, const int intLen, std::vector<CStdCacheMx> & data, const unsigned short type);
	static bool Uncompress(const char * pBuf, const int intLen, CStdCacheBrokerQueue & data, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static bool Uncompress(const char * pBuf, const int intLen, std::vector<CStdCacheQt> & vectData, std::vector<int> & vectIndex, bool & blnIsSnap,\
        const unsigned short wVer = MCM_COMPRESS_VERSION);
    static bool Uncompress(const char * pBuf, const int intLen, std::vector<CStdCacheBlockQt> & vectData, std::vector<int> & vectIndex, bool & blnIsSnap,\
        const unsigned short wVer = MCM_COMPRESS_VERSION);
    static bool Uncompress(const char * pBuf, const int intLen, std::vector<CStdCacheBlockOverView> & vectData, std::vector<int> & vectIndex, bool & blnIsSnap,\
        const unsigned short wVer = MCM_COMPRESS_VERSION);
	static bool Uncompress(const char * pBuf, const int intLen, std::vector<SStockMultiMin> & data, const unsigned short wVer = MCM_COMPRESS_VERSION);
	static bool Uncompress(const char * pBuf, const int intLen, std::vector<SStockMultiDayKLine> & data, const unsigned short wVer = MCM_COMPRESS_VERSION);
	static bool Uncompress(const char * pBuf, const int intLen, CStdCacheQtEx& data, const unsigned short wVer = MCM_COMPRESS_VERSION);
	static bool Uncompress(const char * pBuf, const int intLen, CStdCacheCAS& data, const unsigned short wVer = MCM_COMPRESS_VERSION);
	static bool Uncompress(const char * pBuf, const int intLen, CStdCacheVCM& data, const unsigned short wVer = MCM_COMPRESS_VERSION);
    static bool Uncompress(const char* charBuffer, const int bufferLength, CStdCacheDayFF& data,\
        const unsigned short version);
    static bool Uncompress(const char* charBuffer, const int bufferLength, CStdCacheMarketTurnover& data,\
        const unsigned short version);    
    static bool Uncompress(const char* charBUffer, const int bufferLength, std::vector<CStdCacheFFTrend>& dataInOneMinute,\
        const unsigned short version);
    static bool Uncompress(const char* charBUffer, const int bufferLength, std::vector<CStdCacheMinuteFF>& dataInOneMinute,\
        const unsigned short version);

    template <typename T>
    static bool Uncompress(const char* buffer, const int bufferLength, std::vector<T>& datas, std::vector<int>& indexs, bool& isSnapped,\
        const unsigned short version);
    static bool Uncompress(const char* pBuf, const int intLen, std::vector<MultiStockMinuteFundFlow>& datas, const unsigned short version);

};

#endif
