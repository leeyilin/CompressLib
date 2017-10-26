#ifdef _MSC_VER
#include "stdafx.h"
#include "LzmaLib.h"
#else
#include "LzmaLib.h"
#endif
#include <string.h>
#include "MultiCompress.h"
#include "MultiCompressIf.h"
#include "Buffer.h"
#include "BitStream.h"
#include "MultiCompressUnit.h"
#include <iostream>

#define MCM_NEW_VERSION1       160
#define _const_min_compress_size 2048

enum _CompressType {
    COMPRESS_TYPE_SNAP = 0,
    COMPRESS_TYPE_ADDED = 1,
};

using namespace std;

template <class CacheDatType>
static void
DoCompress(const CacheDatType* pNew, const CacheDatType* pOld, OutputBuffer& buffer, const unsigned short wVer = 1) {
    buffer.append<char>(!pOld ? COMPRESS_TYPE_ADDED : COMPRESS_TYPE_SNAP);
    COutputBitStream stream(buffer);
    CMultiQtCompressUnit::CompressData(stream, pNew, pOld, wVer);
}

template <class CacheDataType>
static bool
DoUncompress(const char* pBuf, const int intLen, CacheDataType& data, const unsigned short wVer) {
    CPackBuffer buf;
    buf.m_bSingleRead = true;
    buf.Write(pBuf, intLen);

    char isSnap = 0;
    buf.Read(&isSnap, 1);
    CBitStream stream(&buf, true);

    if (isSnap) {
        CMultiQtCompressUnit::ExpandData(stream, &data, (CacheDataType *) NULL, wVer);
    }
    else {
        CacheDataType olddata = data;
        CMultiQtCompressUnit::ExpandData(stream, &data, &olddata, wVer);
    }

    return true;
}

CMultiCompress::CMultiCompress(void) {
}

CMultiCompress::~CMultiCompress(void) {
}

struct _CompressParam {

    _CompressParam() {
        m_intLzmaDictSize = 1024 * 16;
        m_intLzmaLevel = 1;
    }

    int m_intLzmaLevel;
    int m_intLzmaDictSize;
};

bool
CompressZip2(OutputBuffer& buffer, const char * pchPacket, const int intPacketSize) {
    if (pchPacket == NULL || intPacketSize == 0) {
        return false;
    }

    // Take care of the data already in the buffer so that we can append to a
    // buffer as well as write to a new buffer.
    size_t usedBytes = buffer.size();
    buffer.resize(LZMA_PROPS_SIZE + usedBytes + intPacketSize);

    _CompressParam param;
    while (true) {
        size_t ulgCompressSize = buffer.size() - LZMA_PROPS_SIZE - usedBytes;
        unsigned char * pchDest = (unsigned char *) &buffer[usedBytes];
        size_t intPropSize = LZMA_PROPS_SIZE;
        SRes ret = LzmaCompress(pchDest + LZMA_PROPS_SIZE,
            &ulgCompressSize,
            (const unsigned char*) pchPacket,
            intPacketSize, pchDest, &intPropSize,
            param.m_intLzmaLevel,
            param.m_intLzmaDictSize, 3, 0, 2, 32, 1);
        if (SZ_OK != ret) {
            if (ret == SZ_ERROR_OUTPUT_EOF) {
                buffer.resize(buffer.size() * 2);
                continue;
            }
            else {
                buffer.resize(usedBytes);
                return false;
            }
        }
        buffer.resize(ulgCompressSize + LZMA_PROPS_SIZE + usedBytes);
        break;
    }
    return true;
}

bool
UncompressZip2(vector<char> & vectUnCompressedPacket, const char * pchPacket, const int intPacketSize) {
    if (pchPacket == NULL || intPacketSize == 0) {
        CMcmException e;
        e.m_wType = CMcmException::LCME_UZIP;
        e.m_strMsg = "pchPacket == NULL || intPacketSize == 0";
        throw e;
        return false;
    }

    if (vectUnCompressedPacket.size() == 0) {
        vectUnCompressedPacket.resize(intPacketSize);
    }

    while (true) {
        if (intPacketSize <= LZMA_PROPS_SIZE) {
            CMcmException e;
            e.m_wType = CMcmException::LCME_UZIP;
            e.m_strMsg = "intPacketSize <= LZMA_PROPS_SIZE";
            throw e;
            return false;
        }

        unsigned char * pchDest = (unsigned char *) &*vectUnCompressedPacket.begin();
        size_t ulgUnCompressSize = vectUnCompressedPacket.size();
        const unsigned char * pchSrc = (unsigned char*) (pchPacket + LZMA_PROPS_SIZE);
        size_t intSrcSize = (intPacketSize - LZMA_PROPS_SIZE);
        int res = LzmaUncompress2(pchDest, &ulgUnCompressSize, pchSrc, &intSrcSize, (unsigned char*) pchPacket, LZMA_PROPS_SIZE);
        if (res != 0) {
            if (res == 1) {
                vectUnCompressedPacket.resize(vectUnCompressedPacket.size() * 2);
                continue;
            }
            else {
                vectUnCompressedPacket.clear();
                CMcmException e;
                e.m_wType = CMcmException::LCME_UZIP;
                e.m_strMsg = "res != 0";
                throw e;
                return false;
            }
        }
        vectUnCompressedPacket.resize(ulgUnCompressSize);
        break;
    }

    return true;
}

static inline
void
AppendDataWithZip(OutputBuffer& data, OutputBuffer& result) {
    size_t size_without_data = result.size();
    if (data.size() > _const_min_compress_size) {
        result.append(static_cast<unsigned char> (1));
        CompressZip2(result, (char*) data.buffer(0), data.size());
    }
    else {
        result.append(static_cast<unsigned char> (0));
        result.append(data.buffer(0), data.size());
    }
}

void
CMultiCompress::Compress(const CStdCacheQt * pNew, const CStdCacheQt * pOld, OutputBuffer& buffer, const unsigned short wVer) {
    buffer.append<char>(COMPRESS_TYPE_ADDED);
    COutputBitStream stream(buffer);
    CMultiQtCompressUnit::CompressData(stream, pNew, pOld, wVer);
}

void
CMultiCompress::Compress(const CStdCacheQtS * pNew, const CStdCacheQtS * pOld, OutputBuffer& buffer, const unsigned short wVer) {
    buffer.append<char>(COMPRESS_TYPE_ADDED);
    COutputBitStream stream(buffer);
    CMultiQtCompressUnit::CompressData(stream, pNew, pOld, wVer);
}

void
CMultiCompress::Compress(const CStdCacheQt * pNew, OutputBuffer& buffer, const unsigned short wVer) {
    buffer.append<char>(COMPRESS_TYPE_SNAP);
    COutputBitStream stream(buffer);
    CMultiQtCompressUnit::CompressData(stream, pNew, (CStdCacheQt *) NULL, wVer);
}

void
CMultiCompress::Compress(const CStdCacheQtS * pNew, OutputBuffer& buffer, const unsigned short wVer) {
    buffer.append<char>(COMPRESS_TYPE_SNAP);
    COutputBitStream stream(buffer);
    CMultiQtCompressUnit::CompressData(stream, pNew, (CStdCacheQtS*) NULL, wVer);
}

void
CMultiCompress::Compress(const CStdCacheBlockQt * pNew, OutputBuffer& buffer, const unsigned short wVer) {
    buffer.append<char>(COMPRESS_TYPE_SNAP);
    buffer.append(pNew->m_pchTopStockCode);
    COutputBitStream stream(buffer);
    CMultiQtCompressUnit::CompressData(stream, pNew, (CStdCacheBlockQt *) NULL, wVer);
}

