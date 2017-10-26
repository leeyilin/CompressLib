/*
 * File:   MultiCompressUnit.h
 *
 * Author: liyilin
 *
 * Created on 08.29.2016, 10:14 AM
 */

#ifndef MULTICOMPRESSUNIT_H
#define	MULTICOMPRESSUNIT_H

#pragma once
#include <vector>
#include "stdstructdef.h"
#include "BitStream.h"
#include "QtTypeDef.h"
#include "compact_memory.h"

#define MCM_DEFAULT_OLD_VERSION 1
#define MCM_VERSION       161

/* In the modified version MCM_VERSION, I include a new variable wVer in the parameter list of some functions
 * in order to unifiy the format of all the functions.
 * In this way, we can modify functions more conveniently in the future.
 */
class CMultiKlineCompressUnit {
public:
    static int CompressData(COutputBitStream& stream, const CStdCacheKline* pData, UINT32 size, const UINT16 wVer);
    static int ExpandData(CBitStream& stream, vector<CStdCacheKline> & vectData, const UINT16 wVer);
};

class CMultiRtminCompressUnit {
public:
    static int CompressData(COutputBitStream& stream, const CStdCacheRtMin* pMinute, UINT32 size, const UINT16 wVer);
    static int ExpandData(CBitStream& stream, vector<CStdCacheRtMin> & aMinute, const UINT16 wVer);
};

class CMultiBQCompressUnit {
public:
    static int CompressData(COutputBitStream& stream, const CStdCacheBrokerQueue* pNew, const UINT16 wVer);
    static int ExpandData(CBitStream& stream, CStdCacheBrokerQueue* pNew, const UINT16 wVer);
    // For code unification and for compression efficiency, I include the following functions. The two functions above are very inefficient.
    // Reserve them just for backward compatibility.
    static int CompressData(COutputBitStream& stream, const CStdCacheBrokerQueue* pNew, const CStdCacheBrokerQueue* pOld, const UINT16 wVer);
    static int ExpandData(CBitStream& stream, CStdCacheBrokerQueue* pNew, const CStdCacheBrokerQueue* pOld, const UINT16 wVer);
private:
    static int CompressSnapData(COutputBitStream& stream, const CStdCacheBrokerQueue* pNew, const UINT16 wVer);
    static int ExpandSnapData(CBitStream& stream, CStdCacheBrokerQueue* pNew, const UINT16 wVer);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheBrokerQueue* pNew, const CStdCacheBrokerQueue* pOld, const UINT16 wVer);
    static int ExpandAddData(CBitStream& stream, CStdCacheBrokerQueue* pNew, const CStdCacheBrokerQueue* pOld, const UINT16 wVer);
};

class CMultiQtCompressUnit {
public:
    template <typename T>
    static int CompressData(COutputBitStream& stream, const T *pNew, const T *pOld, const UINT16 wVer);
    template <typename T>
    static int ExpandData(CBitStream& stream, T *pNew, const T *pOld, const UINT16 wVer);
    // For data protection and for code unification, the following four functions should have been defined private.
    // However, to ensure compatibility, we make them public here.

    // The struct Qt and struct QtS are similar, so they share the same compression functions.
    template <typename T>
    static int CompressSnapData(COutputBitStream& stream, const T *pNew, const UINT16 wVer);
    template <typename T>
    static int ExpandSnapData(CBitStream& stream, T *pNew, const UINT16 wVer);
    template <typename T>
    static int CompressAddData(COutputBitStream& stream, const T *pNew, const T *pOld, const UINT16 wVer);
    template <typename T>
    static int ExpandAddData(CBitStream& stream, T *pNew, const T *pOld, const UINT16 wVer);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheBlockQt* pNew, const UINT16 wVer);
    static int ExpandSnapData(CBitStream& stream, CStdCacheBlockQt* pNew, const UINT16 wVer);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheBlockQt* pNew, const CStdCacheBlockQt* pOld, const UINT16 wVer);
    static int ExpandAddData(CBitStream& stream, CStdCacheBlockQt* pNew, const CStdCacheBlockQt* pOld, const UINT16 wVer);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheBlockOverView* pNew, const UINT16 wVer);
    static int ExpandSnapData(CBitStream& stream, CStdCacheBlockOverView* pNew, const UINT16 wVer);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheBlockOverView* pNew, const CStdCacheBlockOverView* pOld, const UINT16 wVer);
    static int ExpandAddData(CBitStream& stream, CStdCacheBlockOverView* pNew, const CStdCacheBlockOverView* pOld, const UINT16 wVer);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheBrokerTrace* pNew, const UINT16 wVer);
    static int ExpandSnapData(CBitStream& stream, CStdCacheBrokerTrace* pNew, const UINT16 wVer);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheBrokerTrace* pNew, const CStdCacheBrokerTrace* pOld, const UINT16 wVer);
    static int ExpandAddData(CBitStream& stream, CStdCacheBrokerTrace* pNew, const CStdCacheBrokerTrace* pOld, const UINT16 wVer);