void
CMultiCompress::Compress(const CStdCacheBrokerTrace * pNew, const CStdCacheBrokerTrace * pOld, OutputBuffer& buffer, const unsigned short wVer) {
    buffer.append<char>(COMPRESS_TYPE_ADDED);
    buffer.append<unsigned char>(pNew->ucMarket);
    COutputBitStream stream(buffer);
    CMultiQtCompressUnit::CompressData(stream, pNew, pOld, wVer);
}

void
CMultiCompress::Compress(const CStdCacheBrokerTrace * pNew, OutputBuffer& buffer, const unsigned short wVer) {
    buffer.append<char>(COMPRESS_TYPE_SNAP);
    buffer.append<unsigned char>(pNew->ucMarket);
    COutputBitStream stream(buffer);
    CMultiQtCompressUnit::CompressData(stream, pNew, (CStdCacheBrokerTrace*) NULL, wVer);
}

void
CMultiCompress::Compress(const CStdCacheRtMin* data, int size, OutputBuffer& buffer, const unsigned short wVer) {
    LargeStackBuffer tmp;
    tmp.append<UINT32>(size);
    COutputBitStream stream(tmp);
    CMultiRtminCompressUnit::CompressData(stream, data, size, wVer);
    AppendDataWithZip(tmp, buffer);
}

// We will write data directly to buffer without touching the existing data
// their for efficiency. So if the caller want to overwrite an already-used
// buffer, he/she should clear it first.

void
CMultiCompress::Compress(const CStdCacheMx* data, int size, OutputBuffer& buffer, const unsigned short type) {
    LargeStackBuffer tmp;
    tmp.append<UINT32>(size);
    COutputBitStream stream(tmp);
    CMultiMxCompressUnit::CompressData(stream, data, size, type);

    AppendDataWithZip(tmp, buffer);
}

static void Compress();

void CMultiCompress::Compress(
	const deque<CStdCacheMx> &data,
   	size_t from,
   	size_t compress_count,
   	OutputBuffer& buffer,
   	const unsigned short type)
{
    LargeStackBuffer tmp;
    tmp.append<UINT32>(compress_count);
    COutputBitStream stream(tmp);
    CMultiMxCompressUnit::CompressData(stream, data, from, compress_count, type);

    AppendDataWithZip(tmp, buffer);
}

void CMultiCompress::Compress(const CStdCacheMinuteFF* data, int dataSize, OutputBuffer& buffer,\
    const unsigned short version) {
    if (version == MCM_VERSION) {
        LargeStackBuffer tmp;
        tmp.append<unsigned int>(dataSize);
        for (int i = 0; i < dataSize; ++i) {
            COutputBitStream stream(tmp);
            if (i == 0) {
                CMultiQtCompressUnit::CompressData(stream, data + i, (CStdCacheMinuteFF*) NULL, version);
            } else {
                CMultiQtCompressUnit::CompressData(stream, data + i, data + i - 1, version);
            }
        }
        AppendDataWithZip(tmp, buffer);
    }
}

void CMultiCompress::Compress(const CStdCacheFFTrend* data, int dataSize, OutputBuffer& buffer,\
    const unsigned short version) {
    if (version == MCM_VERSION) {
        LargeStackBuffer tmp;
        tmp.append<unsigned int>(dataSize);
        for (int i = 0; i < dataSize; ++i) {
            COutputBitStream stream(tmp);
            if (i == 0) {
                CMultiQtCompressUnit::CompressData(stream, data + i, (CStdCacheFFTrend*) NULL, version);
            } else {
                CMultiQtCompressUnit::CompressData(stream, data + i, data + i - 1, version);
            }
        }
        AppendDataWithZip(tmp, buffer);
    }
}

void
CMultiCompress::Compress(const CStdCacheBrokerQueue * pNew, OutputBuffer& buffer, const unsigned short wVer) {
    SmallStackBuffer tmp;
    COutputBitStream stream(tmp);
    CMultiBQCompressUnit::CompressData(stream, pNew, wVer);

    AppendDataWithZip(tmp, buffer);
}

void
CMultiCompress::Compress(const CStdCacheBrokerQueue * pNew, const CStdCacheBrokerQueue * pOld, OutputBuffer& buffer, const unsigned short wVer) {
    SmallStackBuffer tmp;
    COutputBitStream stream(tmp);
    CMultiBQCompressUnit::CompressData(stream, pNew, pOld, wVer);

    AppendDataWithZip(tmp, buffer);
}

void CMultiCompress::Compress(const vector<SStockMultiMin> & data, OutputBuffer& buffer, const unsigned short wVer) {
    LargeStackBuffer tmp;
    tmp.append<UINT32>(data.size());
    for (size_t i = 0; i < data.size(); i++) {
        const SStockMultiMin & cur = data[i];
        tmp.append<unsigned char>(cur.m_bytMarket);
        tmp.append(cur.m_strStockID);
        tmp.append<UINT32>(cur.m_data.size());
        if (cur.m_data.empty()) {
            COutputBitStream stream(tmp);
            CMultiRtminCompressUnit::CompressData(stream, NULL, cur.m_data.size(), wVer);
        } else {
            COutputBitStream stream(tmp);
            CMultiRtminCompressUnit::CompressData(stream, &cur.m_data[0], cur.m_data.size(), wVer);
        }
    }

    AppendDataWithZip(tmp, buffer);
}

void CMultiCompress::Compress(const std::vector<MultiStockMinuteFundFlow>& datas, OutputBuffer& buffer, const unsigned short version) {
    if (version == MCM_VERSION) {
        LargeStackBuffer tmp;
        tmp.append<UINT32>(datas.size());
        for (size_t i = 0; i < datas.size(); ++i) {
            const MultiStockMinuteFundFlow& cur = datas[i];
            tmp.append<unsigned char>(cur.marketID_);
            tmp.append_string<char>(cur.stockID_);
            std::vector<CStdCacheMinuteFF> minuteFundFlows = cur.getMinuteFundFlows();
            tmp.append<UINT32>(minuteFundFlows.size());
            for (std::size_t j = 0; j < minuteFundFlows.size(); ++j) {
                COutputBitStream stream(tmp);
                if (j == 0) {
                    CMultiQtCompressUnit::CompressData(stream, &(minuteFundFlows[j]), (CStdCacheMinuteFF*) NULL, version);
                } else {
                    CMultiQtCompressUnit::CompressData(stream, &(minuteFundFlows[j]), &(minuteFundFlows[j - 1]), version);
                }
            }
        }

        AppendDataWithZip(tmp, buffer);
    }
}

void
CMultiCompress::Compress(const vector<SStockMultiDayKLine> & data, OutputBuffer& buffer, const unsigned short wVer) {
    LargeStackBuffer tmp;
    tmp.append<UINT32>(data.size());
    for (size_t i = 0; i < data.size(); i++) {
        const SStockMultiDayKLine & cur = data[i];
        tmp.append<unsigned char>(cur.m_bytMarket);
        tmp.append(cur.m_strStockID);
        tmp.append<UINT32>(cur.m_data.size());
        if (cur.m_data.empty()) {
            COutputBitStream stream(tmp);
            CMultiKlineCompressUnit::CompressData(stream, NULL, 0, wVer);
        }
        else {
            COutputBitStream stream(tmp);
            CMultiKlineCompressUnit::CompressData(stream, &cur.m_data[0], cur.m_data.size(), wVer);
        }
    }

    AppendDataWithZip(tmp, buffer);
}

void CMultiCompress::Compress(const CStdCacheQt * pNew, const CStdCacheQt * pOld,
    const vector<int> & vectIndex, OutputBuffer& buffer, const unsigned short wVer) {
    LargeStackBuffer tmp;
    tmp.append<char>(pOld ? COMPRESS_TYPE_ADDED : COMPRESS_TYPE_SNAP);
    tmp.append<UINT32>(vectIndex.size());

    // The vectIndex may be very large, e.g. more than 8000 for HK market. So
    // we don't put the check inside the loop.
    if (pOld) {
        for (size_t i = 0; i < vectIndex.size(); i++) {
            const CStdCacheQt * pCurNew = pNew + vectIndex[i];
            const CStdCacheQt * pCurOld = pOld + vectIndex[i];
            tmp.append<char>(pCurNew->m_wMarket);
            tmp.append_string<char>(pCurNew->m_pchCode);
            COutputBitStream stream(tmp);
            CMultiQtCompressUnit::CompressAddData(stream, pCurNew, pCurOld, wVer);
        }
    } else {
        for (size_t i = 0; i < vectIndex.size(); i++) {
            const CStdCacheQt * pCurNew = pNew + vectIndex[i];
            tmp.append<char>(pCurNew->m_wMarket);
            tmp.append_string<char>(pCurNew->m_pchCode);
            COutputBitStream stream(tmp);
            CMultiQtCompressUnit::CompressSnapData(stream, pCurNew, wVer);
        }
    }
    AppendDataWithZip(tmp, buffer);
}

template <typename T>
void CMultiCompress::Compress(const T* ptrNewData, const T* ptrOldData, const std::vector<int>& indexs, OutputBuffer& buffer,\
    const unsigned short version) {
    if (version == MCM_VERSION) {
        LargeStackBuffer tmp;
        tmp.append<char>(ptrOldData ? COMPRESS_TYPE_ADDED : COMPRESS_TYPE_SNAP);
        tmp.append<UINT32>(indexs.size());

        // The vectIndex may be very large, e.g. more than 8000 for HK market. So
        // we don't put the check inside the loop.
        if (ptrOldData) {
            for (size_t i = 0; i < indexs.size(); i++) {
                const T* pCurNew = ptrNewData + indexs[i];
                const T* pCurOld = ptrOldData + indexs[i];
                tmp.append<unsigned char>(pCurNew->marketID);
                tmp.append_string<char>(pCurNew->stockCode);
                COutputBitStream stream(tmp);
                CMultiQtCompressUnit::CompressData(stream, pCurNew, pCurOld, version);
            }
        } else {
            for (size_t i = 0; i < indexs.size(); i++) {
                const T* pCurNew = ptrNewData + indexs[i];
                tmp.append<unsigned char>(pCurNew->marketID);
                tmp.append_string<char>(pCurNew->stockCode);
                COutputBitStream stream(tmp);
                CMultiQtCompressUnit::CompressData(stream, pCurNew, (T*)NULL, version);
            }
        }
        AppendDataWithZip(tmp, buffer);
    }
}

template
void CMultiCompress::Compress<CStdCacheListFFItem>(const CStdCacheListFFItem*, const CStdCacheListFFItem*, \
    const std::vector<int>&, OutputBuffer& , const unsigned short);

template
void CMultiCompress::Compress<CStdCacheListBuyinRank>(const CStdCacheListBuyinRank*, const CStdCacheListBuyinRank*, \
    const std::vector<int>&, OutputBuffer& , const unsigned short);

void
CMultiCompress::Compress(const CStdCacheBlockQt * pNew, const CStdCacheBlockQt * pOld, const vector<int> & vectIndex, OutputBuffer& buffer, const unsigned short wVer) {
    LargeStackBuffer tmp;
    tmp.append<char>(pOld ? COMPRESS_TYPE_ADDED : COMPRESS_TYPE_SNAP);
    tmp.append<UINT32>(vectIndex.size());
    if (pOld) {
        for (size_t i = 0; i < vectIndex.size(); i++) {
            const CStdCacheBlockQt * pCurNew = pNew + vectIndex[i];
            const CStdCacheBlockQt * pCurOld = pOld + vectIndex[i];
            tmp.append<char>(pCurNew->m_wMarket);
            tmp.append_string<char>(pCurNew->m_pchCode);
            tmp.append_string<char>(pCurNew->m_pchTopStockCode);
            COutputBitStream stream(tmp);
            CMultiQtCompressUnit::CompressAddData(stream, pCurNew, pCurOld, wVer);
        }
    }
    else {
        for (size_t i = 0; i < vectIndex.size(); i++) {
            const CStdCacheBlockQt * pCurNew = pNew + vectIndex[i];
            tmp.append<char>(pCurNew->m_wMarket);
            tmp.append_string<char>(pCurNew->m_pchCode);
            tmp.append_string<char>(pCurNew->m_pchTopStockCode);
            COutputBitStream stream(tmp);
            CMultiQtCompressUnit::CompressSnapData(stream, pCurNew, wVer);
        }
    }

    AppendDataWithZip(tmp, buffer);
}

void
CMultiCompress::Compress(const CStdCacheQt * pNew, const int intSize, OutputBuffer& buffer, const unsigned short wVer) {
    LargeStackBuffer tmp;
    tmp.append<char>(COMPRESS_TYPE_SNAP);
    tmp.append<UINT32>(intSize);
    for (int i = 0; i < intSize; i++) {
        const CStdCacheQt * pCurNew = pNew + i;
        tmp.append<char>(pCurNew->m_wMarket);
        tmp.append_string<char>(pCurNew->m_pchCode);
        COutputBitStream stream(tmp);
        CMultiQtCompressUnit::CompressSnapData(stream, pCurNew, wVer);
    }

    AppendDataWithZip(tmp, buffer);
}

void
CMultiCompress::Compress(const CStdCacheBlockQt * pNew, const int intSize, OutputBuffer& buffer, const unsigned short wVer) {
    LargeStackBuffer tmp;
    tmp.append<char>(COMPRESS_TYPE_SNAP);
    tmp.append<UINT32>(intSize);
    for (int i = 0; i < intSize; i++) {
        const CStdCacheBlockQt * pCurNew = pNew + i;
        tmp.append<char>(pCurNew->m_wMarket);
        tmp.append_string<char>(pCurNew->m_pchCode);
        tmp.append_string<char>(pCurNew->m_pchTopStockCode);
        COutputBitStream stream(tmp);
        CMultiQtCompressUnit::CompressSnapData(stream, pCurNew, wVer);
    }

    AppendDataWithZip(tmp, buffer);
}