private:
    template <typename T>
    static void CompressMMP(COutputBitStream& stream, const T *pDyna, const T *pOldDyna, const UINT16 wVer = 1);
    template <typename T>
    static void CompressMMP(COutputBitStream& stream, const T *pData, const UINT16 wVer = 1);
    template <typename T>
    static void ExpandMMP(CBitStream& stream, T *pDyna, const T *pOldDyna, const UINT16 wVer = 1);
    template <typename T>
    static void ExpandMMP(CBitStream& stream, T *pDyna, const UINT16 wVer = 1);

    template <typename T>
    static int EncodeAllMMPPrice(COutputBitStream& stream, int NUM_OF_MMP, int nNumBuy, int nNumSell, const UINT32 * adwMMPPrice, UINT32 dwPrice, const UINT16 wVer = 1);
    template <typename T>
    static int DecodeAllMMPPrice(CBitStream& stream, int NUM_OF_MMP, int nNumBuy, int nNumSell, PDWORD adwMMPPrice, UINT32 dwPrice, const UINT16 wVer = 1);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheQtEx* pNew, const UINT16 wVer);
    static int ExpandSnapData(CBitStream& stream, CStdCacheQtEx* pNew, const UINT16 wVer);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheQtEx* pNew, const CStdCacheQtEx* pOld, const UINT16 wVer);
    static int ExpandAddData(CBitStream& stream, CStdCacheQtEx* pNew, const CStdCacheQtEx* pOld, const UINT16 wVer);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheCAS* pNew, const UINT16 wVer);
    static int ExpandSnapData(CBitStream& stream, CStdCacheCAS* pNew, const UINT16 wVer);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheCAS* pNew, const CStdCacheCAS* pOld, const UINT16 wVer);
    static int ExpandAddData(CBitStream& stream, CStdCacheCAS* pNew, const CStdCacheCAS* pOld, const UINT16 wVer);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheVCM* pNew, const UINT16 wVer);
    static int ExpandSnapData(CBitStream& stream, CStdCacheVCM* pNew, const UINT16 wVer);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheVCM* pNew, const CStdCacheVCM* pOld, const UINT16 wVer);
    static int ExpandAddData(CBitStream& stream, CStdCacheVCM* pNew, const CStdCacheVCM* pOld, const UINT16 wVer);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheFFDeal* ptrNewData, const unsigned short version);
    static int ExpandSnapData(CBitStream& stream, CStdCacheFFDeal* ptrNewData, const unsigned short version);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheFFDeal* ptrNewData, const CStdCacheFFDeal* ptrOldData,\
        const unsigned short version);
    static int ExpandAddData(CBitStream& stream, CStdCacheFFDeal* ptrNewData, const CStdCacheFFDeal* ptrOldData,\
        const unsigned short version);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheFF* ptrNewData, const unsigned short version);
    static int ExpandSnapData(CBitStream& stream, CStdCacheFF* ptrNewData, const unsigned short version);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheFF* ptrNewData, const CStdCacheFF* ptrOldData,\
        const unsigned short version);
    static int ExpandAddData(CBitStream& stream, CStdCacheFF* ptrNewData, const CStdCacheFF* ptrOldData,\
        const unsigned short version);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheMinuteFF* ptrNewData, const unsigned short version);
    static int ExpandSnapData(CBitStream& stream, CStdCacheMinuteFF* ptrNewData, const unsigned short version);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheMinuteFF* ptrNewData, const CStdCacheMinuteFF* ptrOldData,\
        const unsigned short version);
    static int ExpandAddData(CBitStream& stream, CStdCacheMinuteFF* ptrNewData, const CStdCacheMinuteFF* ptrOldData,\
        const unsigned short version);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheFFTrend* ptrNewData, const unsigned short version);
    static int ExpandSnapData(CBitStream& stream, CStdCacheFFTrend* ptrNewData, const unsigned short version);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheFFTrend* ptrNewData, const CStdCacheFFTrend* ptrOldData,\
        const unsigned short version);
    static int ExpandAddData(CBitStream& stream, CStdCacheFFTrend* ptrNewData, const CStdCacheFFTrend* ptrOldData,\
        const unsigned short version);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheFFBriefInfo* ptrNewData, const unsigned short version);
    static int ExpandSnapData(CBitStream& stream, CStdCacheFFBriefInfo* ptrNewData, const unsigned short version );
    static int CompressAddData(COutputBitStream& stream, const CStdCacheFFBriefInfo* ptrNewData, const CStdCacheFFBriefInfo* ptrOldData,\
        const unsigned short version);
    static int ExpandAddData(CBitStream& stream, CStdCacheFFBriefInfo* ptrNewData, const CStdCacheFFBriefInfo* ptrOldData,\
        const unsigned short version);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheDayFF* ptrNewData, const unsigned short version);
    static int ExpandSnapData(CBitStream& stream, CStdCacheDayFF* ptrNewData, const unsigned short version);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheDayFF* ptrNewData, const CStdCacheDayFF* ptrOldData,\
        const unsigned short version);
    static int ExpandAddData(CBitStream& stream, CStdCacheDayFF* ptrNewData, const CStdCacheDayFF* ptrOldData,\
        const unsigned short version);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheListFFItem* ptrNewData, const unsigned short version);
    static int ExpandSnapData(CBitStream& stream, CStdCacheListFFItem* ptrNewData, const unsigned short version);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheListFFItem* ptrNewData, const CStdCacheListFFItem* ptrOldData,\
        const unsigned short version);
    static int ExpandAddData(CBitStream& stream, CStdCacheListFFItem* ptrNewData, const CStdCacheListFFItem* ptrOldData,\
        const unsigned short version);

    static int CompressSnapData(COutputBitStream& stream, const CStdCacheListBuyinRank* ptrNewData, const unsigned short version);
    static int ExpandSnapData(CBitStream& stream, CStdCacheListBuyinRank* ptrNewData, const unsigned short version);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheListBuyinRank* ptrNewData, const CStdCacheListBuyinRank* ptrOldData,\
        const unsigned short version);
    static int ExpandAddData(CBitStream& stream, CStdCacheListBuyinRank* ptrNewData, const CStdCacheListBuyinRank* ptrOldData,\
        const unsigned short version);
    
    static int CompressSnapData(COutputBitStream& stream, const CStdCacheMarketTurnover* ptrNewData, const unsigned short version);
    static int ExpandSnapData(CBitStream& stream, CStdCacheMarketTurnover* ptrNewData, const unsigned short version);
    static int CompressAddData(COutputBitStream& stream, const CStdCacheMarketTurnover* ptrNewData, const CStdCacheMarketTurnover* ptrOldData,\
        const unsigned short version);
    static int ExpandAddData(CBitStream& stream, CStdCacheMarketTurnover* ptrNewData, const CStdCacheMarketTurnover* ptrOldData,\
        const unsigned short version);

};