void CMultiCompress::Compress(const CStdCacheBlockOverView* ptrNewData, const CStdCacheBlockOverView* ptrOldData,
    const std::vector<int>& indexs, OutputBuffer& buffer, const unsigned short version) {
    if (version == MCM_COMPRESS_VERSION) {
        if (ptrOldData) {
            LargeStackBuffer tmp;
            tmp.append<char>(COMPRESS_TYPE_ADDED);
            tmp.append<UINT32>(indexs.size());
            for (size_t i = 0; i < indexs.size(); i++) {
                const CStdCacheBlockOverView* pCurNew = ptrNewData + indexs[i];
                const CStdCacheBlockOverView* pCurOld = ptrOldData + indexs[i];
                tmp.append<char>(pCurNew->m_wMarket);
                tmp.append_string<char>(pCurNew->m_pchCode);
                tmp.append_string<char>(pCurNew->m_pchTopStockCode);
                COutputBitStream stream(tmp);
                CMultiQtCompressUnit::CompressAddData(stream, pCurNew, pCurOld, version);
            }
            AppendDataWithZip(tmp, buffer);
        } else {
            std::vector<CStdCacheBlockOverView> views;
            for (size_t i = 0; i < indexs.size(); i++) {
                views.push_back(ptrNewData[indexs[i]]);
            }

            CMultiCompress::Compress(&*views.begin(), views.size(), buffer, version);
        }
    } else if (version == MCM_VERSION) {
        LargeStackBuffer tmp;
        tmp.append<char>(ptrOldData ? COMPRESS_TYPE_ADDED : COMPRESS_TYPE_SNAP);
        tmp.append<UINT32>(indexs.size());

        // The vectIndex may be very large, e.g. more than 8000 for HK market. So
        // we don't put the check inside the loop.
        if (ptrOldData) {
            for (size_t i = 0; i < indexs.size(); i++) {
                const CStdCacheBlockOverView* pCurNew = ptrNewData + indexs[i];
                const CStdCacheBlockOverView* pCurOld = ptrOldData + indexs[i];
                tmp.append<unsigned char>(pCurNew->m_wMarket);
                tmp.append<unsigned char>(pCurNew->m_pchTopStockMarketID);
                tmp.append_string<char>(pCurNew->m_pchCode);
                tmp.append_string<char>(pCurNew->m_pchTopStockCode);
                COutputBitStream stream(tmp);
                CMultiQtCompressUnit::CompressData(stream, pCurNew, pCurOld, version);
            }
        } else {
            for (size_t i = 0; i < indexs.size(); i++) {
                const CStdCacheBlockOverView* pCurNew = ptrNewData + indexs[i];
                tmp.append<unsigned char>(pCurNew->m_wMarket);
                tmp.append<unsigned char>(pCurNew->m_pchTopStockMarketID);
                tmp.append_string<char>(pCurNew->m_pchCode);
                tmp.append_string<char>(pCurNew->m_pchTopStockCode);
                COutputBitStream stream(tmp);
                CMultiQtCompressUnit::CompressData(stream, pCurNew, (CStdCacheBlockOverView*)NULL, version);
            }
        }
        AppendDataWithZip(tmp, buffer);
    }
}

void
CMultiCompress::Compress(const CStdCacheBlockOverView * pNew, const int intSize, OutputBuffer& buffer, const unsigned short wVer) {
    LargeStackBuffer tmp;
    tmp.append<char>(COMPRESS_TYPE_SNAP);
    tmp.append<UINT32>(intSize);
    for (int i = 0; i < intSize; i++) {
        const CStdCacheBlockOverView * pCurNew = pNew + i;
        tmp.append<char>(pCurNew->m_wMarket);
        tmp.append_string<char>(pCurNew->m_pchCode);
        tmp.append_string<char>(pCurNew->m_pchTopStockCode);
        COutputBitStream stream(tmp);
        CMultiQtCompressUnit::CompressSnapData(stream, pCurNew, wVer);
    }

    AppendDataWithZip(tmp, buffer);
}

void
CMultiCompress::Compress(const CStdCacheQtEx* pNew, const CStdCacheQtEx* pOld, OutputBuffer& buffer, const unsigned short wVer) {
    DoCompress(pNew, pOld, buffer, wVer);
}

void
CMultiCompress::Compress(const CStdCacheCAS* pNew, const CStdCacheCAS* pOld, OutputBuffer& buffer, const unsigned short wVer) {
    DoCompress(pNew, pOld, buffer, wVer);
}

void
CMultiCompress::Compress(const CStdCacheVCM* pNew, const CStdCacheVCM* pOld, OutputBuffer& buffer, const unsigned short wVer) {
    DoCompress(pNew, pOld, buffer, wVer);
}

bool
CMultiCompress::Uncompress(const char * pBuf, const int intLen, CStdCacheQt & data, const unsigned short wVer) {
    CPackBuffer buf;
    buf.m_bSingleRead = true;
    buf.Write(pBuf, intLen);
    char chrFlag = 0;
    buf.Read(&chrFlag, 1);
    //buf.Read(&data, sizeof(data));
    if (chrFlag) {
        CBitStream stream(&buf, true);
        CStdCacheQt olddata = data;
        CMultiQtCompressUnit::ExpandData(stream, &data, &olddata, wVer);
    }
    else {
        CBitStream stream(&buf, true);
        CMultiQtCompressUnit::ExpandData(stream, &data, (CStdCacheQt *) NULL, wVer);
    }
    return true;
}

bool
CMultiCompress::Uncompress(const char * pBuf, const int intLen, CStdCacheQtS & data, const unsigned short wVer) {
    CPackBuffer buf;
    buf.m_bSingleRead = true;
    buf.Write(pBuf, intLen);
    char chrFlag = 0;
    buf.Read(&chrFlag, 1);
    //buf.Read(&data, sizeof(data));
    if (chrFlag) {
        CBitStream stream(&buf, true);
        CStdCacheQtS olddata = data;
        CMultiQtCompressUnit::ExpandData(stream, &data, &olddata, wVer);
    }
    else {
        CBitStream stream(&buf, true);
        CMultiQtCompressUnit::ExpandData(stream, &data, (CStdCacheQtS*) NULL, wVer);
    }

    return true;
}

bool
CMultiCompress::Uncompress(const char * pBuf, const int intLen, CStdCacheBlockQt & data, const unsigned short wVer) {
    CPackBuffer buf;
    buf.m_bSingleRead = true;
    buf.Write(pBuf, intLen);
    char chrFlag = 0;
    buf.Read(&chrFlag, 1);

    //NOTE: add a new buffer   added by litaozhi
    char pchTopStockCode[32];
    memset(pchTopStockCode, 0, sizeof (pchTopStockCode));
    buf.Read(pchTopStockCode, 32);
    strncpy(data.m_pchTopStockCode, pchTopStockCode, 32);

    //buf.Read(&data, sizeof(data));
    if (chrFlag) {
        CBitStream stream(&buf, true);
        CStdCacheBlockQt olddata = data;
        CMultiQtCompressUnit::ExpandData(stream, &data, &olddata, wVer);
    }
    else {
        CBitStream stream(&buf, true);
        CMultiQtCompressUnit::ExpandData(stream, &data, (CStdCacheBlockQt *) NULL, wVer);
    }
    return true;
}

bool
CMultiCompress::Uncompress(const char * pBuf, const int intLen, CStdCacheBrokerTrace & data, const unsigned short wVer) {
    CPackBuffer buf;
    buf.m_bSingleRead = true;
    buf.Write(pBuf, intLen);
    char chrFlag = 0;
    buf.Read(&chrFlag, 1);
    buf.Read(&(data.ucMarket), 1);
    if (chrFlag) {
        CBitStream stream(&buf, true);
        CStdCacheBrokerTrace olddata = data;
        CMultiQtCompressUnit::ExpandData(stream, &data, &olddata, wVer);
    }
    else {
        CBitStream stream(&buf, true);
        CMultiQtCompressUnit::ExpandData(stream, &data, (CStdCacheBrokerTrace*) NULL, wVer);
    }
    return true;
}

bool
CMultiCompress::Uncompress(const char * pBuf, const int intLen, vector<CStdCacheRtMin> & data, const unsigned short wVer) {
    CPackBuffer buf;
    buf.m_bSingleRead = true;
    if (pBuf[0] == 1) {
        vector<char> vectUncompressdData;
        if (UncompressZip2(vectUncompressdData, pBuf + 1, intLen - 1) == false) {
            return false;
        }

        if (vectUncompressdData.size() == 0) {
            return false;
        }

        buf.Write(&*vectUncompressdData.begin(), vectUncompressdData.size());
    }
    else {
        buf.Write(pBuf + 1, intLen - 1);
    }

    UINT32 dwSubSize = 0;
    buf.Read(&dwSubSize, sizeof (dwSubSize));
    if (dwSubSize) {
        CBitStream stream(&buf, true);
        CMultiRtminCompressUnit::ExpandData(stream, data, wVer);
        //data.resize(dwSubSize);
        //buf.Read(&*data.begin(), data.size() * sizeof(CStdCacheRtMin));
    }

    return true;
}

bool
CMultiCompress::Uncompress(const char * pBuf, const int intLen, vector<CStdCacheMx> & data, const unsigned short type) {
    CPackBuffer buf;
    buf.m_bSingleRead = true;
    if (pBuf[0] == 1) {
        vector<char> vectUncompressdData;
        if (UncompressZip2(vectUncompressdData, pBuf + 1, intLen - 1) == false) {
            return false;
        }

        if (vectUncompressdData.size() == 0) {
            return false;
        }

        buf.Write(&*vectUncompressdData.begin(), vectUncompressdData.size());
    }
    else {
        buf.Write(pBuf + 1, intLen - 1);
    }

    UINT32 dwSubSize = 0;
    buf.Read(&dwSubSize, sizeof (dwSubSize));
    if (dwSubSize) {
        CBitStream stream(&buf, true);
        CMultiMxCompressUnit::ExpandData(stream, data, type);
        //data.resize(dwSubSize);
        //buf.Read(&*data.begin(), data.size() * sizeof(CStdCacheMx));
    }
    return true;
}

bool
CMultiCompress::Uncompress(const char* charBUffer, const int bufferLength, std::vector<CStdCacheMinuteFF>& dataInOneMinute,\
    const unsigned short version) {
    if (version == MCM_VERSION) {
        CPackBuffer buf;
        buf.m_bSingleRead = true;
        if (charBUffer[0] == 1) {
            std::vector<char> vectUncompressdData;
            if ((!UncompressZip2(vectUncompressdData, charBUffer + 1, bufferLength - 1)) || vectUncompressdData.empty())
                return false;

            buf.Write(&*vectUncompressdData.begin(), vectUncompressdData.size());
        }
        else {
            buf.Write(charBUffer + 1, bufferLength - 1);
        }
        unsigned int dataSize = 0;
        buf.Read(&dataSize, sizeof (dataSize));
        if (dataSize) {
            CStdCacheMinuteFF lastMinuteFF;
            for (unsigned int i = 0; i < dataSize; ++i) {
                CStdCacheMinuteFF minuteFF;
                if (i == 0) {
                    CBitStream stream(&buf, true);
                    CMultiQtCompressUnit::ExpandData(stream, &minuteFF, (CStdCacheMinuteFF*) NULL, version);
                }
                else {
                    CBitStream stream(&buf, true);
                    CMultiQtCompressUnit::ExpandData(stream, &minuteFF, &lastMinuteFF, version);
                }
                lastMinuteFF = minuteFF;
                dataInOneMinute.push_back(minuteFF);
            }
        }
    }
    return true;
}

bool
CMultiCompress::Uncompress(const char* charBUffer, const int bufferLength, std::vector<CStdCacheFFTrend>& dataInOneMinute,\
    const unsigned short version) {
    if (version == MCM_VERSION) {
        CPackBuffer buf;
        buf.m_bSingleRead = true;
        if (charBUffer[0] == 1) {
            std::vector<char> vectUncompressdData;
            if ((!UncompressZip2(vectUncompressdData, charBUffer + 1, bufferLength - 1)) || vectUncompressdData.empty())
                return false;

            buf.Write(&*vectUncompressdData.begin(), vectUncompressdData.size());
        }
        else {
            buf.Write(charBUffer + 1, bufferLength - 1);
        }
        unsigned int dataSize = 0;
        buf.Read(&dataSize, sizeof (dataSize));
        if (dataSize) {
            CStdCacheFFTrend lastMinuteFF;
            for (unsigned int i = 0; i < dataSize; ++i) {
                CStdCacheFFTrend minuteFF;
                if (i == 0) {
                    CBitStream stream(&buf, true);
                    CMultiQtCompressUnit::ExpandData(stream, &minuteFF, (CStdCacheFFTrend*) NULL, version);
                }
                else {
                    CBitStream stream(&buf, true);
                    CMultiQtCompressUnit::ExpandData(stream, &minuteFF, &lastMinuteFF, version);
                }
                lastMinuteFF = minuteFF;
                dataInOneMinute.push_back(minuteFF);
            }
        }
    }
    return true;
}


bool
CMultiCompress::Uncompress(const char * pBuf, const int intLen, CStdCacheBrokerQueue & data, const unsigned short wVer) {
    CPackBuffer buf;
    buf.m_bSingleRead = true;
    if (pBuf[0] == 1) {
        vector<char> vectUncompressdData;
        if (UncompressZip2(vectUncompressdData, pBuf + 1, intLen - 1) == false) {
            return false;
        }

        if (vectUncompressdData.size() == 0) {
            return false;
        }

        buf.Write(&*vectUncompressdData.begin(), vectUncompressdData.size());
    }
    else {
        buf.Write(pBuf + 1, intLen - 1);
    }

    CBitStream stream(&buf, true);
    if (wVer != MCM_NEW_VERSION1) {
        CMultiBQCompressUnit::ExpandData(stream, &data, wVer);
    }
    else {
        CStdCacheBrokerQueue tmp(data);
        CMultiBQCompressUnit::ExpandData(stream, &data, &tmp, wVer);
    }
    return true;
}

bool
CMultiCompress::Uncompress(const char * pBuf, const int intLen, vector<SStockMultiMin> & data, const unsigned short wVer) {
    CPackBuffer buf;
    buf.m_bSingleRead = true;
    if (pBuf[0] == 1) {
        vector<char> vectUncompressdData;
        if (UncompressZip2(vectUncompressdData, pBuf + 1, intLen - 1) == false) {
            return false;
        }

        if (vectUncompressdData.size() == 0) {
            return false;
        }

        buf.Write(&*vectUncompressdData.begin(), vectUncompressdData.size());
    }
    else {
        buf.Write(pBuf + 1, intLen - 1);
    }

    UINT32 dwSize = 0;
    int intDate = 0;
    buf.Read(&dwSize, sizeof (dwSize));
    for (size_t i = 0; i < dwSize; i++) {
        SStockMultiMin cur;
        buf.Read(&cur.m_bytMarket, sizeof (cur.m_bytMarket));
        buf.Read(cur.m_strStockID, sizeof (cur.m_strStockID));
        UINT32 dwSubSize = 0;
        buf.Read(&dwSubSize, sizeof (dwSubSize));
        CBitStream stream(&buf, true);
        CMultiRtminCompressUnit::ExpandData(stream, cur.m_data, wVer);
        //cur.m_data.resize(dwSubSize);
        //buf.Read(&*cur.m_data.begin(), cur.m_data.size() * sizeof(CStdCacheRtMin));
        data.push_back(cur);
    }

    return true;
}