class CMultiMxCompressUnit {
public:
    template <typename Container>
    static int CompressData(COutputBitStream& stream,
                            const Container &c,
                            size_t offset,
                            size_t size,
                            UINT16 type);

    static int CompressData(COutputBitStream& stream, const CStdCacheMx* pMinute, UINT32 size, const unsigned short type);
    static int ExpandData(CBitStream& stream, std::vector<CStdCacheMx> & aMinute, const unsigned short type);
private:
    static void CompressNum(COutputBitStream&, UINT32 dwNum, const UINT16 wVer = 1);
    static UINT32 ExpandNum(CBitStream& stream, const UINT16 wVer = 1);
};

template <typename T>
int CMultiQtCompressUnit::CompressData(COutputBitStream& stream, const T *pNew, const T *pOld, const UINT16 wVer) {
    if (!pOld) {
        stream.WriteBool(1);
        return CompressSnapData(stream, pNew, wVer);
    } else {
        stream.WriteBool(0);
        return CompressAddData(stream, pNew, pOld, wVer);
    }
}

template <typename T>
int CMultiQtCompressUnit::ExpandData(CBitStream& stream, T *pNew, const T *pOld, const UINT16 wVer) {
	bool isSnapped = (stream.ReadDWORD(1) !=0);
	if (isSnapped) {
		return ExpandSnapData(stream, pNew, wVer);
	} else {
		return ExpandAddData(stream, pNew, pOld, wVer);
	}
}

#endif	/* MULTICOMPRESSUNIT_H */