bool CMultiCompress::Uncompress(const char* pBuf, const int intLen, std::vector<MultiStockMinuteFundFlow>& datas, const unsigned short version) {
    if (version == MCM_VERSION) {
        CPackBuffer buf;
        buf.m_bSingleRead = true;
        if (pBuf[0] == 1) {
            std::vector<char> vectUncompressdData;
            if (!UncompressZip2(vectUncompressdData, pBuf + 1, intLen - 1))
                return false;

            if (vectUncompressdData.empty())
                return false;

            buf.Write(&*vectUncompressdData.begin(), vectUncompressdData.size());
        } else {
            buf.Write(pBuf + 1, intLen - 1);
        }

        UINT32 dwSize = 0;
        buf.Read(&dwSize, sizeof (dwSize));
        for (size_t i = 0; i < dwSize; ++i) {
            MultiStockMinuteFundFlow cur;
            buf.Read(&cur.marketID_, sizeof(cur.marketID_));
            char cLen = 0;
            buf.Read(&cLen, sizeof (cLen));
            buf.Read(cur.stockID_, cLen);
            UINT32 dwSubSize = 0;
            buf.Read(&dwSubSize, sizeof (dwSubSize));
            CStdCacheMinuteFF lastMinuteFF;
            std::vector<CStdCacheMinuteFF> minuteFundFlows;
            for (unsigned int j = 0; j < dwSubSize; ++j) {
                CStdCacheMinuteFF minuteFF;
                if (j == 0) {
                    CBitStream stream(&buf, true);
                    CMultiQtCompressUnit::ExpandData(stream, &minuteFF, (CStdCacheMinuteFF*) NULL, version);
                } else {
                    CBitStream stream(&buf, true);
                    CMultiQtCompressUnit::ExpandData(stream, &minuteFF, &lastMinuteFF, version);
                }
                lastMinuteFF = minuteFF;
                minuteFundFlows.push_back(minuteFF);
            }
            cur.restoreFromMinuteFundFlows(minuteFundFlows);
            datas.push_back(cur);
        }

        return true;
    }
}

bool
CMultiCompress::Uncompress(const char * pBuf, const int intLen, vector<SStockMultiDayKLine> & data, const unsigned short wVer) {
    CPackBuffer buf;
    buf.m_bSingleRead = true;
    if (pBuf[0] == 1) {
        vector<char> vectUncompressdData;
        if (UncompressZip2(vectUncompressdData, pBuf + 1, intLen - 1) == false) {
            return false;
        }

        if (vectUncompressdData.size() == 0) {
            return false;
        }

        buf.Write(&*vectUncompressdData.begin(), vectUncompressdData.size());
    }
    else {
        buf.Write(pBuf + 1, intLen - 1);
    }

    UINT32 dwSize = 0;
    int intDate = 0;
    buf.Read(&dwSize, sizeof (dwSize));
    for (size_t i = 0; i < dwSize; i++) {
        SStockMultiDayKLine cur;
        buf.Read(&cur.m_bytMarket, sizeof (cur.m_bytMarket));
        buf.Read(cur.m_strStockID, sizeof (cur.m_strStockID));
        UINT32 dwSubSize = 0;
        buf.Read(&dwSubSize, sizeof (dwSubSize));
        CBitStream stream(&buf, true);
        CMultiKlineCompressUnit::ExpandData(stream, cur.m_data, wVer);
        //cur.m_data.resize(dwSubSize);
        //buf.Read(&*cur.m_data.begin(), cur.m_data.size() * sizeof(CStdCacheKline));
        data.push_back(cur);
    }

    return true;
}

bool
CMultiCompress::Uncompress(const char * pBuf, const int intLen, vector<CStdCacheQt> & vectData, vector<int> & vectIndex, bool & blnIsSnap, const unsigned short wVer) {
    CPackBuffer buf;
    buf.m_bSingleRead = true;
    if (pBuf[0] == 1) {
        vector<char> vectUncompressdData;
        if (UncompressZip2(vectUncompressdData, pBuf + 1, intLen - 1) == false) {
            return false;
        }

        if (vectUncompressdData.size() == 0) {
            return false;
        }

        buf.Write(&*vectUncompressdData.begin(), vectUncompressdData.size());
    }
    else {
        buf.Write(pBuf + 1, intLen - 1);
    }

    char chrFlag = 0;
    buf.Read(&chrFlag, sizeof (chrFlag));
    if (chrFlag) {
        if (vectData.size() == 0) {
            //CLcmException e;
            //e.m_wType = CLcmException::LCME_INCREMENT;
            //e.m_strMsg = "vectData.size() == 0";
            //throw e;
            return false;
        }

        UINT32 dwSize = 0;
        buf.Read(&dwSize, sizeof (dwSize));
        for (size_t i = 0; i < dwSize; i++) {
            unsigned char cMarket = 0;
            buf.Read(&cMarket, sizeof (cMarket));
            char pchCode[16];
            memset(pchCode, 0, sizeof (pchCode));
            char cLen = 0;
            buf.Read(&cLen, sizeof (cLen));
            buf.Read(pchCode, cLen);
            int intIndex = -1;
            for (size_t j = 0; j < vectData.size(); j++) {
                if (cMarket == vectData[j].m_wMarket && strncmp(pchCode, vectData[j].m_pchCode, sizeof (pchCode)) == 0) {
                    intIndex = j;
                    break;
                }
            }

            if (intIndex == -1) {
                //CLcmException e;
                //e.m_wType = CLcmException::LCME_INCREMENT;
                //char pTmp[256];
                //char pStock[16];
                //char cMarket;
                //CCovertMarket::ConvertMarketToChar(dwStockID, pStock, cMarket);
                //sprintf(pTmp, "Unknow stock '%s' vectData.size()=%d" , pStock, vectData.size());
                //e.m_strMsg = pTmp;
                //throw e;
                return false;
            }

            CStdCacheQt rec = vectData[intIndex];
            CBitStream stream(&buf, true);
            CMultiQtCompressUnit::ExpandAddData(stream, &rec, &vectData[intIndex], wVer);
            vectData[intIndex] = rec;
            vectIndex.push_back(intIndex);
        }

        blnIsSnap = false;
    }
    else {
        UINT32 dwSize = 0;
        buf.Read(&dwSize, sizeof (dwSize));
        vectData.clear();
        for (size_t i = 0; i < dwSize; i++) {
            CStdCacheQt rec;
            char cLen = 0;
            unsigned char cMarket = 0;
            buf.Read(&cMarket, sizeof (cMarket));
            rec.m_wMarket = cMarket;
            buf.Read(&cLen, sizeof (cLen));
            buf.Read(rec.m_pchCode, cLen);
            CBitStream stream(&buf, true);
            CMultiQtCompressUnit::ExpandSnapData(stream, &rec, wVer);
            vectData.push_back(rec);
            vectIndex.push_back(i);
        }

        blnIsSnap = true;
    }
    return true;
}

template <typename T>
bool CMultiCompress::Uncompress(const char* buffer, const int bufferLength, std::vector<T>& datas, std::vector<int>& indexs, bool & isSnapped,\
    const unsigned short version) {
    if (version == MCM_VERSION) {
        CPackBuffer buf;
        buf.m_bSingleRead = true;
        if (buffer[0] == 1) {
            std::vector<char> vectUncompressdData;
            if (!UncompressZip2(vectUncompressdData, buffer + 1, bufferLength - 1))
                return false;

            if (vectUncompressdData.empty())
                return false;

            buf.Write(&*vectUncompressdData.begin(), vectUncompressdData.size());
        } else {
            buf.Write(buffer + 1, bufferLength - 1);
        }

        char chrFlag = 0;
        buf.Read(&chrFlag, sizeof (chrFlag));
        if (chrFlag) {
            if (datas.empty())
                return false;

            UINT32 dwSize = 0;
            buf.Read(&dwSize, sizeof (dwSize));
            for (size_t i = 0; i < dwSize; i++) {
                unsigned char marketID = 0;
                buf.Read(&marketID, sizeof(marketID));
                char stockCode[16];
                char cLen = 0;
                buf.Read(&cLen, sizeof (cLen));
                buf.Read(stockCode, cLen);
                int intIndex = -1;
                for (size_t j = 0; j < datas.size(); j++) {
                    if (strncmp(stockCode, datas[j].stockCode, cLen) == 0
                        && marketID == datas[j].marketID) {
                        intIndex = j;
                        break;
                    }
                }

                if (intIndex == -1)
                    return false;

                T rec = datas[intIndex];
                CBitStream stream(&buf, true);
                CMultiQtCompressUnit::ExpandData(stream, &rec, &datas[intIndex], version);
                datas[intIndex] = rec;
                indexs.push_back(intIndex);
            }

            isSnapped = false;
        } else {
            UINT32 dwSize = 0;
            buf.Read(&dwSize, sizeof (dwSize));
            datas.clear();
            for (size_t i = 0; i < dwSize; i++) {
                T rec;
                buf.Read(&rec.marketID, sizeof(rec.marketID));
                char cLen = 0;
                buf.Read(&cLen, sizeof (cLen));
                buf.Read(rec.stockCode, cLen);
                CBitStream stream(&buf, true);
                CMultiQtCompressUnit::ExpandData(stream, &rec, (T*)NULL, version);
                datas.push_back(rec);
                indexs.push_back(i);
            }

            isSnapped = true;
        }
        return true;
    }
}

template
bool CMultiCompress::Uncompress<CStdCacheListFFItem>(const char*, const int, std::vector<CStdCacheListFFItem>&, \
    std::vector<int>&, bool&, const unsigned short);

template
bool CMultiCompress::Uncompress<CStdCacheListBuyinRank>(const char*, const int, std::vector<CStdCacheListBuyinRank>&, \
    std::vector<int>&, bool&, const unsigned short);

bool
CMultiCompress::Uncompress(const char * pBuf, const int intLen, vector<CStdCacheBlockQt> & vectData, vector<int> & vectIndex, bool & blnIsSnap, const unsigned short wVer) {
    CPackBuffer buf;
    buf.m_bSingleRead = true;
    if (pBuf[0] == 1) {
        vector<char> vectUncompressdData;
        if (UncompressZip2(vectUncompressdData, pBuf + 1, intLen - 1) == false) {
            return false;
        }

        if (vectUncompressdData.size() == 0) {
            return false;
        }

        buf.Write(&*vectUncompressdData.begin(), vectUncompressdData.size());
    }
    else {
        buf.Write(pBuf + 1, intLen - 1);
    }

    char chrFlag = 0;
    buf.Read(&chrFlag, sizeof (chrFlag));
    if (chrFlag) {
        if (vectData.size() == 0) {
            //CLcmException e;
            //e.m_wType = CLcmException::LCME_INCREMENT;
            //e.m_strMsg = "vectData.size() == 0";
            //throw e;
            return false;
        }

        UINT32 dwSize = 0;
        buf.Read(&dwSize, sizeof (dwSize));
        for (size_t i = 0; i < dwSize; i++) {
            unsigned char cMarket = 0;
            buf.Read(&cMarket, sizeof (cMarket));
            char pchCode[16];
            memset(pchCode, 0, sizeof (pchCode));
            char cLen = 0;
            buf.Read(&cLen, sizeof (cLen));
            buf.Read(pchCode, cLen);

            int intIndex = -1;
            for (size_t j = 0; j < vectData.size(); j++) {
                if (cMarket == vectData[j].m_wMarket && strncmp(pchCode, vectData[j].m_pchCode, sizeof (pchCode)) == 0) {
                    intIndex = j;
                    break;
                }
            }

            if (intIndex == -1) {
                //CLcmException e;
                //e.m_wType = CLcmException::LCME_INCREMENT;
                //char pTmp[256];
                //char pStock[16];
                //char cMarket;
                //CCovertMarket::ConvertMarketToChar(dwStockID, pStock, cMarket);
                //sprintf(pTmp, "Unknow stock '%s' vectData.size()=%d" , pStock, vectData.size());
                //e.m_strMsg = pTmp;
                //throw e;
                return false;
            }

            CStdCacheBlockQt rec = vectData[intIndex];
            //-----
            char cLen1 = 0;
            buf.Read(&cLen1, sizeof (cLen1));
            buf.Read(rec.m_pchTopStockCode, cLen1);
            //-----
            CBitStream stream(&buf, true);
            CMultiQtCompressUnit::ExpandAddData(stream, &rec, &vectData[intIndex], wVer);
            vectData[intIndex] = rec;
            vectIndex.push_back(intIndex);
        }

        blnIsSnap = false;
    }
    else {
        UINT32 dwSize = 0;
        buf.Read(&dwSize, sizeof (dwSize));
        vectData.clear();
        for (size_t i = 0; i < dwSize; i++) {
            CStdCacheBlockQt rec;
            char cLen = 0;
            unsigned char cMarket = 0;
            buf.Read(&cMarket, sizeof (cMarket));
            rec.m_wMarket = cMarket;
            buf.Read(&cLen, sizeof (cLen));
            buf.Read(rec.m_pchCode, cLen);
            //-----
            char cLen1 = 0;
            buf.Read(&cLen1, sizeof (cLen1));
            buf.Read(rec.m_pchTopStockCode, cLen1);
            //-----
            CBitStream stream(&buf, true);
            CMultiQtCompressUnit::ExpandSnapData(stream, &rec, wVer);
            vectData.push_back(rec);
            vectIndex.push_back(i);
        }

        blnIsSnap = true;
    }
    return true;
}

bool CMultiCompress::Uncompress(const char * pBuf, const int intLen, vector<CStdCacheBlockOverView> & vectData,
    vector<int> & vectIndex, bool & blnIsSnap, const unsigned short version) {
    if (version == MCM_COMPRESS_VERSION) {
        CPackBuffer buf;
        buf.m_bSingleRead = true;
        if (pBuf[0] == 1) {
            vector<char> vectUncompressdData;
            if (UncompressZip2(vectUncompressdData, pBuf + 1, intLen - 1) == false) {
                return false;
            }

            if (vectUncompressdData.size() == 0) {
                return false;
            }

            buf.Write(&*vectUncompressdData.begin(), vectUncompressdData.size());
        }
        else {
            buf.Write(pBuf + 1, intLen - 1);
        }

        char chrFlag = 0;
        buf.Read(&chrFlag, sizeof (chrFlag));
        if (chrFlag) {
            if (vectData.size() == 0) {
                //CLcmException e;
                //e.m_wType = CLcmException::LCME_INCREMENT;
                //e.m_strMsg = "vectData.size() == 0";
                //throw e;
                return false;
            }

            UINT32 dwSize = 0;
            buf.Read(&dwSize, sizeof (dwSize));
            for (size_t i = 0; i < dwSize; i++) {
                unsigned char cMarket = 0;
                buf.Read(&cMarket, sizeof (cMarket));
                char pchCode[16];
                memset(pchCode, 0, sizeof (pchCode));
                char cLen = 0;
                buf.Read(&cLen, sizeof (cLen));
                buf.Read(pchCode, cLen);

                int intIndex = -1;
                for (size_t j = 0; j < vectData.size(); j++) {
                    if (cMarket == vectData[j].m_wMarket && strncmp(pchCode, vectData[j].m_pchCode, sizeof (pchCode)) == 0) {
                        intIndex = j;
                        break;
                    }
                }

                if (intIndex == -1) {
                    //CLcmException e;
                    //e.m_wType = CLcmException::LCME_INCREMENT;
                    //char pTmp[256];
                    //char pStock[16];
                    //char cMarket;
                    //CCovertMarket::ConvertMarketToChar(dwStockID, pStock, cMarket);
                    //sprintf(pTmp, "Unknow stock '%s' vectData.size()=%d" , pStock, vectData.size());
                    //e.m_strMsg = pTmp;
                    //throw e;
                    return false;
                }

                CStdCacheBlockOverView rec = vectData[intIndex];
                //-----
                char cLen1 = 0;
                buf.Read(&cLen1, sizeof (cLen1));
                buf.Read(rec.m_pchTopStockCode, cLen1);
                //-----
                CBitStream stream(&buf, true);
                CMultiQtCompressUnit::ExpandAddData(stream, &rec, &vectData[intIndex], version);
                vectData[intIndex] = rec;
                vectIndex.push_back(intIndex);
            }

            blnIsSnap = false;
        }
        else {
            UINT32 dwSize = 0;
            buf.Read(&dwSize, sizeof (dwSize));
            vectData.clear();
            for (size_t i = 0; i < dwSize; i++) {
                CStdCacheBlockOverView rec;
                char cLen = 0;
                unsigned char cMarket = 0;
                buf.Read(&cMarket, sizeof (cMarket));
                rec.m_wMarket = cMarket;
                buf.Read(&cLen, sizeof (cLen));
                buf.Read(rec.m_pchCode, cLen);
                //-----
                char cLen1 = 0;
                buf.Read(&cLen1, sizeof (cLen1));
                buf.Read(rec.m_pchTopStockCode, cLen1);
                //-----
                CBitStream stream(&buf, true);
                CMultiQtCompressUnit::ExpandSnapData(stream, &rec, version);
                vectData.push_back(rec);
                vectIndex.push_back(i);
            }

            blnIsSnap = true;
        }
    } else if (version == MCM_VERSION) {
        CPackBuffer buf;
        buf.m_bSingleRead = true;
        if (pBuf[0] == 1) {
            std::vector<char> vectUncompressdData;
            if (!UncompressZip2(vectUncompressdData, pBuf + 1, intLen - 1))
                return false;

            if (vectUncompressdData.empty())
                return false;

            buf.Write(&*vectUncompressdData.begin(), vectUncompressdData.size());
        } else {
            buf.Write(pBuf + 1, intLen - 1);
        }

        char chrFlag = 0;
        buf.Read(&chrFlag, sizeof (chrFlag));
        if (chrFlag) {
            if (vectData.empty())
                return false;

            UINT32 dwSize = 0;
            buf.Read(&dwSize, sizeof (dwSize));
            for (size_t i = 0; i < dwSize; i++) {
                unsigned char marketID = 0;
                buf.Read(&marketID, sizeof(marketID));
                unsigned char topStockCodeMarketID = 0;
                buf.Read(&topStockCodeMarketID, sizeof(topStockCodeMarketID));

                char stockCode[16];
                char cLen = 0;
                buf.Read(&cLen, sizeof (cLen));
                buf.Read(stockCode, cLen);

                char topStockCode[32];
                char topStockCodeLength = 0;
                buf.Read(&topStockCodeLength, sizeof(topStockCodeLength));
                buf.Read(topStockCode, topStockCodeLength);
                int intIndex = -1;
                for (size_t j = 0; j < vectData.size(); j++) {
                    if (strncmp(stockCode, vectData[j].m_pchCode, cLen) == 0 &&
                        marketID == vectData[j].m_wMarket) {
                        intIndex = j;
                        break;
                    }
                }

                if (intIndex == -1)
                    return false;

                CStdCacheBlockOverView rec = vectData[intIndex];
                CBitStream stream(&buf, true);
                CMultiQtCompressUnit::ExpandData(stream, &rec, &vectData[intIndex], version);

                rec.m_pchTopStockMarketID = topStockCodeMarketID;
                strncpy(rec.m_pchTopStockCode, topStockCode, topStockCodeLength);

                vectData[intIndex] = rec;
                vectIndex.push_back(intIndex);
            }

            blnIsSnap = false;
        } else {
            UINT32 dwSize = 0;
            buf.Read(&dwSize, sizeof (dwSize));
            vectData.clear();
            for (size_t i = 0; i < dwSize; i++) {
                CStdCacheBlockOverView rec;
                // Just read one byte.
                buf.Read(&rec.m_wMarket, sizeof(unsigned char));
                buf.Read(&rec.m_pchTopStockMarketID, sizeof(rec.m_pchTopStockMarketID));
                char cLen = 0;
                buf.Read(&cLen, sizeof (cLen));
                buf.Read(rec.m_pchCode, cLen);

                char topStockCodeLength = 0;
                buf.Read(&topStockCodeLength, sizeof(topStockCodeLength));
                buf.Read(rec.m_pchTopStockCode, topStockCodeLength);
                CBitStream stream(&buf, true);
                CMultiQtCompressUnit::ExpandData(stream, &rec, (CStdCacheBlockOverView*)NULL, version);
                vectData.push_back(rec);
                vectIndex.push_back(i);
            }

            blnIsSnap = true;
        }
    }
    return true;
}

bool
CMultiCompress::Uncompress(const char* pBuf, const int intLen, CStdCacheQtEx& data, const unsigned short wVer) {
    return DoUncompress(pBuf, intLen, data, wVer);
}

bool
CMultiCompress::Uncompress(const char* pBuf, const int intLen, CStdCacheCAS& data, const unsigned short wVer) {
    return DoUncompress(pBuf, intLen, data, wVer);
}

bool
CMultiCompress::Uncompress(const char* pBuf, const int intLen, CStdCacheVCM& data, const unsigned short wVer) {
    return DoUncompress(pBuf, intLen, data, wVer);
}

void
CMultiCompress::Compress(const CStdCacheDayFF* newData, const CStdCacheDayFF* oldData,\
    OutputBuffer& buffer, const unsigned short version) {
    if (version == MCM_VERSION) {
        if (oldData)
            buffer.append<char>(COMPRESS_TYPE_ADDED);
        else
            buffer.append<char>(COMPRESS_TYPE_SNAP);

        COutputBitStream stream(buffer);
        CMultiQtCompressUnit::CompressData(stream, newData, oldData, version);
    }
}

void
CMultiCompress::Compress(const CStdCacheMarketTurnover* newData, const CStdCacheMarketTurnover* oldData,\
    OutputBuffer& buffer, const unsigned short version) {
    if (version == MCM_VERSION) {
        if (oldData)
            buffer.append<char>(COMPRESS_TYPE_ADDED);
        else
            buffer.append<char>(COMPRESS_TYPE_SNAP);

        COutputBitStream stream(buffer);
        CMultiQtCompressUnit::CompressData(stream, newData, oldData, version);
    }
}

bool
CMultiCompress::Uncompress(const char* charBuffer, const int bufferLength, CStdCacheDayFF& data, const unsigned short version) {
    if (version == MCM_VERSION) {
        CPackBuffer buffer;
        buffer.m_bSingleRead = true;
        buffer.Write(charBuffer, bufferLength);
        char isCompressTypeAdded = 0;
        buffer.Read(&isCompressTypeAdded, 1);
        if (isCompressTypeAdded) {
            CBitStream stream(&buffer, true);
            CStdCacheDayFF oldData = data;
            CMultiQtCompressUnit::ExpandData(stream, &data, &oldData, version);
        }
        else {
            CBitStream stream(&buffer, true);
            CMultiQtCompressUnit::ExpandData(stream, &data, (CStdCacheDayFF*) NULL, version);
        }
    }
    return true;
}

bool
CMultiCompress::Uncompress(const char* charBuffer, const int bufferLength, CStdCacheMarketTurnover& data, const unsigned short version) {
    if (version == MCM_VERSION) {
        CPackBuffer buffer;
        buffer.m_bSingleRead = true;
        buffer.Write(charBuffer, bufferLength);
        char isCompressTypeAdded = 0;
        buffer.Read(&isCompressTypeAdded, 1);
        if (isCompressTypeAdded) {
            CBitStream stream(&buffer, true);
            CStdCacheMarketTurnover oldData = data;
            CMultiQtCompressUnit::ExpandData(stream, &data, &oldData, version);
        }
        else {
            CBitStream stream(&buffer, true);
            CMultiQtCompressUnit::ExpandData(stream, &data, (CStdCacheMarketTurnover*) NULL, version);
        }
    }
    return true;
}


