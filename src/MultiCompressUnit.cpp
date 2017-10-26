// Implementation of the above functions
#include <iostream>
#include <typeinfo>
#include <ctime>
#include <deque>
#include "MultiCompressIf.h"
#include "comdatetime.h"
#include "MatchCode.h"
#include "MultiCompressUnit.h"

using namespace BackCom;
using namespace MCM;

#define MCM_NEW_VERSION1 160

#define CTime CComDateTime
#define CTimeSpan CComDateTimeSpan

const int DAY_BEGIN_YEAR = 1980;
const WORD MINUTE_BEGIN_TIME = 0;

int CMultiKlineCompressUnit::CompressData(COutputBitStream& stream, const CStdCacheKline* pData, UINT32 size, const UINT16 wVer)
{
	UINT32 dwCount = size;
	try
	{
		stream.EncodeData(dwCount, BC_DAY_NUMBER, BCNUM_DAY_NUMBER);
		BYTE cCheckSum = 0;
		UINT32 dwLastClose = 0;
		UINT32 dwLastAve = 0;
		UINT32 dwLastSellPrice = 0;
		UINT32 dwLastBuyPrice = 0;
		UINT32 wLastTime = 0;
		for (size_t i = 0; i < dwCount; ++i) {
			// In case of weird situations.
			if (pData == NULL)
				break;

			const CStdCacheKline* ptrData = pData + i;
			if (i == 0) {
				stream.WriteDWORD(ptrData->m_wTime, 26);
			}
			else {
				CTime tmBase(wLastTime / 10000, wLastTime % 10000 / 100, wLastTime % 100, 0, 0, 0);
				CTime tm(ptrData->m_wTime / 10000, ptrData->m_wTime % 10000 / 100, ptrData->m_wTime % 100, 0, 0, 0);
				UINT32 dwCurDay = (tm - tmBase).GetDays();
				stream.EncodeData(dwCurDay, BC_DAY_TIME, BCNUM_DAY_TIME);
			}


			wLastTime = ptrData->m_wTime;
			// m_dwOpen, m_dwHigh, m_dwLow, m_dwClose, m_dwAve
			UINT32 dwOpen = ptrData->m_dwOpen;
			UINT32 dwHigh = ptrData->m_dwHigh;
			UINT32 dwLow = ptrData->m_dwLow;
			UINT32 dwClose = ptrData->m_dwClose;
			if (i == 0) {
				stream.EncodeData(dwOpen, BC_MINDAY_PRICE, BCNUM_MINDAY_PRICE);
			}
			else {
				stream.EncodeData(dwOpen, BC_MINDAY_PRICE_DIFFS, BCNUM_MINDAY_PRICE_DIFFS, &dwLastClose);
			}

			stream.EncodeData(dwHigh, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwOpen);
			stream.EncodeData(dwLow, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwOpen, true);
			stream.EncodeData(dwClose, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwLow);
			dwLastClose = dwClose;
			if (i == 0) {
				stream.EncodeData(ptrData->m_dwAve, BC_MINDAY_PRICE, BCNUM_MINDAY_PRICE);
				dwLastAve = ptrData->m_dwAve;
			}
			else {
				stream.EncodeData(ptrData->m_dwAve, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwLastAve);
				dwLastAve = ptrData->m_dwAve;
			}

			stream.EncodeBigInt(ptrData->m_xVolume, BC_MIN_VOL, BCNUM_MIN_VOL);
			EncodeBigintSnap(ptrData->m_xAmount, 0, BC_MINDAY_AMNT, BCNUM_MINDAY_AMNT, stream);
			EncodeBigintSnap(ptrData->m_xTradeNum, 0, BC_DAY_TRADENUM, BCNUM_DAY_TRADENUM, stream);
			stream.EncodeBigInt(ptrData->m_xWaiPan, BC_MIN_VOL, BCNUM_MIN_VOL);
			EncodeBigintSnap(ptrData->m_xExt1, 0, BC_DAY_VOL, BCNUM_DAY_VOL, stream);
			EncodeBigintSnap(ptrData->m_xExt2, 0, BC_DYNA_QH_OI_DIFF, BCNUM_DYNA_QH_OI_DIFF, stream);
			EncodeDataSnap(ptrData->m_dwExt1, 0, BC_MINDAY_PRICE, BCNUM_MINDAY_PRICE, stream);
			//cCheckSum += ptrData.GetCheckSum();
		}
		stream.WriteDWORD(cCheckSum, 8);
	}
	catch (int)
	{
		return ERR_COMPRESS_BITBUF;
	}

	return 0;
}


int CMultiKlineCompressUnit::ExpandData(CBitStream& stream, vector<CStdCacheKline> & vectData, const UINT16 wVer)
{
	try
	{
		UINT32 dwCount;
		stream.DecodeData(dwCount, BC_DAY_NUMBER, BCNUM_DAY_NUMBER);
		BYTE cCheckSum = 0;
		UINT32 dwLastClose = 0;
		UINT32 dwLastAve = 0;
		UINT32 dwLastSellPrice = 0;
		UINT32 dwLastBuyPrice = 0;
		UINT32 wLastTime = 0;
		if (dwCount > 0) {
			for (size_t i = 0; i < dwCount; i++) {
				CStdCacheKline data;
				if (i == 0) {
					data.m_wTime = stream.ReadDWORD(26);
				}
				else {
					CTime tmBase(wLastTime / 10000, wLastTime % 10000 / 100, wLastTime % 100, 0, 0, 0);
					UINT32 dwCurDay = 0;
					stream.DecodeData(dwCurDay, BC_DAY_TIME, BCNUM_DAY_TIME);
					CTime tm = tmBase + CTimeSpan(dwCurDay, 0, 0, 0);
					data.m_wTime = tm.GetYear() * 10000 + tm.GetMonth() * 100 + tm.GetDay();
				}

				wLastTime = data.m_wTime;
				UINT32 dwOpen = 0;
				UINT32 dwHigh = 0;
				UINT32 dwLow = 0;
				UINT32 dwClose = 0;
				if (i == 0) {
					stream.DecodeData(dwOpen, BC_MINDAY_PRICE, BCNUM_MINDAY_PRICE);
				}
				else {
					stream.DecodeData(dwOpen, BC_MINDAY_PRICE_DIFFS, BCNUM_MINDAY_PRICE_DIFFS, &dwLastClose);
				}
				stream.DecodeData(dwHigh, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwOpen);
				stream.DecodeData(dwLow, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwOpen, true);
				stream.DecodeData(dwClose, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwLow);
				dwLastClose = dwClose;
				data.m_dwOpen = dwOpen;
				data.m_dwHigh = dwHigh;
				data.m_dwLow = dwLow;
				data.m_dwClose = dwClose;
				if (i == 0) {
					stream.DecodeData(data.m_dwAve, BC_MINDAY_PRICE, BCNUM_MINDAY_PRICE);
					dwLastAve = data.m_dwAve;
				}
				else {
					stream.DecodeData(data.m_dwAve, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwLastAve);
					dwLastAve = data.m_dwAve;
				}
				stream.DecodeBigInt(data.m_xVolume, BC_MIN_VOL, BCNUM_MIN_VOL);
				DecodeBigintSnap(data.m_xAmount, 0, BC_MINDAY_AMNT, BCNUM_MINDAY_AMNT, stream);
				DecodeBigintSnap(data.m_xTradeNum, 0, BC_DAY_TRADENUM, BCNUM_DAY_TRADENUM, stream);
				stream.DecodeBigInt(data.m_xWaiPan, BC_MIN_VOL, BCNUM_MIN_VOL);
				DecodeBigintSnap(data.m_xExt1, 0, BC_DAY_VOL, BCNUM_DAY_VOL, stream);
				DecodeBigintSnap(data.m_xExt2, 0, BC_DYNA_QH_OI_DIFF, BCNUM_DYNA_QH_OI_DIFF, stream);
				DecodeDataSnap(data.m_dwExt1, 0, BC_MINDAY_PRICE, BCNUM_MINDAY_PRICE, stream);
				//cCheckSum += data.GetCheckSum();
				vectData.push_back(data);
			}
		}
		BYTE cStreamChkSum = (BYTE)stream.ReadDWORD(8);
		if (cStreamChkSum != cCheckSum)
			return ERR_COMPRESS_CHECK;
	}
	catch (int)
	{
		return ERR_COMPRESS_BITBUF;
	}

	return 0;
}

int CMultiRtminCompressUnit::CompressData(COutputBitStream& stream, const CStdCacheRtMin* pMinute, UINT32 size, const UINT16 wVer)
{
	WORD wMinuteNum = (WORD)size;
	try {
		stream.EncodeData(wMinuteNum, BC_MIN_NUMBER, BCNUM_MIN_NUMBER);
		WORD wSize = wMinuteNum;
		WORD wLastMinute = MINUTE_BEGIN_TIME;
		UINT32 dwLastDay = 0;
		CComDateTime tmBase(DAY_BEGIN_YEAR, 1, 1, 0, 0, 0);                     // long long ago..
		BYTE cCheckSum = 0;
		UINT32 dwLastClose = 0;
		UINT32 dwLastAve = 0;

		for (WORD w = 0; w < wSize; ++w) {
			// In case of weird situations.
			if (pMinute == NULL)
				break;

			const CStdCacheRtMin*  pmin = pMinute + w;
			UINT32 dwTimeDay = 2000 * 10000 + pmin->m_wTime / 10000;		// yyyymmdd
			WORD wTimeMin = pmin->m_wTime % 10000;					// hhmm
			CComDateTime tm(dwTimeDay / 10000, dwTimeDay % 10000 / 100, dwTimeDay % 100, 0, 0, 0);
			UINT32 dwCurDay = (tm - tmBase).GetDays();
			stream.EncodeData(dwCurDay, BC_DAY_TIME_MIND, BCNUM_DAY_TIME_MIND, &dwLastDay);
			if (dwCurDay != dwLastDay) {
				dwLastDay = dwCurDay;
				wLastMinute = MINUTE_BEGIN_TIME;
			}

			WORD wMinutes = GetMinutesDiff(wLastMinute, wTimeMin);
			stream.EncodeData(wMinutes, BC_MIN_TIME, BCNUM_MIN_TIME);
			wLastMinute = wTimeMin;

			// m_dwOpen, m_dwHigh, m_dwLow, m_dwClose, m_dwAve
			UINT32 dwOpen = pmin->m_dwOpen;
			UINT32 dwHigh = pmin->m_dwHigh;
			UINT32 dwLow = pmin->m_dwLow;
			UINT32 dwClose = pmin->m_dwClose;
			if (w == 0) {
				stream.EncodeData(dwOpen, BC_MINDAY_PRICE, BCNUM_MINDAY_PRICE);
			}
			else {
				stream.EncodeData(dwOpen, BC_MINDAY_PRICE_DIFFS, BCNUM_MINDAY_PRICE_DIFFS, &dwLastClose);
			}

			stream.EncodeData(dwHigh, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwOpen);
			stream.EncodeData(dwLow, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwOpen, true);
			stream.EncodeData(dwClose, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwLow);
			dwLastClose = dwClose;

			if (w == 0) {
				stream.EncodeData(pmin->m_dwAve, BC_MINDAY_PRICE, BCNUM_MINDAY_PRICE);
				dwLastAve = pmin->m_dwAve;
			}
			else {
				stream.EncodeData(pmin->m_dwAve, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwLastAve);
				dwLastAve = pmin->m_dwAve;
			}

			stream.EncodeBigInt(pmin->m_xVolume, BC_MIN_VOL, BCNUM_MIN_VOL);
			EncodeBigintSnap(pmin->m_xAmount, 0, BC_MINDAY_AMNT, BCNUM_MINDAY_AMNT, stream);
			EncodeBigintSnap(pmin->m_xTradeNum, 0, BC_DAY_TRADENUM, BCNUM_DAY_TRADENUM, stream);
			stream.EncodeBigInt(pmin->m_xWaiPan, BC_MIN_VOL, BCNUM_MIN_VOL);
			EncodeBigintSnap(pmin->m_xExt1, 0, BC_DAY_VOL, BCNUM_DAY_VOL, stream);
			EncodeBigintSnap(pmin->m_xExt2, 0, BC_DYNA_QH_OI_DIFF, BCNUM_DYNA_QH_OI_DIFF, stream);
			EncodeDataSnap(pmin->m_dwExt1, 0, BC_MINDAY_PRICE, BCNUM_MINDAY_PRICE, stream);
			//cCheckSum += pmin->GetCheckSum();
		}
		stream.WriteDWORD(cCheckSum, 8);

	}
	catch (int) {
		return ERR_COMPRESS_BITBUF;
	}

	return 0;
}
int CMultiRtminCompressUnit::ExpandData(CBitStream& stream, vector<CStdCacheRtMin> & aMinute, const UINT16 wVer)
{
	try
	{
		UINT32 dwMinuteNum;
		stream.DecodeData(dwMinuteNum, BC_MIN_NUMBER, BCNUM_MIN_NUMBER);
		WORD wMinuteNum = (WORD)dwMinuteNum;
		CComDateTime tmBase(DAY_BEGIN_YEAR, 1, 1, 0, 0, 0);
		UINT32 dwLastDay = 0;
		WORD  wLastMinute = MINUTE_BEGIN_TIME;
		BYTE cCheckSum = 0;
		UINT32 dwLastClose = 0;
		UINT32 dwLastAve = 0;
		if (wMinuteNum > 0)
		{
			WORD wLastMinute = MINUTE_BEGIN_TIME;

			for (WORD w = 0; w < wMinuteNum; w++)
			{
				CStdCacheRtMin min;
				UINT32 dwCurDay = 0;
				stream.DecodeData(dwCurDay, BC_DAY_TIME_MIND, BCNUM_DAY_TIME_MIND, &dwLastDay);
				CComDateTime tm = tmBase + CComDateTimeSpan(dwCurDay, 0, 0, 0);
				UINT32 dwTimeDay = tm.GetYear() * 10000 + tm.GetMonth() * 100 + tm.GetDay();

				if (dwCurDay != dwLastDay)
				{
					dwLastDay = dwCurDay;
					wLastMinute = MINUTE_BEGIN_TIME;
				}

				UINT32 dwTemp = 0;
				stream.DecodeData(dwTemp, BC_MIN_TIME, BCNUM_MIN_TIME);
				WORD wMinutes = (WORD)dwTemp;
				WORD wTimeMin = AddMinutes(wLastMinute, wMinutes);
				min.m_wTime = (dwTimeDay - 2000 * 10000) * 10000 + wTimeMin;
				wLastMinute = wTimeMin;

				UINT32 dwOpen = 0;
				UINT32 dwHigh = 0;
				UINT32 dwLow = 0;
				UINT32 dwClose = 0;

				if (w == 0)
				{
					stream.DecodeData(dwOpen, BC_MINDAY_PRICE, BCNUM_MINDAY_PRICE);
				}
				else
				{
					stream.DecodeData(dwOpen, BC_MINDAY_PRICE_DIFFS, BCNUM_MINDAY_PRICE_DIFFS, &dwLastClose);
				}

				stream.DecodeData(dwHigh, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwOpen);
				stream.DecodeData(dwLow, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwOpen, true);
				stream.DecodeData(dwClose, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwLow);

				dwLastClose = dwClose;
				min.m_dwOpen = dwOpen;
				min.m_dwHigh = dwHigh;
				min.m_dwLow = dwLow;
				min.m_dwClose = dwClose;

				if (w == 0)
				{
					stream.DecodeData(min.m_dwAve, BC_MINDAY_PRICE, BCNUM_MINDAY_PRICE);
					dwLastAve = min.m_dwAve;
				}
				else
				{
					stream.DecodeData(min.m_dwAve, BC_MINDAY_PRICE_DIFF, BCNUM_MINDAY_PRICE_DIFF, &dwLastAve);
					dwLastAve = min.m_dwAve;
				}

				stream.DecodeBigInt(min.m_xVolume, BC_MIN_VOL, BCNUM_MIN_VOL);
				DecodeBigintSnap(min.m_xAmount, 0, BC_MINDAY_AMNT, BCNUM_MINDAY_AMNT, stream);
				DecodeBigintSnap(min.m_xTradeNum, 0, BC_DAY_TRADENUM, BCNUM_DAY_TRADENUM, stream);

				stream.DecodeBigInt(min.m_xWaiPan, BC_MIN_VOL, BCNUM_MIN_VOL);

				DecodeBigintSnap(min.m_xExt1, 0, BC_DAY_VOL, BCNUM_DAY_VOL, stream);
				DecodeBigintSnap(min.m_xExt2, 0, BC_DYNA_QH_OI_DIFF, BCNUM_DYNA_QH_OI_DIFF, stream);
				DecodeDataSnap(min.m_dwExt1, 0, BC_MINDAY_PRICE, BCNUM_MINDAY_PRICE, stream);

				//cCheckSum += min.GetCheckSum();
				aMinute.push_back(min);
			}
		}

		BYTE cStreamChkSum = (BYTE)stream.ReadDWORD(8);
		if (cStreamChkSum != cCheckSum)
			return ERR_COMPRESS_CHECK;
	}
	catch (int)
	{
		return ERR_COMPRESS_BITBUF;
	}

	return 0;
}

int CMultiBQCompressUnit::CompressData(COutputBitStream& stream, const CStdCacheBrokerQueue* pNew, const UINT16 wVer)
{
	int nFlagB = pNew->m_pcBuyBQMoreFlag;
	stream.WriteDWORD(nFlagB, 8);
	int nFlagS = pNew->m_pcSellBQMoreFlag;
	stream.WriteDWORD(nFlagS, 8);
	int nNumB = pNew->m_btBuyItemCount;
	stream.WriteDWORD(nNumB, 6);
	int nNumS = pNew->m_btSellItemCount;
	stream.WriteDWORD(nNumS, 6);
	for (int i = 0; i < pNew->m_btBuyItemCount; i++)
	{
		int nType = pNew->m_stItemBuy[i].m_pcBSType;
		stream.WriteDWORD(nType, 8);
		int nBrokerNo = pNew->m_stItemBuy[i].m_wBSBrokerNo;
		stream.WriteDWORD(nBrokerNo, 14);
	}

	for (int k = 0; k < pNew->m_btSellItemCount; k++)
	{
		int nType = pNew->m_stItemSell[k].m_pcBSType;
		stream.WriteDWORD(nType, 8);
		int nBrokerNo = pNew->m_stItemSell[k].m_wBSBrokerNo;
		stream.WriteDWORD(nBrokerNo, 14);
	}
	return 0;
}
int CMultiBQCompressUnit::CompressData(COutputBitStream& stream, const CStdCacheBrokerQueue* pNew, const CStdCacheBrokerQueue* pOld, const UINT16 wVer)
{
	if (pOld == NULL) {
		stream.WriteBool(1);
		return CMultiBQCompressUnit::CompressSnapData(stream, pNew, wVer);
	}
	else {
		stream.WriteBool(0);
		return CMultiBQCompressUnit::CompressAddData(stream, pNew, pOld, wVer);
	}
}
int CMultiBQCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheBrokerQueue* pNew, const UINT16 wVer)
{
	if (pNew->m_pcBuyBQMoreFlag == 'Y') {
		stream.WriteBool(true);
	}
	else {
		stream.WriteBool(false);
	}

	if (pNew->m_pcSellBQMoreFlag == 'Y') {
		stream.WriteBool(true);
	}
	else {
		stream.WriteBool(false);
	}
	stream.WriteDWORD(pNew->m_btBuyItemCount, 6); // 0-40
	stream.WriteDWORD(pNew->m_btSellItemCount, 6); // 0-40

	for (int i = 0; i < 40; i++) {
		if (pNew->m_stItemBuy[i].m_pcBSType == 'B') {
			stream.WriteBool(true);
		}
		else {
			stream.WriteBool(false);
		}
		stream.WriteDWORD(pNew->m_stItemBuy[i].m_wBSBrokerNo, 14); // max 9999
	}

	for (int i = 0; i < 40; i++) {
		if (pNew->m_stItemSell[i].m_pcBSType == 'B') {
			stream.WriteBool(true);
		}
		else {
			stream.WriteBool(false);
		}
		stream.WriteDWORD(pNew->m_stItemSell[i].m_wBSBrokerNo, 14); // max 9999
	}
	return 0;
}
int CMultiBQCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheBrokerQueue* pNew, const CStdCacheBrokerQueue* pOld, const UINT16 wVer)
{
	if (pNew->m_pcBuyBQMoreFlag == 'Y') {
		stream.WriteBool(true);
	}
	else {
		stream.WriteBool(false);
	}

	if (pNew->m_pcSellBQMoreFlag == 'Y') {
		stream.WriteBool(true);
	}
	else {
		stream.WriteBool(false);
	}
	stream.WriteDWORD(pNew->m_btBuyItemCount, 6); // 0-40
	stream.WriteDWORD(pNew->m_btSellItemCount, 6); // 0-40

	for (int i = 0; i < 40; ++i) {
		if (pNew->m_stItemBuy[i].m_pcBSType == 'B') {
			stream.WriteBool(true);
		}
		else {
			stream.WriteBool(false);
		}
		// Usually pNew->ItemBuy[i].m_wBuyBrokerNo is equal to pOld->ItemBuy[i].m_wBuyBrokerNo, so we set this flag.
		// Otherwise, we store the data in its original length because it only takes 14 bits.
		// On the other hand, the range of the encode function is rather limited.
		if (pNew->m_stItemBuy[i].m_wBSBrokerNo == pOld->m_stItemBuy[i].m_wBSBrokerNo) {
			stream.WriteBool(false);
		}
		else {
			stream.WriteBool(true);
			stream.WriteDWORD(pNew->m_stItemBuy[i].m_wBSBrokerNo, 14); // max 9999
		}
	}

	for (int i = 0; i < 40; ++i) {
		if (pNew->m_stItemSell[i].m_pcBSType == 'B') {
			stream.WriteBool(true);
		}
		else {
			stream.WriteBool(false);
		}
		if (pNew->m_stItemSell[i].m_wBSBrokerNo == pOld->m_stItemSell[i].m_wBSBrokerNo) {
			stream.WriteBool(false);
		}
		else {
			stream.WriteBool(true);
			stream.WriteDWORD(pNew->m_stItemSell[i].m_wBSBrokerNo, 14); // max 9999
		}
	}
	return 0;
}
int CMultiBQCompressUnit::ExpandData(CBitStream& stream, CStdCacheBrokerQueue* pNew, const UINT16 wVer)
{
	int nFlagB = stream.ReadDWORD(8);
	pNew->m_pcBuyBQMoreFlag = nFlagB;
	int nFlagS = stream.ReadDWORD(8);
	pNew->m_pcSellBQMoreFlag = nFlagS;
	int nNumB = stream.ReadDWORD(6);
	pNew->m_btBuyItemCount = nNumB;
	int nNumS = stream.ReadDWORD(6);
	pNew->m_btSellItemCount = nNumS;
	for (int i = 0; i < nNumB; i++)
	{
		int nType = stream.ReadDWORD(8);
		pNew->m_stItemBuy[i].m_pcBSType = nType;
		int nBrokerNo = stream.ReadDWORD(14);
		pNew->m_stItemBuy[i].m_wBSBrokerNo = nBrokerNo;
	}
	for (int k = 0; k < nNumS; k++)
	{
		int nType = stream.ReadDWORD(8);
		pNew->m_stItemSell[k].m_pcBSType = nType;
		int nBrokerNo = stream.ReadDWORD(14);
		pNew->m_stItemSell[k].m_wBSBrokerNo = nBrokerNo;
	}
	return 0;
}
int CMultiBQCompressUnit::ExpandData(CBitStream& stream, CStdCacheBrokerQueue* pNew, const CStdCacheBrokerQueue* pOld, const UINT16 wVer)
{
	bool blnIsSnap = stream.ReadDWORD(1);
	if (blnIsSnap) {
		return CMultiBQCompressUnit::ExpandSnapData(stream, pNew, wVer);
	}
	else {
		if (pOld == NULL) {
			return -1;
		}
		else {
			return CMultiBQCompressUnit::ExpandAddData(stream, pNew, pOld, wVer);
		}
	}
}
int CMultiBQCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheBrokerQueue* pNew, const UINT16 wVer)
{
	if (stream.ReadDWORD(1)) {
		pNew->m_pcBuyBQMoreFlag = 'Y';
	}
	else {
		pNew->m_pcBuyBQMoreFlag = 'N';
	}

	if (stream.ReadDWORD(1)) {
		pNew->m_pcSellBQMoreFlag = 'Y';
	}
	else {
		pNew->m_pcSellBQMoreFlag = 'N';
	}

	pNew->m_btBuyItemCount = stream.ReadDWORD(6);
	pNew->m_btSellItemCount = stream.ReadDWORD(6);

	for (int i = 0; i < 40; i++) {
		if (stream.ReadDWORD(1)) {
			pNew->m_stItemBuy[i].m_pcBSType = 'B';
		}
		else {
			pNew->m_stItemBuy[i].m_pcBSType = 'S';
		}
		pNew->m_stItemBuy[i].m_wBSBrokerNo = stream.ReadDWORD(14);
	}

	for (int i = 0; i < 40; i++) {
		if (stream.ReadDWORD(1)) {
			pNew->m_stItemSell[i].m_pcBSType = 'B';
		}
		else {
			pNew->m_stItemSell[i].m_pcBSType = 'S';
		}
		pNew->m_stItemSell[i].m_wBSBrokerNo = stream.ReadDWORD(14);
	}

	return 0;
}
int CMultiBQCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheBrokerQueue* pNew, const CStdCacheBrokerQueue* pOld, const UINT16 wVer)
{
	if (stream.ReadDWORD(1)) {
		pNew->m_pcBuyBQMoreFlag = 'Y';
	}
	else {
		pNew->m_pcBuyBQMoreFlag = 'N';
	}

	if (stream.ReadDWORD(1)) {
		pNew->m_pcSellBQMoreFlag = 'Y';
	}
	else{
		pNew->m_pcSellBQMoreFlag = 'N';
	}

	pNew->m_btBuyItemCount = stream.ReadDWORD(6);
	pNew->m_btSellItemCount = stream.ReadDWORD(6);

	for (int i = 0; i < 40; i++) {
		if (stream.ReadDWORD(1)) {
			pNew->m_stItemBuy[i].m_pcBSType = 'B';
		}
		else {
			pNew->m_stItemBuy[i].m_pcBSType = 'S';
		}
		if (stream.ReadDWORD(1)) {
			pNew->m_stItemBuy[i].m_wBSBrokerNo = stream.ReadDWORD(14);
		}
		else {
			pNew->m_stItemBuy[i].m_wBSBrokerNo = pOld->m_stItemBuy[i].m_wBSBrokerNo;
		}
	}

	for (int i = 0; i < 40; i++) {
		if (stream.ReadDWORD(1)) {
			pNew->m_stItemSell[i].m_pcBSType = 'B';
		}
		else {
			pNew->m_stItemSell[i].m_pcBSType = 'S';
		}
		if (stream.ReadDWORD(1)) {
			pNew->m_stItemSell[i].m_wBSBrokerNo = stream.ReadDWORD(14);
		}
		else {
			pNew->m_stItemSell[i].m_wBSBrokerNo = pOld->m_stItemSell[i].m_wBSBrokerNo;
		}
	}
	return 0;
}

template <typename T>
int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const T *pNew, const UINT16 wVer) {
	stream.WriteDWORD(pNew->m_dwTradeSequence, 26);
	stream.WriteDWORD(pNew->m_dwDate, 26);
	stream.WriteDWORD(pNew->m_dwTime, 18);
	stream.WriteDWORD(pNew->m_cTradeFlag, 2);		// 0,1
	stream.EncodeData(pNew->m_dwClose, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	stream.EncodeData(pNew->m_dwOpen, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, &pNew->m_dwClose);
	stream.EncodeData(pNew->m_dwHigh, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, &pNew->m_dwOpen);
	stream.EncodeData(pNew->m_dwLow, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, &pNew->m_dwOpen, true);
	stream.EncodeData(pNew->m_dwPrice, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, &pNew->m_dwLow);
	stream.EncodeBigInt(pNew->m_xVolume, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.EncodeBigInt(pNew->m_xAmount, BC_DYNA_AMNT, BCNUM_DYNA_AMNT);
	stream.EncodeBigInt(pNew->m_xTradeNum, BC_DYNA_TRADENUM, BCNUM_DYNA_TRADENUM);
	CompressMMP(stream, pNew, wVer);
	stream.EncodeBigInt(pNew->m_xWaiPan, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.EncodeBigInt(pNew->m_xCurVol, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	stream.WriteDWORD(pNew->m_cCurVol + 1, 2);		// -1 0 1
	if (pNew->m_xCurOI.GetValue() > 0) {
		stream.WriteBool(0);
		stream.EncodeBigInt(pNew->m_xCurOI, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	}
	else {
		stream.WriteBool(1);
		CBigInt tmp = 0 - pNew->m_xCurOI.GetValue();
		stream.EncodeBigInt(tmp, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	}

	stream.EncodeBigInt(pNew->m_xOpenInterest, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.EncodeBigInt(pNew->m_xPreOpenInterest, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.EncodeData(pNew->m_dwAvg, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	stream.EncodeData(pNew->m_dwAvgPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	stream.EncodeData(pNew->m_dwPreAvgPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);

	stream.EncodeData(pNew->m_dwPriceZT, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	stream.EncodeData(pNew->m_dwPriceDT, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	return 0;
}
template int CMultiQtCompressUnit::CompressSnapData<CStdCacheQt>(COutputBitStream&, const CStdCacheQt*, const unsigned short);
template int CMultiQtCompressUnit::CompressSnapData<CStdCacheQtS>(COutputBitStream&, const CStdCacheQtS*, const unsigned short);

template <typename T>
int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, T *pNew, const UINT16 wVer) {
	pNew->m_dwTradeSequence = stream.ReadDWORD(26);
	pNew->m_dwDate = stream.ReadDWORD(26);
	pNew->m_dwTime = stream.ReadDWORD(18);
	pNew->m_cTradeFlag = stream.ReadDWORD(2);
	stream.DecodeData(pNew->m_dwClose, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	stream.DecodeData(pNew->m_dwOpen, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, &pNew->m_dwClose);
	stream.DecodeData(pNew->m_dwHigh, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, &pNew->m_dwOpen);
	stream.DecodeData(pNew->m_dwLow, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, &pNew->m_dwOpen, true);
	stream.DecodeData(pNew->m_dwPrice, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, &pNew->m_dwLow);
	stream.DecodeBigInt(pNew->m_xVolume, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.DecodeBigInt(pNew->m_xAmount, BC_DYNA_AMNT, BCNUM_DYNA_AMNT);
	stream.DecodeBigInt(pNew->m_xTradeNum, BC_DYNA_TRADENUM, BCNUM_DYNA_TRADENUM);
	ExpandMMP(stream, pNew, wVer);
	stream.DecodeBigInt(pNew->m_xWaiPan, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.DecodeBigInt(pNew->m_xCurVol, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	pNew->m_cCurVol = stream.ReadDWORD(2) - 1;		// -1 0 1
	if (stream.ReadDWORD(1)) {
		CBigInt tmp;
		stream.DecodeBigInt(tmp, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
		pNew->m_xCurOI = 0 - tmp.GetValue();
	}
	else {
		stream.DecodeBigInt(pNew->m_xCurOI, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	}

	stream.DecodeBigInt(pNew->m_xOpenInterest, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.DecodeBigInt(pNew->m_xPreOpenInterest, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.DecodeData(pNew->m_dwAvg, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	stream.DecodeData(pNew->m_dwAvgPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	stream.DecodeData(pNew->m_dwPreAvgPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);

	stream.DecodeData(pNew->m_dwPriceZT, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	stream.DecodeData(pNew->m_dwPriceDT, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	return 0;
}
template int CMultiQtCompressUnit::ExpandSnapData<CStdCacheQt>(CBitStream& stream, CStdCacheQt*, const unsigned short);
template int CMultiQtCompressUnit::ExpandSnapData<CStdCacheQtS>(CBitStream& stream, CStdCacheQtS*, const unsigned short);

template <typename T>
int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const T *pNew, const T *pOld, const UINT16 wVer) {
	//stream.WriteBool(1);
	if (pNew->m_dwTradeSequence == pOld->m_dwTradeSequence) {
		stream.WriteBool(false);
	}
	else {
		stream.WriteBool(true);
		stream.WriteDWORD(pNew->m_dwTradeSequence, 26);
	}

	if (pNew->m_dwDate == pOld->m_dwDate) {
		stream.WriteBool(false);
	}
	else {
		stream.WriteBool(true);
		stream.WriteDWORD(pNew->m_dwDate, 26);
	}

	UINT32 dwSecondsDiff = GetSecondsDiff(pOld->m_dwTime, pNew->m_dwTime);
	stream.EncodeData(dwSecondsDiff, BC_DYNA_TIMETDIFF, BCNUM_DYNA_TIMETDIFF);
	stream.WriteDWORD(pNew->m_cTradeFlag, 2);		// 0,1
	EncodeDataDiff(pNew->m_dwClose, pOld->m_dwClose, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwOpen, pOld->m_dwOpen, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwHigh, pOld->m_dwHigh, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwLow, pOld->m_dwLow, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwPrice, pOld->m_dwPrice, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeBigintDiff(pNew->m_xVolume, pOld->m_xVolume, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	EncodeBigintDiff(pNew->m_xAmount, pOld->m_xAmount, BC_DYNA_AMNT, BCNUM_DYNA_AMNT, stream);
	EncodeBigintDiff(pNew->m_xTradeNum, pOld->m_xTradeNum, BC_DYNA_TRADENUM, BCNUM_DYNA_TRADENUM, stream);
	CompressMMP(stream, pNew, pOld, wVer);
	EncodeBigintDiff(pNew->m_xWaiPan, pOld->m_xWaiPan, BC_DYNA_VOL, BCNUM_DYNA_VOL, stream);
	stream.EncodeBigInt(pNew->m_xCurVol, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	stream.WriteDWORD(pNew->m_cCurVol + 1, 2);		// -1 0 1
	if (pNew->m_xCurOI.GetValue() > 0) {
		stream.WriteBool(0);
		stream.EncodeBigInt(pNew->m_xCurOI, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	}
	else {
		stream.WriteBool(1);
		CBigInt tmp = 0 - pNew->m_xCurOI.GetValue();
		stream.EncodeBigInt(tmp, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	}

	EncodeBigintDiff(pNew->m_xOpenInterest, pOld->m_xOpenInterest, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	EncodeBigintDiff(pNew->m_xPreOpenInterest, pOld->m_xPreOpenInterest, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);

	EncodeDataDiff(pNew->m_dwAvg, pOld->m_dwAvg, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwAvgPrice, pOld->m_dwAvgPrice, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwPreAvgPrice, pOld->m_dwPreAvgPrice, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

	EncodeDataDiff(pNew->m_dwPriceZT, pOld->m_dwPriceZT, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwPriceDT, pOld->m_dwPriceDT, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

	return 0;
}
template int CMultiQtCompressUnit::CompressAddData<CStdCacheQt>(COutputBitStream&, const CStdCacheQt*, const CStdCacheQt*, const unsigned short);
template int CMultiQtCompressUnit::CompressAddData<CStdCacheQtS>(COutputBitStream&, const CStdCacheQtS*, const CStdCacheQtS*, const unsigned short);

template <typename T>
int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, T *pNew, const T *pOld, const UINT16 wVer) {
	if (stream.ReadDWORD(1)) {
		pNew->m_dwTradeSequence = stream.ReadDWORD(26);
	}

	if (stream.ReadDWORD(1)) {
		pNew->m_dwDate = stream.ReadDWORD(26);
	}

	UINT32 dwSecondsDiff = 0;
	stream.DecodeData(dwSecondsDiff, BC_DYNA_TIMETDIFF, BCNUM_DYNA_TIMETDIFF);
	pNew->m_dwTime = AddSeconds(pOld->m_dwTime, dwSecondsDiff);
	pNew->m_cTradeFlag = stream.ReadDWORD(2);
	DecodeDataDiff(pNew->m_dwClose, pOld->m_dwClose, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwOpen, pOld->m_dwOpen, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwHigh, pOld->m_dwHigh, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwLow, pOld->m_dwLow, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwPrice, pOld->m_dwPrice, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeBigintDiff(pNew->m_xVolume, pOld->m_xVolume, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	DecodeBigintDiff(pNew->m_xAmount, pOld->m_xAmount, BC_DYNA_AMNT, BCNUM_DYNA_AMNT, stream);
	DecodeBigintDiff(pNew->m_xTradeNum, pOld->m_xTradeNum, BC_DYNA_TRADENUM, BCNUM_DYNA_TRADENUM, stream);
	ExpandMMP(stream, pNew, pOld, wVer);
	DecodeBigintDiff(pNew->m_xWaiPan, pOld->m_xWaiPan, BC_DYNA_VOL, BCNUM_DYNA_VOL, stream);
	stream.DecodeBigInt(pNew->m_xCurVol, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	pNew->m_cCurVol = (BYTE)stream.ReadDWORD(2) - 1;
	if (stream.ReadDWORD(1) == 0) {
		stream.DecodeBigInt(pNew->m_xCurOI, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	}
	else {
		CBigInt tmp;
		stream.DecodeBigInt(tmp, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
		pNew->m_xCurOI = 0 - tmp.GetValue();
	}

	DecodeBigintDiff(pNew->m_xOpenInterest, pOld->m_xOpenInterest, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	DecodeBigintDiff(pNew->m_xPreOpenInterest, pOld->m_xPreOpenInterest, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);

	DecodeDataDiff(pNew->m_dwAvg, pOld->m_dwAvg, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwAvgPrice, pOld->m_dwAvgPrice, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwPreAvgPrice, pOld->m_dwPreAvgPrice, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

	DecodeDataDiff(pNew->m_dwPriceZT, pOld->m_dwPriceZT, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwPriceDT, pOld->m_dwPriceDT, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	return 0;
}
template int CMultiQtCompressUnit::ExpandAddData<CStdCacheQt>(CBitStream&, CStdCacheQt*, const CStdCacheQt*, const unsigned short);
template int CMultiQtCompressUnit::ExpandAddData<CStdCacheQtS>(CBitStream&, CStdCacheQtS*, const CStdCacheQtS*, const unsigned short);

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheBlockQt* pNew, const UINT16 wVer)
{
	stream.WriteDWORD(pNew->m_dwTradeSequence, 26);
	stream.WriteDWORD(pNew->m_dwDate, 26);
	stream.WriteDWORD(pNew->m_dwTime, 18);

	stream.EncodeData(pNew->m_dwClose, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	stream.EncodeData(pNew->m_dwOpen, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, &pNew->m_dwClose);
	stream.EncodeData(pNew->m_dwHigh, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, &pNew->m_dwOpen);
	stream.EncodeData(pNew->m_dwLow, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, &pNew->m_dwOpen, true);
	stream.EncodeData(pNew->m_dwPrice, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, &pNew->m_dwLow);
	stream.EncodeBigInt(pNew->m_xVolume, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.EncodeBigInt(pNew->m_xAmount, BC_DYNA_AMNT, BCNUM_DYNA_AMNT);

	int nNum = pNew->m_pchTopStockMarketID;
	stream.WriteDWORD(nNum, 8);

	stream.WriteDWORD(pNew->m_dwStockNum, 18);
	stream.WriteDWORD(pNew->m_dwUpNum, 18);
	stream.WriteDWORD(pNew->m_dwDownNum, 18);
	stream.WriteDWORD(pNew->m_dwStrongNum, 18);
	stream.WriteDWORD(pNew->m_dwWeakNum, 18);

	stream.EncodeBigInt(pNew->m_xZGB, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.EncodeBigInt(pNew->m_xZSZ, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.EncodeBigInt(pNew->m_xAvgProfit, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.EncodeBigInt(pNew->m_xPeRatio, BC_DYNA_VOL, BCNUM_DYNA_VOL);

	stream.EncodeBigInt(pNew->m_xAvgAmount2Day, BC_DYNA_AMNT, BCNUM_DYNA_AMNT);
	stream.EncodeBigInt(pNew->m_xAvgAmount20Day, BC_DYNA_AMNT, BCNUM_DYNA_AMNT);
	stream.EncodeBigInt(pNew->m_xTurnover2Day, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.EncodeBigInt(pNew->m_xTurnover20Day, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.EncodeBigInt(pNew->m_xPercent3Day, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.EncodeBigInt(pNew->m_xPercent20Day, BC_DYNA_VOL, BCNUM_DYNA_VOL);

	stream.EncodeData(pNew->m_dwPre5MinPrice[0], BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	stream.EncodeData(pNew->m_dwPre5MinPrice[1], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, &pNew->m_dwPre5MinPrice[0]);
	stream.EncodeData(pNew->m_dwPre5MinPrice[2], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, &pNew->m_dwPre5MinPrice[0]);
	stream.EncodeData(pNew->m_dwPre5MinPrice[3], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, &pNew->m_dwPre5MinPrice[0]);
	stream.EncodeData(pNew->m_dwPre5MinPrice[4], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, &pNew->m_dwPre5MinPrice[0]);

	return 0;
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheBlockOverView* pNew, const UINT16 version) {
    if (version == MCM_DEFAULT_OLD_VERSION) {
        stream.EncodeBigInt(pNew->m_xNetFlow, BC_DYNA_VOL, BCNUM_DYNA_VOL);
        if (pNew->m_dwPercent > 0) {
            stream.WriteBool(0);
            stream.WriteDWORD(pNew->m_dwPercent, 12);
        } else {
            stream.WriteBool(1);
            int tmp = 0 - pNew->m_dwPercent;
            stream.WriteDWORD(tmp, 12);
        }

        stream.WriteDWORD(pNew->m_dwUpDay, 12);
        int nNum = pNew->m_pchTopStockMarketID;
        stream.WriteDWORD(nNum, 8);

        if (pNew->m_dwTopPercent > 0) {
            stream.WriteBool(0);
            stream.WriteDWORD(pNew->m_dwTopPercent, 12);
        } else {
            stream.WriteBool(1);
            int tmp = 0 - pNew->m_dwTopPercent;
            stream.WriteDWORD(tmp, 12);
        }

    } else if (version == MCM_VERSION) {
        stream.EncodeData(pNew->m_wType, GENERAL_UNSIGNED_SHORT_MATCH, GENERAL_UNSIGNED_SHORT_MATCH_NUMBER);
        stream.EncodeBigInt(pNew->m_xNetFlow, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.encodeData(pNew->m_dwPercent, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER);
        stream.EncodeData(pNew->m_dwUpDay, GENERAL_UNSIGNED_SHORT_MATCH, GENERAL_UNSIGNED_SHORT_MATCH_NUMBER);
        stream.encodeData(pNew->m_dwTopPercent, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER);
    }

	return 0;
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheBrokerTrace* pNew, const UINT16 wVer)
{
	int nBuyNum = pNew->mapBuyBT.size();
	stream.WriteDWORD(nBuyNum, 6);
	int nTotalCountB = 0;
	for (map<BYTE, set<CStdCacheBrokerTrace::STrace> >::const_iterator it = pNew->mapBuyBT.begin(); it != pNew->mapBuyBT.end(); it++)
	{
		int nIndex = it->first;
		int nSonNum = it->second.size();
		int nIntervalNum = 200 - nTotalCountB;  //at most send 200 stocks
		if (nIntervalNum == 0)
		{
			break;
		}

		int nUsedNum = 0;
		if (nIntervalNum >= nSonNum)
		{
			nTotalCountB += nSonNum;
			nUsedNum = nSonNum;
		}
		else
		{
			nTotalCountB += nIntervalNum;
			nUsedNum = nIntervalNum;
		}

		stream.WriteDWORD(nIndex, 6);
		set<CStdCacheBrokerTrace::STrace> vecTrace = it->second;
		stream.WriteDWORD(nUsedNum, 8);
		/*
		for(int i = 0; i < nUsedNum; i++)
		{
		CStdCacheBrokerTrace::STrace Item = vecTrace[i];
		stream.EncodeData(Item.dwStockID, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(Item.dwPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		}
		*/
		int nCountB = 0;
		for (set<CStdCacheBrokerTrace::STrace>::iterator ittmp = vecTrace.begin(); ittmp != vecTrace.end(); ittmp++)
		{
			if (nCountB >= nUsedNum)
			{
				break;
			}
			CStdCacheBrokerTrace::STrace Item = *ittmp;
			stream.EncodeData(Item.dwStockID, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			stream.WriteDWORD(Item.ucFlag, 2);
			stream.EncodeData(Item.dwPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			nCountB++;
		}

	}

	int nSellNum = pNew->mapSellBT.size();
	stream.WriteDWORD(nSellNum, 6);
	int nTotalCountS = 0;
	for (map<BYTE, set<CStdCacheBrokerTrace::STrace> >::const_iterator it = pNew->mapSellBT.begin(); it != pNew->mapSellBT.end(); it++)
	{
		int nIndex = it->first;
		int nSonNum = it->second.size();
		int nIntervalNum = 200 - nTotalCountS;   //at most send 200 stocks
		if (nIntervalNum == 0)
		{
			break;
		}

		int nUsedNum = 0;
		if (nIntervalNum >= nSonNum)
		{
			nTotalCountS += nSonNum;
			nUsedNum = nSonNum;
		}
		else
		{
			nTotalCountS += nIntervalNum;
			nUsedNum = nIntervalNum;
		}

		stream.WriteDWORD(nIndex, 6);
		set<CStdCacheBrokerTrace::STrace> vecTrace = it->second;
		stream.WriteDWORD(nUsedNum, 8);
		/*
		for(int i = 0; i < nUsedNum; i++)
		{
		CStdCacheBrokerTrace::STrace Item = vecTrace[i];
		stream.EncodeData(Item.dwStockID, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(Item.dwPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		}
		*/
		int nCountS = 0;
		for (set<CStdCacheBrokerTrace::STrace>::iterator ittmp = vecTrace.begin(); ittmp != vecTrace.end(); ittmp++)
		{
			if (nCountS >= nUsedNum)
			{
				break;
			}
			CStdCacheBrokerTrace::STrace Item = *ittmp;
			stream.EncodeData(Item.dwStockID, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			stream.WriteDWORD(Item.ucFlag, 2);
			stream.EncodeData(Item.dwPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			nCountS++;
		}

	}

	return 0;
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheQtEx* pNew, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		// m_bTradingStatus has only four states, namely 0, 2, 3, so we only need 2 bits to record these states.
		unsigned int tradingStatus = pNew->m_bTradingStatus;
		stream.WriteDWORD(tradingStatus, 2);

		// So far m_cCurrencyCode has only eight possible values, so I set four bits to record these states.
		// The fourth bit is just for future use.
		UINT32 currencyCode = 0;
		if (strncmp(pNew->m_cCurrencyCode, "HKD", 3) == 0) currencyCode = 0;
		else if (strncmp(pNew->m_cCurrencyCode, "USD", 3) == 0) currencyCode = 1;
		else if (strncmp(pNew->m_cCurrencyCode, "EUR", 3) == 0) currencyCode = 2;
		else if (strncmp(pNew->m_cCurrencyCode, "JPY", 3) == 0) currencyCode = 3;
		else if (strncmp(pNew->m_cCurrencyCode, "GBP", 3) == 0) currencyCode = 4;
		else if (strncmp(pNew->m_cCurrencyCode, "CAD", 3) == 0) currencyCode = 5;
		else if (strncmp(pNew->m_cCurrencyCode, "SGD", 3) == 0) currencyCode = 6;
		else if (strncmp(pNew->m_cCurrencyCode, "CNY", 3) == 0) currencyCode = 7;
		else if (strncmp(pNew->m_cCurrencyCode, "AUD", 3) == 0) currencyCode = 8;
		else                                                  currencyCode = 9;
		stream.WriteDWORD(currencyCode, 4);

		// The following flag members have only three possible values, so I set 2 bits to record these states.
		UINT32 spreadTable = 0;
		if (strncmp(pNew->m_cSpreadTable, "01", 2) == 0) spreadTable = 0;
		else if (strncmp(pNew->m_cSpreadTable, "03", 2) == 0) spreadTable = 1;
		stream.WriteDWORD(spreadTable, 1);

		UINT32 casFlag = 0;
		if (pNew->m_cCASFlag == 'Y') casFlag = 0;
		else if (pNew->m_cCASFlag == 'N') casFlag = 1;
		else if (pNew->m_cCASFlag == '\0') casFlag = 2;
		stream.WriteDWORD(casFlag, 2);

		UINT32 vcmFlag = 0;
		if (pNew->m_cVCMFlag == 'Y') vcmFlag = 0;
		else if (pNew->m_cVCMFlag == 'N') vcmFlag = 1;
		else if (pNew->m_cVCMFlag == '\0') vcmFlag = 2;
		stream.WriteDWORD(vcmFlag, 2);

		UINT32 shortSellFlag = 0;
		if (pNew->m_cShortSellFlag == 'Y') shortSellFlag = 0;
		else if (pNew->m_cShortSellFlag == 'N') shortSellFlag = 1;
		else if (pNew->m_cShortSellFlag == '\0') shortSellFlag = 2;
		stream.WriteDWORD(shortSellFlag, 2);

		UINT32 ccassFlag = 0;
		if (pNew->m_cCCASSFlag == 'Y') ccassFlag = 0;
		else if (pNew->m_cCCASSFlag == 'N') ccassFlag = 1;
		else if (pNew->m_cCCASSFlag == '\0') ccassFlag = 2;
		stream.WriteDWORD(ccassFlag, 2);

		UINT32 dummySecurityFlag = 0;
		if (pNew->m_cDummySecurityFlag == 'Y') dummySecurityFlag = 0;
		else if (pNew->m_cDummySecurityFlag == 'N') dummySecurityFlag = 1;
		else if (pNew->m_cDummySecurityFlag == '\0') dummySecurityFlag = 2;
		stream.WriteDWORD(dummySecurityFlag, 2);

		UINT32 stampDutyFlag = 0;
		if (pNew->m_cStampDutyFlag == 'Y') stampDutyFlag = 0;
		else if (pNew->m_cStampDutyFlag == 'N') stampDutyFlag = 1;
		else if (pNew->m_cStampDutyFlag == '\0') stampDutyFlag = 2;
		stream.WriteDWORD(stampDutyFlag, 2);

		UINT32 efnFalg = 0;
		if (pNew->m_cEFNFalg == 'Y') efnFalg = 0;
		else if (pNew->m_cEFNFalg == 'N') efnFalg = 1;
		else if (pNew->m_cEFNFalg == '\0') efnFalg = 2;
		stream.WriteDWORD(efnFalg, 2);

		UINT32 callPutFlag = 0;
		if (pNew->m_cCallPutFlag == 'Y') callPutFlag = 0;
		else if (pNew->m_cCallPutFlag == 'N') callPutFlag = 1;
		else if (pNew->m_cCallPutFlag == 'P') callPutFlag = 2;
		else if (pNew->m_cCallPutFlag == 'C') callPutFlag = 3;
		else if (pNew->m_cCallPutFlag == '\0') callPutFlag = 4;
		stream.WriteDWORD(callPutFlag, 3);

		UINT32 style = 0;
		if (pNew->m_cStyle == 'A') style = 0;
		else if (pNew->m_cStyle == 'E') style = 1;
		else if (pNew->m_cStyle == '\0') style = 2;
		stream.WriteDWORD(style, 2);

		// Usually this data member is NULL, so it is necessary to set a flag to indicate whether it is NULL for compression efficiency.
		if (*pNew->m_cFiller != '\0') {
			stream.WriteBool(false);
			UINT32 filler = 0;
			memcpy(&filler, pNew->m_cFiller, 4);
			stream.WriteDWORD(filler, 32);
		}
		else {
			stream.WriteBool(true);
		}

		UINT32 underlyingCodeLength = strlen(pNew->m_pchUnderlyingCode);
		stream.WriteDWORD(underlyingCodeLength, 4);
		UINT32 numWhole = underlyingCodeLength / 4;
		if (numWhole != 0) {
			UINT32 *underlyingCode = new UINT32[numWhole];
			memcpy(underlyingCode, pNew->m_pchUnderlyingCode, numWhole * 4);
			for (int k = 0; k < numWhole; ++k)
				stream.WriteDWORD(underlyingCode[k], 32);
			if (underlyingCode != NULL) delete[] underlyingCode;
		}

		UINT32 leftLength = underlyingCodeLength % 4;
		if (leftLength != 0) {
			UINT32 leftCode;
			memcpy(&leftCode, pNew->m_pchUnderlyingCode + numWhole * 4, leftLength);
			stream.WriteDWORD(leftCode, leftLength * 8);
		}

		stream.WriteDWORD(pNew->m_dwLotSize, 18);
		stream.EncodeData(pNew->m_dwListingDate, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(pNew->m_dwDelistingDate, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);

		stream.EncodeData(pNew->m_dwCouponRate, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(pNew->m_dwAccruedInt, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(pNew->m_dwYield, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);

		stream.EncodeData(pNew->m_dwConversionRatio, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(pNew->m_dwStrikePrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(pNew->m_dwMaturityDate, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.WriteDWORD(pNew->m_cUnderlyingMarket, 8);            //
		UINT32 IEPrice;
		memcpy(&IEPrice, &pNew->m_dwIEPrice, sizeof IEPrice);
		stream.EncodeData(IEPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);

		stream.EncodeBigInt(pNew->m_xIEVolume, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	}
	else {
		stream.WriteDWORD(pNew->m_bTradingStatus, 2); // 0, 2, 3
		EncodeArray(stream, pNew->m_cCurrencyCode);
		stream.WriteDWORD(pNew->m_dwLotSize, 20); // 1M is enough
		EncodeArray(stream, pNew->m_cSpreadTable);
		stream.WriteDWORD(pNew->m_cCASFlag, 8);
		stream.WriteDWORD(pNew->m_cVCMFlag, 8);
		stream.WriteDWORD(pNew->m_cShortSellFlag, 8);
		stream.WriteDWORD(pNew->m_cCCASSFlag, 8);
		stream.WriteDWORD(pNew->m_cDummySecurityFlag, 8);
		stream.WriteDWORD(pNew->m_cStampDutyFlag, 8);
		stream.WriteDWORD(pNew->m_dwListingDate, DATE_YYYYMMDD_BITS);
		stream.WriteDWORD(pNew->m_dwDelistingDate, DATE_YYYYMMDD_BITS);
		EncodeArray(stream, pNew->m_cFiller);
		stream.WriteDWORD(pNew->m_cEFNFalg, 8);
		stream.EncodeData(pNew->m_dwCouponRate, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(pNew->m_dwAccruedInt, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(pNew->m_dwYield, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.WriteDWORD(pNew->m_cCallPutFlag, 8);
		stream.EncodeData(pNew->m_dwConversionRatio, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(pNew->m_dwStrikePrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.WriteDWORD(pNew->m_dwMaturityDate, DATE_YYYYMMDD_BITS);
		stream.WriteDWORD(pNew->m_cUnderlyingMarket, 8);
		EncodeArray(stream, pNew->m_pchUnderlyingCode);
		stream.WriteDWORD(pNew->m_cStyle, 8);
		stream.EncodeData(pNew->m_dwIEPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeBigInt(pNew->m_xIEVolume, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	}
	return 0;
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheCAS* pNew, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		UINT32 orderImbalanceDirection = 0;
		if (pNew->m_cOrderImbalanceDirection == 'N') orderImbalanceDirection = 0;
		else if (pNew->m_cOrderImbalanceDirection == 'B') orderImbalanceDirection = 1;
		else if (pNew->m_cOrderImbalanceDirection == 'S') orderImbalanceDirection = 2;
		else if (pNew->m_cOrderImbalanceDirection == ' ') orderImbalanceDirection = 3;
		else if (pNew->m_cOrderImbalanceDirection == '\0') orderImbalanceDirection = 4;
		stream.WriteDWORD(orderImbalanceDirection, 3);
		// If pNew->m_cOrderImbalanceDirection == ' ', there is no need to encode pNew->m_xOrderImbalanceQuantity.
		if (pNew->m_cOrderImbalanceDirection == ' ') {
			stream.WriteBool(true);
		}
		else {
			stream.WriteBool(false);
			stream.EncodeBigInt(pNew->m_xOrderImbalanceQuantity, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
		}
		UINT32 referencePrice;
		UINT32 lowerPrice;
		UINT32 upperPrice;
		memcpy(&referencePrice, &pNew->m_dwReferencePrice, sizeof referencePrice);
		memcpy(&lowerPrice, &pNew->m_dwLowerPrice, sizeof lowerPrice);
		memcpy(&upperPrice, &pNew->m_xUpperPrice, sizeof upperPrice);
		stream.EncodeData(referencePrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(lowerPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(upperPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	}
	else {
		stream.WriteDWORD(pNew->m_cOrderImbalanceDirection, 8);
		stream.EncodeBigInt(pNew->m_xOrderImbalanceQuantity, BC_DYNA_VOL, BCNUM_DYNA_VOL);
		stream.EncodeData(pNew->m_dwReferencePrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(pNew->m_dwLowerPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(pNew->m_xUpperPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	}
	return 0;
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheVCM* pNew, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		if (pNew->m_dwVCMDate == 0 && pNew->m_dwVCMStartTime == 0 && pNew->m_dwVCMEndTime == 0
			&& pNew->m_dwVCMReferencePrice == 0 && pNew->m_xVCMLowerPrice == 0 && pNew->m_xVCMUpperPrice == 0) {
			stream.WriteBool(true);
		}
		else {
			stream.WriteBool(false);
			stream.EncodeData(pNew->m_dwVCMDate, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			stream.EncodeData(pNew->m_dwVCMStartTime, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			stream.EncodeData(pNew->m_dwVCMEndTime, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			UINT32 referencePrice;
			UINT32 lowerPrice;
			UINT32 upperPrice;
			memcpy(&referencePrice, &pNew->m_dwVCMReferencePrice, sizeof referencePrice);
			memcpy(&lowerPrice, &pNew->m_xVCMLowerPrice, sizeof lowerPrice);
			memcpy(&upperPrice, &pNew->m_xVCMUpperPrice, sizeof upperPrice);
			stream.EncodeData(referencePrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			stream.EncodeData(lowerPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			stream.EncodeData(upperPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		}
	}
	else {
		stream.WriteDWORD(pNew->m_dwVCMDate, DATE_YYYYMMDD_BITS);
		stream.WriteDWORD(pNew->m_dwVCMStartTime, TIME_HHMMSS_BITS);
		stream.WriteDWORD(pNew->m_dwVCMEndTime, TIME_HHMMSS_BITS);
		stream.EncodeData(pNew->m_dwVCMReferencePrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(pNew->m_xVCMLowerPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.EncodeData(pNew->m_xVCMUpperPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	}
	return 0;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheBlockOverView* pNew, const UINT16 version) {
    if (version == MCM_DEFAULT_OLD_VERSION) {
        stream.DecodeBigInt(pNew->m_xNetFlow, BC_DYNA_VOL, BCNUM_DYNA_VOL);
        if (stream.ReadDWORD(1))
        {
            int tmp = stream.ReadDWORD(12);
            pNew->m_dwPercent = 0 - tmp;
        }
        else
        {
            pNew->m_dwPercent = stream.ReadDWORD(12);
        }
        pNew->m_dwUpDay = stream.ReadDWORD(12);
        int MarketID = stream.ReadDWORD(8);
        pNew->m_pchTopStockMarketID = MarketID;
        if (stream.ReadDWORD(1))
        {
            int tmp = stream.ReadDWORD(12);
            pNew->m_dwTopPercent = 0 - tmp;
        }
        else
        {
            pNew->m_dwTopPercent = stream.ReadDWORD(12);
        }
    } else if (version == MCM_VERSION) {
        unsigned int type = 0;
        stream.DecodeData(type, GENERAL_UNSIGNED_SHORT_MATCH, GENERAL_UNSIGNED_SHORT_MATCH_NUMBER);
        pNew->m_wType = type;
        stream.DecodeBigInt(pNew->m_xNetFlow, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        pNew->m_dwPercent = stream.decodeData(GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER);
        stream.DecodeData(pNew->m_dwUpDay, GENERAL_UNSIGNED_SHORT_MATCH, GENERAL_UNSIGNED_SHORT_MATCH_NUMBER);
        pNew->m_dwTopPercent = stream.decodeData(GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER);
    }

	return 0;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheBlockQt* pNew, const UINT16 wVer)
{
	pNew->m_dwTradeSequence = stream.ReadDWORD(26);
	pNew->m_dwDate = stream.ReadDWORD(26);
	pNew->m_dwTime = stream.ReadDWORD(18);
	stream.DecodeData(pNew->m_dwClose, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	stream.DecodeData(pNew->m_dwOpen, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, &pNew->m_dwClose);
	stream.DecodeData(pNew->m_dwHigh, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, &pNew->m_dwOpen);
	stream.DecodeData(pNew->m_dwLow, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, &pNew->m_dwOpen, true);
	stream.DecodeData(pNew->m_dwPrice, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, &pNew->m_dwLow);
	stream.DecodeBigInt(pNew->m_xVolume, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.DecodeBigInt(pNew->m_xAmount, BC_DYNA_AMNT, BCNUM_DYNA_AMNT);

	int MarketID = stream.ReadDWORD(8);
	pNew->m_pchTopStockMarketID = MarketID;

	pNew->m_dwStockNum = stream.ReadDWORD(18);
	pNew->m_dwUpNum = stream.ReadDWORD(18);
	pNew->m_dwDownNum = stream.ReadDWORD(18);
	pNew->m_dwStrongNum = stream.ReadDWORD(18);
	pNew->m_dwWeakNum = stream.ReadDWORD(18);

	stream.DecodeBigInt(pNew->m_xZGB, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.DecodeBigInt(pNew->m_xZSZ, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.DecodeBigInt(pNew->m_xAvgProfit, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.DecodeBigInt(pNew->m_xPeRatio, BC_DYNA_VOL, BCNUM_DYNA_VOL);

	stream.DecodeBigInt(pNew->m_xAvgAmount2Day, BC_DYNA_AMNT, BCNUM_DYNA_AMNT);
	stream.DecodeBigInt(pNew->m_xAvgAmount20Day, BC_DYNA_AMNT, BCNUM_DYNA_AMNT);
	stream.DecodeBigInt(pNew->m_xTurnover2Day, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.DecodeBigInt(pNew->m_xTurnover20Day, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.DecodeBigInt(pNew->m_xPercent3Day, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	stream.DecodeBigInt(pNew->m_xPercent20Day, BC_DYNA_VOL, BCNUM_DYNA_VOL);

	stream.DecodeData(pNew->m_dwPre5MinPrice[0], BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	stream.DecodeData(pNew->m_dwPre5MinPrice[1], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, &pNew->m_dwPre5MinPrice[0]);
	stream.DecodeData(pNew->m_dwPre5MinPrice[2], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, &pNew->m_dwPre5MinPrice[0]);
	stream.DecodeData(pNew->m_dwPre5MinPrice[3], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, &pNew->m_dwPre5MinPrice[0]);
	stream.DecodeData(pNew->m_dwPre5MinPrice[4], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, &pNew->m_dwPre5MinPrice[0]);
	return 0;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheBrokerTrace* pNew, const UINT16 wVer)
{
	//    map<BYTE, vector<CStdCacheBrokerTrace::STrace> > mapBuy;
	//    mapBuy.clear();
	//    int nBuyNum = stream.ReadDWORD(6);
	//    if(nBuyNum > 0)
	//    {
	//        for(int b = 0; b < nBuyNum; b++)
	//        {
	//            int nIndex = stream.ReadDWORD(6);
	//            vector<CStdCacheBrokerTrace::STrace> vecTrace;
	//
	//            int nSonNum = stream.ReadDWORD(8);
	//            CStdCacheBrokerTrace::STrace Item;
	//            for(int i = 0; i < nSonNum; i++)
	//            {
	//                stream.DecodeData(Item.dwStockID, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	//                //Item.bytMarket = stream.ReadDWORD(8);
	//                stream.DecodeData(Item.dwPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	//            }
	//            vecTrace.push_back(Item);
	//
	//            mapBuy[nIndex] = vecTrace;
	//        }
	//    }
	//
	//    map<BYTE, vector<CStdCacheBrokerTrace::STrace> > SellBuy;
	//    SellBuy.clear();
	//    int nSellNum = stream.ReadDWORD(6);
	//    if(nSellNum > 0)
	//    {
	//        for(int b = 0; b < nSellNum; b++)
	//        {
	//            int nIndex = stream.ReadDWORD(6);
	//            vector<CStdCacheBrokerTrace::STrace> vecTrace;
	//
	//            int nSonNum = stream.ReadDWORD(8);
	//            CStdCacheBrokerTrace::STrace Item;
	//            for(int i = 0; i < nSonNum; i++)
	//            {
	//                stream.DecodeData(Item.dwStockID, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	//                //Item.bytMarket = stream.ReadDWORD(8);
	//                stream.DecodeData(Item.dwPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
	//            }
	//            vecTrace.push_back(Item);
	//
	//            SellBuy[nIndex] = vecTrace;
	//        }
	//    }
	//    pNew->mapBuyBT = mapBuy;
	//    pNew->mapSellBT = SellBuy;
	int nTotalCountB = 0;
	int nBuyNum = stream.ReadDWORD(6);
	if (nBuyNum > 0) {
		for (int b = 0; b < nBuyNum; b++) {
			int nIndex = stream.ReadDWORD(6);
			set<CStdCacheBrokerTrace::STrace> vecTrace;

			int nSonNum = stream.ReadDWORD(8);
			CStdCacheBrokerTrace::STrace Item;
			for (int i = 0; i < nSonNum; i++) {
				stream.DecodeData(Item.dwStockID, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
				Item.ucFlag = stream.ReadDWORD(2);
				stream.DecodeData(Item.dwPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
				vecTrace.insert(Item);
			}

			pNew->mapBuyBT[nIndex] = vecTrace;
			nTotalCountB += nSonNum;
			if (nTotalCountB >= 200)     break;
		}
	}

	int nTotalCountS = 0;
	int nSellNum = stream.ReadDWORD(6);
	if (nSellNum > 0) {
		for (int b = 0; b < nSellNum; b++) {
			int nIndex = stream.ReadDWORD(6);
			set<CStdCacheBrokerTrace::STrace> vecTrace;

			int nSonNum = stream.ReadDWORD(8);
			CStdCacheBrokerTrace::STrace Item;
			for (int i = 0; i < nSonNum; i++) {
				stream.DecodeData(Item.dwStockID, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
				Item.ucFlag = stream.ReadDWORD(2);
				stream.DecodeData(Item.dwPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
				vecTrace.insert(Item);
			}

			pNew->mapSellBT[nIndex] = vecTrace;
			nTotalCountS += nSonNum;
			if (nTotalCountS >= 200)     break;
		}
	}

	return 0;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheQtEx* pNew, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		unsigned int tradingStatus = stream.ReadDWORD(2);
		pNew->m_bTradingStatus = tradingStatus;

		UINT32 currencyCode = stream.ReadDWORD(4);
		if (currencyCode == 0)      strncpy(pNew->m_cCurrencyCode, "HKD", 3);
		else if (currencyCode == 1) strncpy(pNew->m_cCurrencyCode, "USD", 3);
		else if (currencyCode == 2) strncpy(pNew->m_cCurrencyCode, "EUR", 3);
		else if (currencyCode == 3) strncpy(pNew->m_cCurrencyCode, "JPY", 3);
		else if (currencyCode == 4) strncpy(pNew->m_cCurrencyCode, "GBP", 3);
		else if (currencyCode == 5) strncpy(pNew->m_cCurrencyCode, "CAD", 3);
		else if (currencyCode == 6) strncpy(pNew->m_cCurrencyCode, "SGD", 3);
		else if (currencyCode == 7) strncpy(pNew->m_cCurrencyCode, "CNY", 3);
		else if (currencyCode == 8) strncpy(pNew->m_cCurrencyCode, "AUD", 3);
		else if (currencyCode == 9) strncpy(pNew->m_cCurrencyCode, "OTHER", 4);

		UINT32 spreadTable = stream.ReadDWORD(1);
		if (spreadTable == 0) strncpy(pNew->m_cSpreadTable, "01", 2);
		if (spreadTable == 1) strncpy(pNew->m_cSpreadTable, "03", 2);

		UINT32 casFlag = stream.ReadDWORD(2);
		if (casFlag == 0) pNew->m_cCASFlag = 'Y';
		if (casFlag == 1) pNew->m_cCASFlag = 'N';
		if (casFlag == 2) pNew->m_cCASFlag = '\0';

		UINT32  vcmFlag = stream.ReadDWORD(2);
		if (vcmFlag == 0) pNew->m_cVCMFlag = 'Y';
		if (vcmFlag == 1) pNew->m_cVCMFlag = 'N';
		if (vcmFlag == 2) pNew->m_cVCMFlag = '\0';

		UINT32  shortSellFlag = stream.ReadDWORD(2);
		if (shortSellFlag == 0) pNew->m_cShortSellFlag = 'Y';
		if (shortSellFlag == 1) pNew->m_cShortSellFlag = 'N';
		if (shortSellFlag == 2) pNew->m_cShortSellFlag = '\0';

		UINT32  ccassFlag = stream.ReadDWORD(2);
		if (ccassFlag == 0) pNew->m_cCCASSFlag = 'Y';
		if (ccassFlag == 1) pNew->m_cCCASSFlag = 'N';
		if (ccassFlag == 2) pNew->m_cCCASSFlag = '\0';

		UINT32  dummySecurityFlag = stream.ReadDWORD(2);
		if (dummySecurityFlag == 0) pNew->m_cDummySecurityFlag = 'Y';
		if (dummySecurityFlag == 1) pNew->m_cDummySecurityFlag = 'N';
		if (dummySecurityFlag == 2) pNew->m_cDummySecurityFlag = '\0';

		UINT32  stampDutyFlag = stream.ReadDWORD(2);
		if (stampDutyFlag == 0) pNew->m_cStampDutyFlag = 'Y';
		if (stampDutyFlag == 1) pNew->m_cStampDutyFlag = 'N';
		if (stampDutyFlag == 2) pNew->m_cStampDutyFlag = '\0';

		UINT32  efnFlag = stream.ReadDWORD(2);
		if (efnFlag == 0) pNew->m_cEFNFalg = 'Y';
		if (efnFlag == 1) pNew->m_cEFNFalg = 'N';
		if (efnFlag == 2) pNew->m_cEFNFalg = '\0';

		UINT32  callPutFlag = stream.ReadDWORD(3);
		if (callPutFlag == 0) pNew->m_cCallPutFlag = 'Y';
		if (callPutFlag == 1) pNew->m_cCallPutFlag = 'N';
		if (callPutFlag == 2) pNew->m_cCallPutFlag = 'P';
		if (callPutFlag == 3) pNew->m_cCallPutFlag = 'C';
		if (callPutFlag == 4) pNew->m_cCallPutFlag = '\0';

		UINT32  style = stream.ReadDWORD(2);
		if (style == 0) pNew->m_cStyle = 'A';
		if (style == 1) pNew->m_cStyle = 'E';
		if (style == 2) pNew->m_cStyle = '\0';

		bool blnFuller = stream.ReadDWORD(1);
		if (blnFuller == false) {
			UINT32 filler = stream.ReadDWORD(32);
			memcpy(pNew->m_cFiller, &filler, 4);
		}
		else {
			*pNew->m_cFiller = '\0';
		}

		UINT32 underlyingCodeLength = stream.ReadDWORD(4);
		UINT32 numWhole = underlyingCodeLength / 4;
		if (numWhole != 0) {
			for (int k = 0; k < numWhole; ++k) {
				UINT32 underlyingCode = stream.ReadDWORD(32);
				memcpy(pNew->m_pchUnderlyingCode + k * 4, &underlyingCode, 4);
			}
		}
		UINT32 leftLength = underlyingCodeLength % 4;
		if (leftLength != 0) {
			UINT32 leftCode = stream.ReadDWORD(leftLength * 8);
			memcpy(pNew->m_pchUnderlyingCode + numWhole * 4, &leftCode, leftLength);
		}

		pNew->m_dwLotSize = stream.ReadDWORD(18);
		stream.DecodeData(pNew->m_dwListingDate, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.DecodeData(pNew->m_dwDelistingDate, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.DecodeData(pNew->m_dwCouponRate, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.DecodeData(pNew->m_dwAccruedInt, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		UINT32 dwTmp = 0;
		stream.DecodeData(dwTmp, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		pNew->m_dwYield = dwTmp;
		stream.DecodeData(pNew->m_dwConversionRatio, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.DecodeData(pNew->m_dwStrikePrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.DecodeData(pNew->m_dwMaturityDate, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		pNew->m_cUnderlyingMarket = stream.ReadDWORD(8);

		UINT32 IEPrice;
		stream.DecodeData(IEPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		memcpy(&pNew->m_dwIEPrice, &IEPrice, sizeof IEPrice);
		stream.DecodeBigInt(pNew->m_xIEVolume, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	}
	else {
		pNew->m_bTradingStatus = stream.ReadDWORD(2); // 0, 2, 3
		DecodeArray(stream, pNew->m_cCurrencyCode);
		pNew->m_dwLotSize = stream.ReadDWORD(20); // 1M is enough
		DecodeArray(stream, pNew->m_cSpreadTable);
		pNew->m_cCASFlag = stream.ReadDWORD(8);
		pNew->m_cVCMFlag = stream.ReadDWORD(8);
		pNew->m_cShortSellFlag = stream.ReadDWORD(8);
		pNew->m_cCCASSFlag = stream.ReadDWORD(8);
		pNew->m_cDummySecurityFlag = stream.ReadDWORD(8);
		pNew->m_cStampDutyFlag = stream.ReadDWORD(8);
		pNew->m_dwListingDate = stream.ReadDWORD(DATE_YYYYMMDD_BITS);
		pNew->m_dwDelistingDate = stream.ReadDWORD(DATE_YYYYMMDD_BITS);
		DecodeArray(stream, pNew->m_cFiller);
		pNew->m_cEFNFalg = stream.ReadDWORD(8);

		stream.DecodeData(pNew->m_dwCouponRate, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.DecodeData(pNew->m_dwAccruedInt, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		pNew->m_dwYield = DecodeUINT32(BC_DYNA_PRICE, BCNUM_DYNA_PRICE, stream);
		pNew->m_cCallPutFlag = stream.ReadDWORD(8);
		stream.DecodeData(pNew->m_dwConversionRatio, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.DecodeData(pNew->m_dwStrikePrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		pNew->m_dwMaturityDate = stream.ReadDWORD(DATE_YYYYMMDD_BITS);
		pNew->m_cUnderlyingMarket = stream.ReadDWORD(8);
		DecodeArray(stream, pNew->m_pchUnderlyingCode);
		pNew->m_cStyle = stream.ReadDWORD(8);

		pNew->m_dwIEPrice = DecodeUINT32(BC_DYNA_PRICE, BCNUM_DYNA_PRICE, stream);

		stream.DecodeBigInt(pNew->m_xIEVolume, BC_DYNA_VOL, BCNUM_DYNA_VOL);
	}
	return 0;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheCAS* pNew, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		UINT32 orderImbalanceDirection = stream.ReadDWORD(3);
		if (orderImbalanceDirection == 0) pNew->m_cOrderImbalanceDirection = 'N';
		if (orderImbalanceDirection == 1) pNew->m_cOrderImbalanceDirection = 'B';
		if (orderImbalanceDirection == 2) pNew->m_cOrderImbalanceDirection = 'S';
		if (orderImbalanceDirection == 3) pNew->m_cOrderImbalanceDirection = ' ';
		if (orderImbalanceDirection == 4) pNew->m_cOrderImbalanceDirection = '\0';
		bool blnReadQuantity = stream.ReadDWORD(1);
		if (blnReadQuantity == false) {
			stream.DecodeBigInt(pNew->m_xOrderImbalanceQuantity, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
		}
		else {
			pNew->m_xOrderImbalanceQuantity = 0;
		}
		UINT32 referencePrice;
		UINT32 lowerPrice;
		UINT32 upperPrice;
		stream.DecodeData(referencePrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.DecodeData(lowerPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		stream.DecodeData(upperPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
		memcpy(&pNew->m_dwReferencePrice, &referencePrice, sizeof referencePrice);
		memcpy(&pNew->m_dwLowerPrice, &lowerPrice, sizeof lowerPrice);
		memcpy(&pNew->m_xUpperPrice, &upperPrice, sizeof upperPrice);

	}
	else {
		pNew->m_cOrderImbalanceDirection = stream.ReadDWORD(8);

		stream.DecodeBigInt(pNew->m_xOrderImbalanceQuantity,
			BC_DYNA_VOL,
			BCNUM_DYNA_VOL);

		pNew->m_dwReferencePrice = DecodeUINT32(BC_DYNA_PRICE,
			BCNUM_DYNA_PRICE,
			stream);

		pNew->m_dwLowerPrice = DecodeUINT32(BC_DYNA_PRICE,
			BCNUM_DYNA_PRICE,
			stream);

		pNew->m_xUpperPrice = DecodeUINT32(BC_DYNA_PRICE,
			BCNUM_DYNA_PRICE,
			stream);
	}
	return 0;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheVCM* pNew, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		bool blnreadVCM = stream.ReadDWORD(1);
		if (blnreadVCM == true) {
			pNew->m_dwVCMDate = 0;
			pNew->m_dwVCMStartTime = 0;
			pNew->m_dwVCMEndTime = 0;
			pNew->m_dwVCMReferencePrice = 0;
			pNew->m_xVCMLowerPrice = 0;
			pNew->m_xVCMUpperPrice = 0;
		}
		else {
			stream.DecodeData(pNew->m_dwVCMDate, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			stream.DecodeData(pNew->m_dwVCMStartTime, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			stream.DecodeData(pNew->m_dwVCMEndTime, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			UINT32 referencePrice;
			UINT32 lowerPrice;
			UINT32 upperPrice;
			stream.DecodeData(referencePrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			stream.DecodeData(lowerPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			stream.DecodeData(upperPrice, BC_DYNA_PRICE, BCNUM_DYNA_PRICE);
			memcpy(&pNew->m_dwVCMReferencePrice, &referencePrice, sizeof referencePrice);
			memcpy(&pNew->m_xVCMLowerPrice, &lowerPrice, sizeof lowerPrice);
			memcpy(&pNew->m_xVCMUpperPrice, &upperPrice, sizeof upperPrice);
		}
	}
	else {
		pNew->m_dwVCMDate = stream.ReadDWORD(DATE_YYYYMMDD_BITS);
		pNew->m_dwVCMStartTime = stream.ReadDWORD(TIME_HHMMSS_BITS);
		pNew->m_dwVCMEndTime = stream.ReadDWORD(TIME_HHMMSS_BITS);

		pNew->m_dwVCMReferencePrice = DecodeUINT32(BC_DYNA_PRICE,
			BCNUM_DYNA_PRICE,
			stream);

		pNew->m_xVCMLowerPrice = DecodeUINT32(BC_DYNA_PRICE,
			BCNUM_DYNA_PRICE,
			stream);

		pNew->m_xVCMUpperPrice = DecodeUINT32(BC_DYNA_PRICE,
			BCNUM_DYNA_PRICE,
			stream);
	}
	return 0;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheBlockQt* pNew, const CStdCacheBlockQt* pOld, const UINT16 wVer)
{
	if (pNew->m_dwTradeSequence == pOld->m_dwTradeSequence)
	{
		stream.WriteBool(false);
	}
	else
	{
		stream.WriteBool(true);
		stream.WriteDWORD(pNew->m_dwTradeSequence, 26);
	}

	if (pNew->m_dwDate == pOld->m_dwDate)
	{
		stream.WriteBool(false);
	}
	else
	{
		stream.WriteBool(true);
		stream.WriteDWORD(pNew->m_dwDate, 26);
	}

	UINT32 dwSecondsDiff = GetSecondsDiff(pOld->m_dwTime, pNew->m_dwTime);
	stream.EncodeData(dwSecondsDiff, BC_DYNA_TIMETDIFF, BCNUM_DYNA_TIMETDIFF);
	EncodeDataDiff(pNew->m_dwClose, pOld->m_dwClose, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwOpen, pOld->m_dwOpen, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwHigh, pOld->m_dwHigh, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwLow, pOld->m_dwLow, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwPrice, pOld->m_dwPrice, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeBigintDiff(pNew->m_xVolume, pOld->m_xVolume, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	EncodeBigintDiff(pNew->m_xAmount, pOld->m_xAmount, BC_DYNA_AMNT, BCNUM_DYNA_AMNT, stream);

	if (pNew->m_pchTopStockMarketID == pOld->m_pchTopStockMarketID)
	{
		stream.WriteBool(0);
	}
	else
	{
		stream.WriteBool(1);
		int nMarket = pNew->m_pchTopStockMarketID;
		stream.WriteDWORD(nMarket, 8);
	}

	EncodeDataDiff(pNew->m_dwStockNum, pOld->m_dwStockNum, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwUpNum, pOld->m_dwUpNum, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwDownNum, pOld->m_dwDownNum, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwStrongNum, pOld->m_dwStrongNum, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwWeakNum, pOld->m_dwWeakNum, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

	EncodeBigintDiff(pNew->m_xZGB, pOld->m_xZGB, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	EncodeBigintDiff(pNew->m_xZSZ, pOld->m_xZSZ, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	EncodeBigintDiff(pNew->m_xAvgProfit, pOld->m_xAvgProfit, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	EncodeBigintDiff(pNew->m_xPeRatio, pOld->m_xPeRatio, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);

	EncodeBigintDiff(pNew->m_xAvgAmount2Day, pOld->m_xAvgAmount2Day, BC_DYNA_AMNT, BCNUM_DYNA_AMNT, stream);
	EncodeBigintDiff(pNew->m_xAvgAmount20Day, pOld->m_xAvgAmount20Day, BC_DYNA_AMNT, BCNUM_DYNA_AMNT, stream);
	EncodeBigintDiff(pNew->m_xTurnover2Day, pOld->m_xTurnover2Day, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	EncodeBigintDiff(pNew->m_xTurnover20Day, pOld->m_xTurnover20Day, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	EncodeBigintDiff(pNew->m_xPercent3Day, pOld->m_xPercent3Day, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	EncodeBigintDiff(pNew->m_xPercent20Day, pOld->m_xPercent20Day, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);

	EncodeDataDiff(pNew->m_dwPre5MinPrice[0], pOld->m_dwPre5MinPrice[0], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwPre5MinPrice[1], pOld->m_dwPre5MinPrice[1], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwPre5MinPrice[2], pOld->m_dwPre5MinPrice[2], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwPre5MinPrice[3], pOld->m_dwPre5MinPrice[3], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	EncodeDataDiff(pNew->m_dwPre5MinPrice[4], pOld->m_dwPre5MinPrice[4], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

	return 0;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheBlockOverView* pNew, const CStdCacheBlockOverView* pOld, const UINT16 version) {
    if (version == MCM_DEFAULT_OLD_VERSION) {
        EncodeBigintDiff(pNew->m_xNetFlow, pOld->m_xNetFlow, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
        if (pNew->m_dwPercent == pOld->m_dwPercent) {
            stream.WriteBool(0);
        } else {
            stream.WriteBool(1);
            if (pNew->m_dwPercent > 0) {
                stream.WriteBool(0);
                stream.WriteDWORD(pNew->m_dwPercent, 12);
            } else {
                stream.WriteBool(1);
                int tmp = 0 - pNew->m_dwPercent;
                stream.WriteDWORD(tmp, 12);
            }
        }
        EncodeDataDiff(pNew->m_dwUpDay, pOld->m_dwUpDay, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
        if (pNew->m_pchTopStockMarketID == pOld->m_pchTopStockMarketID) {
            stream.WriteBool(0);
        } else {
            stream.WriteBool(1);
            int nMarket = pNew->m_pchTopStockMarketID;
            stream.WriteDWORD(nMarket, 8);
        }

        if (pNew->m_dwTopPercent == pOld->m_dwTopPercent) {
            stream.WriteBool(0);
        } else {
            stream.WriteBool(1);
            if (pNew->m_dwTopPercent > 0) {
                stream.WriteBool(0);
                stream.WriteDWORD(pNew->m_dwTopPercent, 12);
            } else {
                stream.WriteBool(1);
                int tmp = 0 - pNew->m_dwTopPercent;
                stream.WriteDWORD(tmp, 12);
            }
        }
    } else if (version == MCM_VERSION) {
        stream.EncodeData(pNew->m_wType, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, (const unsigned int*)&pOld->m_wType);
        stream.EncodeBigInt(pNew->m_xNetFlow, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &pOld->m_xNetFlow);
        stream.encodeData(pNew->m_dwPercent, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &pOld->m_dwPercent);
        stream.EncodeData(pNew->m_dwUpDay, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &pOld->m_dwUpDay);
        stream.encodeData(pNew->m_dwTopPercent, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &pOld->m_dwTopPercent);
    }
	return 0;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheBrokerTrace* pNew, const CStdCacheBrokerTrace* pOld, const UINT16 wVer)
{

	return 0;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheQtEx* pNew, const CStdCacheQtEx* pOld, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		unsigned int tradingStatus = pNew->m_bTradingStatus;
		stream.WriteDWORD(tradingStatus, 2);

		UINT32 currencyCode = 0;
		if (strncmp(pNew->m_cCurrencyCode, "HKD", 3) == 0)      currencyCode = 0;
		else if (strncmp(pNew->m_cCurrencyCode, "USD", 3) == 0) currencyCode = 1;
		else if (strncmp(pNew->m_cCurrencyCode, "EUR", 3) == 0) currencyCode = 2;
		else if (strncmp(pNew->m_cCurrencyCode, "JPY", 3) == 0) currencyCode = 3;
		else if (strncmp(pNew->m_cCurrencyCode, "GBP", 3) == 0) currencyCode = 4;
		else if (strncmp(pNew->m_cCurrencyCode, "CAD", 3) == 0) currencyCode = 5;
		else if (strncmp(pNew->m_cCurrencyCode, "SGD", 3) == 0) currencyCode = 6;
		else if (strncmp(pNew->m_cCurrencyCode, "CNY", 3) == 0) currencyCode = 7;
		else if (strncmp(pNew->m_cCurrencyCode, "AUD", 3) == 0) currencyCode = 8;
		else                                                  currencyCode = 9;
		stream.WriteDWORD(currencyCode, 4);

		UINT32 spreadTable = 0;
		if (strncmp(pNew->m_cSpreadTable, "01", 2) == 0) spreadTable = 0;
		else if (strncmp(pNew->m_cSpreadTable, "03", 2) == 0) spreadTable = 1;
		stream.WriteDWORD(spreadTable, 1);

		UINT32 casFlag = 0;
		if (pNew->m_cCASFlag == 'Y') casFlag = 0;
		if (pNew->m_cCASFlag == 'N') casFlag = 1;
		if (pNew->m_cCASFlag == '\0') casFlag = 2;
		stream.WriteDWORD(casFlag, 2);

		UINT32 vcmFlag = 0;
		if (pNew->m_cVCMFlag == 'Y') vcmFlag = 0;
		if (pNew->m_cVCMFlag == 'N') vcmFlag = 1;
		if (pNew->m_cVCMFlag == '\0') vcmFlag = 2;
		stream.WriteDWORD(vcmFlag, 2);

		UINT32 shortSellFlag = 0;
		if (pNew->m_cShortSellFlag == 'Y') shortSellFlag = 0;
		if (pNew->m_cShortSellFlag == 'N') shortSellFlag = 1;
		if (pNew->m_cShortSellFlag == '\0') shortSellFlag = 2;
		stream.WriteDWORD(shortSellFlag, 2);

		UINT32 ccassFlag = 0;
		if (pNew->m_cCCASSFlag == 'Y') ccassFlag = 0;
		if (pNew->m_cCCASSFlag == 'N') ccassFlag = 1;
		if (pNew->m_cCCASSFlag == '\0') ccassFlag = 2;
		stream.WriteDWORD(ccassFlag, 2);

		UINT32 dummySecurityFlag = 0;
		if (pNew->m_cDummySecurityFlag == 'Y') dummySecurityFlag = 0;
		if (pNew->m_cDummySecurityFlag == 'N') dummySecurityFlag = 1;
		if (pNew->m_cDummySecurityFlag == '\0') dummySecurityFlag = 2;
		stream.WriteDWORD(dummySecurityFlag, 2);

		UINT32 stampDutyFlag = 0;
		if (pNew->m_cStampDutyFlag == 'Y') stampDutyFlag = 0;
		if (pNew->m_cStampDutyFlag == 'N') stampDutyFlag = 1;
		if (pNew->m_cStampDutyFlag == '\0') stampDutyFlag = 2;
		stream.WriteDWORD(stampDutyFlag, 2);

		UINT32 efnFalg = 0;
		if (pNew->m_cEFNFalg == 'Y') efnFalg = 0;
		if (pNew->m_cEFNFalg == 'N') efnFalg = 1;
		if (pNew->m_cEFNFalg == '\0') efnFalg = 2;
		stream.WriteDWORD(efnFalg, 2);

		UINT32 callPutFlag = 0;
		if (pNew->m_cCallPutFlag == 'Y') callPutFlag = 0;
		if (pNew->m_cCallPutFlag == 'N') callPutFlag = 1;
		if (pNew->m_cCallPutFlag == 'P') callPutFlag = 2;
		if (pNew->m_cCallPutFlag == 'C') callPutFlag = 3;
		if (pNew->m_cCallPutFlag == '\0') callPutFlag = 4;
		stream.WriteDWORD(callPutFlag, 3);

		UINT32 style = 0;
		if (pNew->m_cStyle == 'A') style = 0;
		if (pNew->m_cStyle == 'E') style = 1;
		if (pNew->m_cStyle == '\0') style = 2;
		stream.WriteDWORD(style, 2);

		if (*pNew->m_cFiller != '\0') {
			stream.WriteBool(false);
			UINT32 filler = 0;
			memcpy(&filler, pNew->m_cFiller, 4);
			stream.WriteDWORD(filler, 32);
		}
		else {
			stream.WriteBool(true);
		}
		// Usually m_pchUnderlyingCode in pNew is equal to that in pOld, so we set a flag here for compression efficiency.
		if (strncmp(pNew->m_pchUnderlyingCode, pOld->m_pchUnderlyingCode, sizeof pNew->m_pchUnderlyingCode) == 0) {
			stream.WriteBool(true);
		}
		else {
			stream.WriteBool(false);
			UINT32 underlyingCodeLength = strlen(pNew->m_pchUnderlyingCode);
			stream.WriteDWORD(underlyingCodeLength, 4);
			UINT32 numWhole = underlyingCodeLength / 4;
			if (numWhole != 0) {
				UINT32 *underlyingCode = new UINT32[numWhole];
				memcpy(underlyingCode, pNew->m_pchUnderlyingCode, numWhole * 4);
				for (int k = 0; k < numWhole; ++k)
					stream.WriteDWORD(underlyingCode[k], 32);
				if (underlyingCode != NULL) delete[] underlyingCode;
			}
			UINT32 leftLength = underlyingCodeLength % 4;
			if (leftLength != 0) {
				UINT32 leftCode;
				memcpy(&leftCode, pNew->m_pchUnderlyingCode + numWhole * 4, leftLength);
				stream.WriteDWORD(leftCode, leftLength * 8);
			}
		}

		EncodeDataDiff(pNew->m_dwLotSize, pOld->m_dwLotSize, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

		EncodeDataDiff(pNew->m_dwListingDate, pOld->m_dwListingDate, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		EncodeDataDiff(pNew->m_dwDelistingDate, pOld->m_dwDelistingDate, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

		EncodeDataDiff(pNew->m_dwCouponRate, pOld->m_dwCouponRate, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		EncodeDataDiff(pNew->m_dwAccruedInt, pOld->m_dwAccruedInt, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		EncodeDataDiff(pNew->m_dwYield, pOld->m_dwYield, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

		EncodeDataDiff(pNew->m_dwConversionRatio, pOld->m_dwConversionRatio, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		EncodeDataDiff(pNew->m_dwStrikePrice, pOld->m_dwStrikePrice, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		EncodeDataDiff(pNew->m_dwMaturityDate, pOld->m_dwMaturityDate, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		stream.WriteDWORD(pNew->m_cUnderlyingMarket, 8);

		UINT32 IEPriceOld;
		UINT32 IEPriceNew;
		memcpy(&IEPriceOld, &pOld->m_dwIEPrice, sizeof IEPriceOld);
		memcpy(&IEPriceNew, &pNew->m_dwIEPrice, sizeof IEPriceNew);
		EncodeDataDiff(IEPriceNew, IEPriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

		EncodeBigintDiff(pNew->m_xIEVolume, pOld->m_xIEVolume, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	}
	else {
		stream.WriteDWORD(pNew->m_bTradingStatus, 2); // 0, 2, 3
		EncodeArrayDiff(stream, pNew->m_cCurrencyCode, pOld->m_cCurrencyCode);
		EncodeDWORDDiff(stream, pNew->m_dwLotSize, pOld->m_dwLotSize, 20);
		EncodeArrayDiff(stream, pNew->m_cSpreadTable, pOld->m_cSpreadTable);
		stream.WriteDWORD(pNew->m_cCASFlag, 8);
		stream.WriteDWORD(pNew->m_cVCMFlag, 8);
		stream.WriteDWORD(pNew->m_cShortSellFlag, 8);
		stream.WriteDWORD(pNew->m_cCCASSFlag, 8);
		stream.WriteDWORD(pNew->m_cDummySecurityFlag, 8);
		stream.WriteDWORD(pNew->m_cStampDutyFlag, 8);
		EncodeDWORDDiff(stream, pNew->m_dwListingDate, pOld->m_dwListingDate, DATE_YYYYMMDD_BITS);
		EncodeDWORDDiff(stream, pNew->m_dwDelistingDate, pOld->m_dwDelistingDate, DATE_YYYYMMDD_BITS);
		EncodeArrayDiff(stream, pNew->m_cFiller, pOld->m_cFiller);
		stream.WriteDWORD(pNew->m_cEFNFalg, 8);
		EncodeDataDiff(pNew->m_dwCouponRate, pOld->m_dwCouponRate, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, stream);
		EncodeDataDiff(pNew->m_dwAccruedInt, pOld->m_dwAccruedInt, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, stream);
		EncodeDataDiff(pNew->m_dwYield, pOld->m_dwYield, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, stream);
		stream.WriteDWORD(pNew->m_cCallPutFlag, 8);
		EncodeDataDiff(pNew->m_dwConversionRatio, pOld->m_dwConversionRatio, BC_DYNA_PRICE_DIFF, BCNUM_DYNA_PRICE_DIFF, stream);
		EncodeDataDiff(pNew->m_dwStrikePrice, pOld->m_dwStrikePrice, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		EncodeDWORDDiff(stream, pNew->m_dwMaturityDate, pOld->m_dwMaturityDate, DATE_YYYYMMDD_BITS);
		stream.WriteDWORD(pNew->m_cUnderlyingMarket, 8);
		EncodeArrayDiff(stream, pNew->m_pchUnderlyingCode, pOld->m_pchUnderlyingCode);
		stream.WriteDWORD(pNew->m_cStyle, 8);
		EncodeDataDiff(pNew->m_dwIEPrice, pOld->m_dwIEPrice, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		EncodeBigintDiff(pNew->m_xIEVolume, pOld->m_xIEVolume, BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF, stream);
	}
	return 0;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheCAS* pNew, const CStdCacheCAS* pOld, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		UINT32 orderImbalanceDirection = 0;
		if (pNew->m_cOrderImbalanceDirection == 'N') orderImbalanceDirection = 0;
		if (pNew->m_cOrderImbalanceDirection == 'B') orderImbalanceDirection = 1;
		if (pNew->m_cOrderImbalanceDirection == 'S') orderImbalanceDirection = 2;
		if (pNew->m_cOrderImbalanceDirection == ' ') orderImbalanceDirection = 3;
		if (pNew->m_cOrderImbalanceDirection == '\0') orderImbalanceDirection = 4;
		stream.WriteDWORD(orderImbalanceDirection, 3);
		if (pNew->m_cOrderImbalanceDirection == ' ') {
			stream.WriteBool(true);
		}
		else {
			stream.WriteBool(false);
			EncodeBigintDiff(pNew->m_xOrderImbalanceQuantity, pOld->m_xOrderImbalanceQuantity, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
		}
		UINT32 referencePriceOld;
		UINT32 referencePriceNew;
		UINT32 lowerPriceOld;
		UINT32 lowerPriceNew;
		UINT32 upperPriceOld;
		UINT32 upperPriceNew;
		memcpy(&referencePriceOld, &pOld->m_dwReferencePrice, sizeof referencePriceOld);
		memcpy(&referencePriceNew, &pNew->m_dwReferencePrice, sizeof referencePriceNew);
		memcpy(&lowerPriceOld, &pOld->m_dwLowerPrice, sizeof lowerPriceOld);
		memcpy(&lowerPriceNew, &pNew->m_dwLowerPrice, sizeof lowerPriceNew);
		memcpy(&upperPriceOld, &pOld->m_xUpperPrice, sizeof upperPriceOld);
		memcpy(&upperPriceNew, &pNew->m_xUpperPrice, sizeof upperPriceNew);
		EncodeDataDiff(referencePriceNew, referencePriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		EncodeDataDiff(lowerPriceNew, lowerPriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		EncodeDataDiff(upperPriceNew, upperPriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	}
	else {
		stream.WriteDWORD(pNew->m_cOrderImbalanceDirection, 8);

		EncodeBigintDiff(pNew->m_xOrderImbalanceQuantity,
			pOld->m_xOrderImbalanceQuantity,
			BC_DYNA_VOL_DIFF,
			BCNUM_DYNA_VOL_DIFF,
			stream);

		EncodeDataDiff(pNew->m_dwReferencePrice,
			pOld->m_dwReferencePrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);

		EncodeDataDiff(pNew->m_dwLowerPrice,
			pOld->m_dwLowerPrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);

		EncodeDataDiff(pNew->m_xUpperPrice,
			pOld->m_xUpperPrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);
	}
	return 0;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheVCM* pNew, const CStdCacheVCM* pOld, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		if (pNew->m_dwVCMDate == 0 && pNew->m_dwVCMStartTime == 0 && pNew->m_dwVCMEndTime == 0 && pNew->m_dwVCMReferencePrice == 0 && pNew->m_xVCMLowerPrice == 0 && pNew->m_xVCMUpperPrice == 0) {
			stream.WriteBool(true);
		}
		else {
			stream.WriteBool(false);
			EncodeDataDiff(pNew->m_dwVCMDate, pOld->m_dwVCMDate, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
			EncodeDataDiff(pNew->m_dwVCMStartTime, pOld->m_dwVCMStartTime, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
			EncodeDataDiff(pNew->m_dwVCMEndTime, pOld->m_dwVCMEndTime, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
			UINT32 referencePriceOld;
			UINT32 referencePriceNew;
			UINT32 lowerPriceOld;
			UINT32 lowerPriceNew;
			UINT32 upperPriceOld;
			UINT32 upperPriceNew;
			memcpy(&referencePriceNew, &pNew->m_dwVCMReferencePrice, sizeof referencePriceNew);
			memcpy(&referencePriceOld, &pOld->m_dwVCMReferencePrice, sizeof referencePriceOld);
			memcpy(&lowerPriceNew, &pNew->m_xVCMLowerPrice, sizeof lowerPriceNew);
			memcpy(&lowerPriceOld, &pOld->m_xVCMLowerPrice, sizeof lowerPriceOld);
			memcpy(&upperPriceNew, &pNew->m_xVCMUpperPrice, sizeof upperPriceNew);
			memcpy(&upperPriceOld, &pOld->m_xVCMUpperPrice, sizeof upperPriceOld);
			EncodeDataDiff(referencePriceNew, referencePriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
			EncodeDataDiff(lowerPriceNew, lowerPriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
			EncodeDataDiff(upperPriceNew, upperPriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		}
	}
	else {
		EncodeDWORDDiff(stream, pNew->m_dwVCMDate, pOld->m_dwVCMDate, DATE_YYYYMMDD_BITS);
		EncodeDWORDDiff(stream, pNew->m_dwVCMStartTime, pOld->m_dwVCMStartTime, TIME_HHMMSS_BITS);
		EncodeDWORDDiff(stream, pNew->m_dwVCMEndTime, pOld->m_dwVCMEndTime, TIME_HHMMSS_BITS);

		EncodeDataDiff(pNew->m_dwVCMReferencePrice,
			pOld->m_dwVCMReferencePrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);

		EncodeDataDiff(pNew->m_xVCMLowerPrice,
			pOld->m_xVCMLowerPrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);

		EncodeDataDiff(pNew->m_xVCMUpperPrice,
			pOld->m_xVCMUpperPrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);
	}
	return 0;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheBlockQt* pNew, const CStdCacheBlockQt* pOld, const UINT16 wVer)
{
	if (stream.ReadDWORD(1))
	{
		pNew->m_dwTradeSequence = stream.ReadDWORD(26);
	}

	if (stream.ReadDWORD(1))
	{
		pNew->m_dwDate = stream.ReadDWORD(26);
	}

	UINT32 dwSecondsDiff = 0;
	stream.DecodeData(dwSecondsDiff, BC_DYNA_TIMETDIFF, BCNUM_DYNA_TIMETDIFF);
	pNew->m_dwTime = AddSeconds(pOld->m_dwTime, dwSecondsDiff);
	DecodeDataDiff(pNew->m_dwClose, pOld->m_dwClose, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwOpen, pOld->m_dwOpen, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwHigh, pOld->m_dwHigh, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwLow, pOld->m_dwLow, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwPrice, pOld->m_dwPrice, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeBigintDiff(pNew->m_xVolume, pOld->m_xVolume, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	DecodeBigintDiff(pNew->m_xAmount, pOld->m_xAmount, BC_DYNA_AMNT, BCNUM_DYNA_AMNT, stream);

	if (stream.ReadDWORD(1))
	{
		pNew->m_pchTopStockMarketID = stream.ReadDWORD(8);
	}

	DecodeDataDiff(pNew->m_dwStockNum, pOld->m_dwStockNum, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwUpNum, pOld->m_dwUpNum, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwDownNum, pOld->m_dwDownNum, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwStrongNum, pOld->m_dwStrongNum, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwWeakNum, pOld->m_dwWeakNum, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

	DecodeBigintDiff(pNew->m_xZGB, pOld->m_xZGB, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	DecodeBigintDiff(pNew->m_xZSZ, pOld->m_xZSZ, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	DecodeBigintDiff(pNew->m_xAvgProfit, pOld->m_xAvgProfit, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	DecodeBigintDiff(pNew->m_xPeRatio, pOld->m_xPeRatio, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);

	DecodeBigintDiff(pNew->m_xAvgAmount2Day, pOld->m_xAvgAmount2Day, BC_DYNA_AMNT, BCNUM_DYNA_AMNT, stream);
	DecodeBigintDiff(pNew->m_xAvgAmount20Day, pOld->m_xAvgAmount20Day, BC_DYNA_AMNT, BCNUM_DYNA_AMNT, stream);
	DecodeBigintDiff(pNew->m_xTurnover2Day, pOld->m_xTurnover2Day, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	DecodeBigintDiff(pNew->m_xTurnover20Day, pOld->m_xTurnover20Day, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	DecodeBigintDiff(pNew->m_xPercent3Day, pOld->m_xPercent3Day, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	DecodeBigintDiff(pNew->m_xPercent20Day, pOld->m_xPercent20Day, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);

	DecodeDataDiff(pNew->m_dwPre5MinPrice[0], pOld->m_dwPre5MinPrice[0], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwPre5MinPrice[1], pOld->m_dwPre5MinPrice[1], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwPre5MinPrice[2], pOld->m_dwPre5MinPrice[2], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwPre5MinPrice[3], pOld->m_dwPre5MinPrice[3], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
	DecodeDataDiff(pNew->m_dwPre5MinPrice[4], pOld->m_dwPre5MinPrice[4], BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

	return 0;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheBlockOverView* pNew, const CStdCacheBlockOverView* pOld, const UINT16 version) {
    if (version == MCM_DEFAULT_OLD_VERSION) {
        DecodeBigintDiff(pNew->m_xNetFlow, pOld->m_xNetFlow, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
        if (stream.ReadDWORD(1)) {
            if (stream.ReadDWORD(1)) {
                int tmp = stream.ReadDWORD(12);
                pNew->m_dwPercent = 0 - tmp;
            } else {
                pNew->m_dwPercent = stream.ReadDWORD(12);
            }
        }

        DecodeDataDiff(pNew->m_dwUpDay, pOld->m_dwUpDay, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

        if (stream.ReadDWORD(1)) {
            pNew->m_pchTopStockMarketID = stream.ReadDWORD(8);
        }

        if (stream.ReadDWORD(1)) {
            if (stream.ReadDWORD(1)) {
                int tmp = stream.ReadDWORD(12);
                pNew->m_dwTopPercent = 0 - tmp;
            } else {
                pNew->m_dwTopPercent = stream.ReadDWORD(12);
            }
        }
    } else if (version == MCM_VERSION) {
        unsigned int type = 0;
        stream.DecodeData(type, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, (const unsigned int*)&pOld->m_wType);
        pNew->m_wType = type;

        stream.DecodeBigInt(pNew->m_xNetFlow, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &pOld->m_xNetFlow);
        pNew->m_dwPercent = stream.decodeData(GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &pOld->m_dwPercent);
        stream.DecodeData(pNew->m_dwUpDay, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &pOld->m_dwUpDay);
        pNew->m_dwTopPercent = stream.decodeData(GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &pOld->m_dwTopPercent);
    }

	return 0;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheBrokerTrace* pNew, const CStdCacheBrokerTrace* pOld, const UINT16 wVer)
{
	return 0;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheQtEx* pNew, const CStdCacheQtEx* pOld, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		unsigned int tradingStatus = stream.ReadDWORD(2);
		pNew->m_bTradingStatus = tradingStatus;

		UINT32 currencyCode = stream.ReadDWORD(4);
		if (currencyCode == 0)      strncpy(pNew->m_cCurrencyCode, "HKD", 3);
		else if (currencyCode == 1) strncpy(pNew->m_cCurrencyCode, "USD", 3);
		else if (currencyCode == 2) strncpy(pNew->m_cCurrencyCode, "EUR", 3);
		else if (currencyCode == 3) strncpy(pNew->m_cCurrencyCode, "JPY", 3);
		else if (currencyCode == 4) strncpy(pNew->m_cCurrencyCode, "GBP", 3);
		else if (currencyCode == 5) strncpy(pNew->m_cCurrencyCode, "CAD", 3);
		else if (currencyCode == 6) strncpy(pNew->m_cCurrencyCode, "SGD", 3);
		else if (currencyCode == 7) strncpy(pNew->m_cCurrencyCode, "CNY", 3);
		else if (currencyCode == 8) strncpy(pNew->m_cCurrencyCode, "AUD", 3);
		else if (currencyCode == 9) strncpy(pNew->m_cCurrencyCode, "OTHER", 4);

		UINT32 spreadTable = stream.ReadDWORD(1);
		if (spreadTable == 0) strncpy(pNew->m_cSpreadTable, "01", 2);
		if (spreadTable == 1) strncpy(pNew->m_cSpreadTable, "03", 2);

		UINT32 casFlag = stream.ReadDWORD(2);
		if (casFlag == 0) pNew->m_cCASFlag = 'Y';
		if (casFlag == 1) pNew->m_cCASFlag = 'N';
		if (casFlag == 2) pNew->m_cCASFlag = '\0';

		UINT32  vcmFlag = stream.ReadDWORD(2);
		if (vcmFlag == 0) pNew->m_cVCMFlag = 'Y';
		if (vcmFlag == 1) pNew->m_cVCMFlag = 'N';
		if (vcmFlag == 2) pNew->m_cVCMFlag = '\0';

		UINT32  shortSellFlag = stream.ReadDWORD(2);
		if (shortSellFlag == 0) pNew->m_cShortSellFlag = 'Y';
		if (shortSellFlag == 1) pNew->m_cShortSellFlag = 'N';
		if (shortSellFlag == 2) pNew->m_cShortSellFlag = '\0';

		UINT32  ccassFlag = stream.ReadDWORD(2);
		if (ccassFlag == 0) pNew->m_cCCASSFlag = 'Y';
		if (ccassFlag == 1) pNew->m_cCCASSFlag = 'N';
		if (ccassFlag == 2) pNew->m_cCCASSFlag = '\0';

		UINT32  dummySecurityFlag = stream.ReadDWORD(2);
		if (dummySecurityFlag == 0) pNew->m_cDummySecurityFlag = 'Y';
		if (dummySecurityFlag == 1) pNew->m_cDummySecurityFlag = 'N';
		if (dummySecurityFlag == 2) pNew->m_cDummySecurityFlag = '\0';

		UINT32  stampDutyFlag = stream.ReadDWORD(2);
		if (stampDutyFlag == 0) pNew->m_cStampDutyFlag = 'Y';
		if (stampDutyFlag == 1) pNew->m_cStampDutyFlag = 'N';
		if (stampDutyFlag == 2) pNew->m_cStampDutyFlag = '\0';

		UINT32  efnFlag = stream.ReadDWORD(2);
		if (efnFlag == 0) pNew->m_cEFNFalg = 'Y';
		if (efnFlag == 1) pNew->m_cEFNFalg = 'N';
		if (efnFlag == 2) pNew->m_cEFNFalg = '\0';

		UINT32  callPutFlag = stream.ReadDWORD(3);
		if (callPutFlag == 0) pNew->m_cCallPutFlag = 'Y';
		if (callPutFlag == 1) pNew->m_cCallPutFlag = 'N';
		if (callPutFlag == 2) pNew->m_cCallPutFlag = 'P';
		if (callPutFlag == 3) pNew->m_cCallPutFlag = 'C';
		if (callPutFlag == 4) pNew->m_cCallPutFlag = '\0';

		UINT32  style = stream.ReadDWORD(2);
		if (style == 0) pNew->m_cStyle = 'A';
		if (style == 1) pNew->m_cStyle = 'E';
		if (style == 2) pNew->m_cStyle = '\0';

		bool blnFuller = stream.ReadDWORD(1);
		if (blnFuller == false) {
			UINT32 filler = stream.ReadDWORD(32);
			memcpy(pNew->m_cFiller, &filler, 4);
		}
		else {
			*pNew->m_cFiller = '\0';
		}
		bool blnreadCode = stream.ReadDWORD(1);
		if (blnreadCode == true) {
			strcpy(pNew->m_pchUnderlyingCode, pOld->m_pchUnderlyingCode);
		}
		else {
			UINT32 underlyingCodeLength = stream.ReadDWORD(4);
			UINT32 numWhole = underlyingCodeLength / 4;
			if (numWhole != 0) {
				for (int k = 0; k < numWhole; ++k) {
					UINT32 underlyingCode = stream.ReadDWORD(32);
					memcpy(pNew->m_pchUnderlyingCode + k * 4, &underlyingCode, 4);
				}
			}
			UINT32 leftLength = underlyingCodeLength % 4;
			if (leftLength != 0) {
				UINT32 leftCode = stream.ReadDWORD(leftLength * 8);
				memcpy(pNew->m_pchUnderlyingCode + numWhole * 4, &leftCode, leftLength);
			}
		}
		DecodeDataDiff(pNew->m_dwLotSize, pOld->m_dwLotSize, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

		DecodeDataDiff(pNew->m_dwListingDate, pOld->m_dwListingDate, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		DecodeDataDiff(pNew->m_dwDelistingDate, pOld->m_dwDelistingDate, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);

		DecodeDataDiff(pNew->m_dwCouponRate, pOld->m_dwCouponRate, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		DecodeDataDiff(pNew->m_dwAccruedInt, pOld->m_dwAccruedInt, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		UINT dwTmp = 0;
		DecodeDataDiff(dwTmp, (UINT)pOld->m_dwYield, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		pNew->m_dwYield = dwTmp;

		DecodeDataDiff(pNew->m_dwConversionRatio, pOld->m_dwConversionRatio, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		DecodeDataDiff(pNew->m_dwStrikePrice, pOld->m_dwStrikePrice, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		DecodeDataDiff(pNew->m_dwMaturityDate, pOld->m_dwMaturityDate, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		pNew->m_cUnderlyingMarket = stream.ReadDWORD(8);

		UINT32 IEPriceOld;
		UINT32 IEPriceNew;
		memcpy(&IEPriceOld, &pOld->m_dwIEPrice, sizeof IEPriceOld);
		DecodeDataDiff(IEPriceNew, IEPriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		memcpy(&pNew->m_dwIEPrice, &IEPriceNew, sizeof IEPriceNew);

		DecodeBigintDiff(pNew->m_xIEVolume, pOld->m_xIEVolume, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
	}
	else {
		pNew->m_bTradingStatus = stream.ReadDWORD(2); // 0, 2, 3
		DecodeArrayDiff(stream, pOld->m_cCurrencyCode, pNew->m_cCurrencyCode);
		pNew->m_dwLotSize = ReadDWORDDiff(stream, pOld->m_dwLotSize, 20);
		DecodeArrayDiff(stream, pOld->m_cSpreadTable, pNew->m_cSpreadTable);
		pNew->m_cCASFlag = stream.ReadDWORD(8);
		pNew->m_cVCMFlag = stream.ReadDWORD(8);
		pNew->m_cShortSellFlag = stream.ReadDWORD(8);
		pNew->m_cCCASSFlag = stream.ReadDWORD(8);
		pNew->m_cDummySecurityFlag = stream.ReadDWORD(8);
		pNew->m_cStampDutyFlag = stream.ReadDWORD(8);
		pNew->m_dwListingDate = ReadDWORDDiff(stream, pOld->m_dwListingDate, DATE_YYYYMMDD_BITS);
		pNew->m_dwDelistingDate = ReadDWORDDiff(stream, pOld->m_dwDelistingDate, DATE_YYYYMMDD_BITS);
		DecodeArrayDiff(stream, pOld->m_cFiller, pNew->m_cFiller);
		pNew->m_cEFNFalg = stream.ReadDWORD(8);

		DecodeDataDiff(pNew->m_dwCouponRate,
			pOld->m_dwCouponRate,
			BC_DYNA_PRICE_DIFF,
			BCNUM_DYNA_PRICE_DIFF,
			stream);

		DecodeDataDiff(pNew->m_dwAccruedInt,
			pOld->m_dwAccruedInt,
			BC_DYNA_PRICE_DIFF,
			BCNUM_DYNA_PRICE_DIFF,
			stream);

		pNew->m_dwYield = DecodeUINT32Diff(pOld->m_dwYield,
			BC_DYNA_PRICE_DIFF,
			BCNUM_DYNA_PRICE_DIFF,
			stream);

		pNew->m_cCallPutFlag = stream.ReadDWORD(8);

		DecodeDataDiff(pNew->m_dwConversionRatio,
			pOld->m_dwConversionRatio,
			BC_DYNA_PRICE_DIFF,
			BCNUM_DYNA_PRICE_DIFF,
			stream);

		DecodeDataDiff(pNew->m_dwStrikePrice,
			pOld->m_dwStrikePrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);

		pNew->m_dwMaturityDate = ReadDWORDDiff(stream, pOld->m_dwMaturityDate, DATE_YYYYMMDD_BITS);
		pNew->m_cUnderlyingMarket = stream.ReadDWORD(8);

		DecodeArrayDiff(stream,
			pOld->m_pchUnderlyingCode,
			pNew->m_pchUnderlyingCode);

		pNew->m_cStyle = stream.ReadDWORD(8);

		pNew->m_dwIEPrice = DecodeUINT32Diff(pOld->m_dwIEPrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);

		DecodeBigintDiff(pNew->m_xIEVolume,
			pOld->m_xIEVolume,
			BC_DYNA_VOL_DIFF,
			BCNUM_DYNA_VOL_DIFF,
			stream);
	}
	return 0;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheCAS* pNew, const CStdCacheCAS* pOld, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		UINT32 orderImbalanceDirection = stream.ReadDWORD(3);
		if (orderImbalanceDirection == 0) pNew->m_cOrderImbalanceDirection = 'N';
		if (orderImbalanceDirection == 1) pNew->m_cOrderImbalanceDirection = 'B';
		if (orderImbalanceDirection == 2) pNew->m_cOrderImbalanceDirection = 'S';
		if (orderImbalanceDirection == 3) pNew->m_cOrderImbalanceDirection = ' ';
		if (orderImbalanceDirection == 4) pNew->m_cOrderImbalanceDirection = '\0';
		bool blnReadQuantity = stream.ReadDWORD(1);
		if (blnReadQuantity == false) {
			DecodeBigintDiff(pNew->m_xOrderImbalanceQuantity, pOld->m_xOrderImbalanceQuantity, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS, stream);
		}
		else {
			pNew->m_xOrderImbalanceQuantity = 0;
		}
		UINT32 referencePriceOld;
		UINT32 referencePriceNew;
		UINT32 lowerPriceOld;
		UINT32 lowerPriceNew;
		UINT32 upperPriceOld;
		UINT32 upperPriceNew;
		memcpy(&referencePriceOld, &pOld->m_dwReferencePrice, sizeof referencePriceOld);
		memcpy(&lowerPriceOld, &pOld->m_dwLowerPrice, sizeof lowerPriceOld);
		memcpy(&upperPriceOld, &pOld->m_xUpperPrice, sizeof upperPriceOld);
		DecodeDataDiff(referencePriceNew, referencePriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		DecodeDataDiff(lowerPriceNew, lowerPriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		DecodeDataDiff(upperPriceNew, upperPriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
		memcpy(&pNew->m_dwReferencePrice, &referencePriceNew, sizeof referencePriceNew);
		memcpy(&pNew->m_dwLowerPrice, &lowerPriceNew, sizeof lowerPriceNew);
		memcpy(&pNew->m_xUpperPrice, &upperPriceNew, sizeof upperPriceNew);
	}
	else {
		pNew->m_cOrderImbalanceDirection = stream.ReadDWORD(8);

		DecodeBigintDiff(pNew->m_xOrderImbalanceQuantity,
			pOld->m_xOrderImbalanceQuantity,
			BC_DYNA_VOL_DIFF,
			BCNUM_DYNA_VOL_DIFF,
			stream);

		pNew->m_dwReferencePrice = DecodeUINT32Diff(pOld->m_dwReferencePrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);

		pNew->m_dwLowerPrice = DecodeUINT32Diff(pOld->m_dwLowerPrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);

		pNew->m_xUpperPrice = DecodeUINT32Diff(pOld->m_xUpperPrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);
	}
	return 0;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheVCM* pNew, const CStdCacheVCM* pOld, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		bool blnreadVCM = stream.ReadDWORD(1);
		if (blnreadVCM == true) {
			pNew->m_dwVCMDate = 0;
			pNew->m_dwVCMStartTime = 0;
			pNew->m_dwVCMEndTime = 0;
			pNew->m_dwVCMReferencePrice = 0;
			pNew->m_xVCMLowerPrice = 0;
			pNew->m_xVCMUpperPrice = 0;
		}
		else {
			DecodeDataDiff(pNew->m_dwVCMDate, pOld->m_dwVCMDate, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
			DecodeDataDiff(pNew->m_dwVCMStartTime, pOld->m_dwVCMStartTime, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
			DecodeDataDiff(pNew->m_dwVCMEndTime, pOld->m_dwVCMEndTime, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
			UINT32 referencePriceOld;
			UINT32 referencePriceNew;
			UINT32 lowerPriceOld;
			UINT32 lowerPriceNew;
			UINT32 upperPriceOld;
			UINT32 upperPriceNew;
			memcpy(&referencePriceOld, &pOld->m_dwVCMReferencePrice, sizeof referencePriceOld);
			memcpy(&lowerPriceOld, &pOld->m_xVCMLowerPrice, sizeof lowerPriceOld);
			memcpy(&upperPriceOld, &pOld->m_xVCMUpperPrice, sizeof upperPriceOld);
			DecodeDataDiff(referencePriceNew, referencePriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
			DecodeDataDiff(lowerPriceNew, lowerPriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
			DecodeDataDiff(upperPriceNew, upperPriceOld, BC_DYNA_PRICE_DIFFS, BCNUM_DYNA_PRICE_DIFFS, stream);
			memcpy(&pNew->m_dwVCMReferencePrice, &referencePriceNew, sizeof referencePriceNew);
			memcpy(&pNew->m_xVCMLowerPrice, &lowerPriceNew, sizeof lowerPriceNew);
			memcpy(&pNew->m_xVCMUpperPrice, &upperPriceNew, sizeof upperPriceNew);
		}
	}
	else {
		pNew->m_dwVCMDate = ReadDWORDDiff(stream, pOld->m_dwVCMDate, DATE_YYYYMMDD_BITS);
		pNew->m_dwVCMStartTime = ReadDWORDDiff(stream, pOld->m_dwVCMStartTime, TIME_HHMMSS_BITS);
		pNew->m_dwVCMEndTime = ReadDWORDDiff(stream, pOld->m_dwVCMEndTime, TIME_HHMMSS_BITS);

		pNew->m_dwVCMReferencePrice = DecodeUINT32Diff(pOld->m_dwVCMReferencePrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);

		pNew->m_xVCMLowerPrice = DecodeUINT32Diff(pOld->m_xVCMLowerPrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);

		pNew->m_xVCMUpperPrice = DecodeUINT32Diff(pOld->m_xVCMUpperPrice,
			BC_DYNA_PRICE_DIFFS,
			BCNUM_DYNA_PRICE_DIFFS,
			stream);
	}
	return 0;
}

template <typename T>
void CMultiQtCompressUnit::CompressMMP(COutputBitStream& stream, const T *pDyna, const UINT16 wVer) {
	// mmp consts
	int NUM_OF_MMP = 0;
	if (typeid (T) == typeid (CStdCacheQt))
		NUM_OF_MMP = 5;
	if (typeid (T) == typeid (CStdCacheQtS))
		NUM_OF_MMP = 10;
	int POS_BITS = NUM_OF_MMP - 1;
	int nArraySize = NUM_OF_MMP * 2;

	// mmp arrays
	const UINT32 * adwMMPPrice;
	const CBigInt * axMMPVol;

	int nNumBuy = pDyna->m_cNumBuy;
	int nNumSell = pDyna->m_cNumSell;

	adwMMPPrice = pDyna->m_pdwMMP;
	axMMPVol = pDyna->m_pxMMPVol;

	// nNumBuy, nNumSell
	if (nNumBuy == NUM_OF_MMP && nNumSell == NUM_OF_MMP) {
		stream.WriteBool(false);
	}
	else {
		stream.WriteBool(true);
		stream.WriteDWORD(nNumBuy, POS_BITS - 1);
		stream.WriteDWORD(nNumSell, POS_BITS - 1);
	}

	EncodeAllMMPPrice<T>(stream, NUM_OF_MMP, nNumBuy, nNumSell, adwMMPPrice, pDyna->m_dwPrice, wVer);

	if (wVer == MCM_NEW_VERSION1) {
		for (int i = NUM_OF_MMP - nNumSell; i < NUM_OF_MMP + nNumBuy; i++)
			stream.EncodeBigInt(axMMPVol[i], BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	}
	else {
		for (int i = NUM_OF_MMP - nNumBuy; i < NUM_OF_MMP + nNumSell; i++)
			stream.EncodeBigInt(axMMPVol[i], BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	}
}
template void CMultiQtCompressUnit::CompressMMP<CStdCacheQt>(COutputBitStream&, const CStdCacheQt*, const unsigned short);
template void CMultiQtCompressUnit::CompressMMP<CStdCacheQtS>(COutputBitStream&, const CStdCacheQtS*, const unsigned short);

template <typename T>
int CMultiQtCompressUnit::EncodeAllMMPPrice(COutputBitStream& stream, int NUM_OF_MMP, int nNumBuy, int nNumSell, const UINT32 * adwMMPPrice, UINT32 dwPrice, const UINT16 wVer) {
	if (wVer == MCM_NEW_VERSION1) {
		if (nNumBuy < 0 || nNumSell < 0 || (NUM_OF_MMP != 5 && NUM_OF_MMP != 10) || nNumBuy > NUM_OF_MMP || nNumSell > NUM_OF_MMP || adwMMPPrice == NULL)
			return -1;

		int POS_BITS = 4;
		if (NUM_OF_MMP == 10)
			POS_BITS = 5;

		// gap position
		UINT32 dwGap = 0;
		int nNumOfGap = 0;
		int nGapPos = -1;
		for (int i = NUM_OF_MMP - nNumSell; i < NUM_OF_MMP + nNumBuy - 1; i++) {            // this may work only for stock A.
			if (adwMMPPrice[i] - adwMMPPrice[i + 1] != 1) {
				dwGap |= (1 << i);

				if (nGapPos < 0)
					nGapPos = i;

				nNumOfGap++;
			}
		}
		if (nNumOfGap == 0) {
			stream.WriteDWORD(0, 2);         // 00
		}
		else if (nNumOfGap == 1) {
			stream.WriteDWORD(1, 2);         // 01
		}
		else {
			stream.WriteDWORD(2, 2);        // 10
		}
		// buy1 / sell1
		int nBasePos = NUM_OF_MMP;	// buy1
		UINT32 dwEncodePrice = adwMMPPrice[nBasePos];           // the default EncodePrice is from buy1.

		if (adwMMPPrice[NUM_OF_MMP - 1] == dwPrice && dwPrice > 0) {          // If sell1's price is equal to the current price, we set the EncodePrice a value
			dwEncodePrice = dwPrice + 999999;                                  // that can be encoded to correspond to the value which is from sell1.
			nBasePos = NUM_OF_MMP - 1;
		}
		else if (nNumBuy == 0) {                                  // If there is no buyers, we set the EncodePrice the value which is from sell1.
			dwEncodePrice = adwMMPPrice[NUM_OF_MMP - 1];
			nBasePos = NUM_OF_MMP - 1;
		}
		// the dwEncodePrice is the fundamental price from which we can decode all the other price.
		stream.EncodeData(dwEncodePrice, BC_DYNA_PRICE_MMP, BCNUM_DYNA_PRICE_MMP, &dwPrice);
		if (nNumOfGap == 1) {
			stream.WriteDWORD(nGapPos, POS_BITS);
			if (nGapPos < nBasePos)
				stream.EncodeData(adwMMPPrice[nGapPos], BC_DYNA_PRICE_GAP, BCNUM_DYNA_PRICE_GAP, &adwMMPPrice[nGapPos + 1]);
			else
				stream.EncodeData(adwMMPPrice[nGapPos + 1], BC_DYNA_PRICE_GAP, BCNUM_DYNA_PRICE_GAP, &adwMMPPrice[nGapPos], true);
		}
		else if (nNumOfGap > 1) {
			stream.WriteDWORD((dwGap >> (NUM_OF_MMP - nNumSell)), nNumBuy + nNumSell - 1);            // to move the Gap Pos to the first bit in order to save some memory space.
			for (int i = nBasePos - 1; i >= NUM_OF_MMP - nNumSell; i--) {
				if (dwGap & (1 << i)) {
					stream.EncodeData(adwMMPPrice[i], BC_DYNA_PRICE_GAP, BCNUM_DYNA_PRICE_GAP, &adwMMPPrice[i + 1]);
				}
			}
			for (int i = nBasePos; i < NUM_OF_MMP + nNumBuy - 1; i++) {
				if (dwGap & (1 << i)) {
					stream.EncodeData(adwMMPPrice[i + 1], BC_DYNA_PRICE_GAP, BCNUM_DYNA_PRICE_GAP, &adwMMPPrice[i], true);
				}
			}
		}
	}
	else {
		if (nNumBuy < 0 || nNumSell < 0 || (NUM_OF_MMP != 5 && NUM_OF_MMP != 10)    // Should we judge NUM_OF_MMP != 20 as the previous version does ?
			|| nNumBuy > NUM_OF_MMP || nNumSell > NUM_OF_MMP
			|| adwMMPPrice == NULL)
			return -1;

		int POS_BITS = 0;
		if (typeid (T) == typeid (CStdCacheQt)) {
			POS_BITS = (NUM_OF_MMP == 10) ? 5 : 4;
		}
		else if (typeid (T) == typeid (CStdCacheQtS)) {
			POS_BITS = (NUM_OF_MMP == 20) ? 10 : 9;
		}

		// gap position
		UINT32 dwGap = 0;
		int nNumOfGap = 0;
		int nGapPos = -1;
		for (int i = NUM_OF_MMP - nNumBuy; i < NUM_OF_MMP + nNumSell - 1; i++)
		{
			if (adwMMPPrice[i + 1] - adwMMPPrice[i] != 1)
			{
				dwGap |= (1 << i);

				if (nGapPos < 0)
					nGapPos = i;

				nNumOfGap++;
			}
		}

		if (nNumOfGap == 0)
			stream.WriteDWORD(0, 2);
		else if (nNumOfGap == 1)
			stream.WriteDWORD(1, 2);
		else
			stream.WriteDWORD(2, 2);

		// buy1 / sell1
		int nBasePos = NUM_OF_MMP - 1;	// buy1
		UINT32 dwEncodePrice = adwMMPPrice[nBasePos];

		if (adwMMPPrice[NUM_OF_MMP] == dwPrice && dwPrice > 0)
		{
			dwEncodePrice = dwPrice + 999999;
			nBasePos = NUM_OF_MMP;
		}
		else if (nNumBuy == 0)
		{
			dwEncodePrice = adwMMPPrice[NUM_OF_MMP];
			nBasePos = NUM_OF_MMP;
		}
		stream.EncodeData(dwEncodePrice, BC_DYNA_PRICE_MMP, BCNUM_DYNA_PRICE_MMP, &dwPrice);

		if (nNumOfGap == 1)
		{
			stream.WriteDWORD(nGapPos, POS_BITS);

			if (nGapPos < nBasePos)
				stream.EncodeData(adwMMPPrice[nGapPos], BC_DYNA_PRICE_GAP, BCNUM_DYNA_PRICE_GAP, &adwMMPPrice[nGapPos + 1], true);
			else
				stream.EncodeData(adwMMPPrice[nGapPos + 1], BC_DYNA_PRICE_GAP, BCNUM_DYNA_PRICE_GAP, &adwMMPPrice[nGapPos]);
		}
		else if (nNumOfGap > 1)
		{
			stream.WriteDWORD((dwGap >> (NUM_OF_MMP - nNumBuy)), nNumBuy + nNumSell - 1);

			for (int i = nBasePos - 1; i >= NUM_OF_MMP - nNumBuy; i--)
			{
				if (dwGap & (1 << i))
					stream.EncodeData(adwMMPPrice[i], BC_DYNA_PRICE_GAP, BCNUM_DYNA_PRICE_GAP, &adwMMPPrice[i + 1], true);
			}

			for (int i = nBasePos; i < NUM_OF_MMP + nNumSell - 1; i++)
			{
				if (dwGap & (1 << i))
					stream.EncodeData(adwMMPPrice[i + 1], BC_DYNA_PRICE_GAP, BCNUM_DYNA_PRICE_GAP, &adwMMPPrice[i]);
			}
		}
	}

	return 0;
}
template int CMultiQtCompressUnit::EncodeAllMMPPrice<CStdCacheQt>(COutputBitStream&, int, int, int, const UINT32*, UINT32, const UINT16);
template int CMultiQtCompressUnit::EncodeAllMMPPrice<CStdCacheQtS>(COutputBitStream&, int, int, int, const UINT32*, UINT32, const UINT16);

template <typename T>
void CMultiQtCompressUnit::ExpandMMP(CBitStream& stream, T *pDyna, const UINT16 wVer) {
	// mmp consts
	int NUM_OF_MMP = 0;
	if (typeid (T) == typeid (CStdCacheQt)) {
		NUM_OF_MMP = 5;
	}
	if (typeid (T) == typeid (CStdCacheQtS)) {
		NUM_OF_MMP = 10;
	}
	int POS_BITS = NUM_OF_MMP - 1;
	int nArraySize = NUM_OF_MMP * 2;
	// mmp arrays
	PDWORD adwMMPPrice;
	CBigInt *axMMPVol;
	{
		adwMMPPrice = pDyna->m_pdwMMP;
		axMMPVol = pDyna->m_pxMMPVol;
	}
	BYTE& cNumBuy = pDyna->m_cNumBuy;
	BYTE& cNumSell = pDyna->m_cNumSell;

	// NumBuy, NumSell
	bool bNumBuySell = (stream.ReadDWORD(1) != 0);
	if (bNumBuySell) {
		cNumBuy = (BYTE)stream.ReadDWORD(POS_BITS - 1);
		cNumSell = (BYTE)stream.ReadDWORD(POS_BITS - 1);
	}
	else {
		cNumBuy = NUM_OF_MMP;
		cNumSell = NUM_OF_MMP;
	}

	// whole price array
	DecodeAllMMPPrice<T>(stream, NUM_OF_MMP, cNumBuy, cNumSell, adwMMPPrice, pDyna->m_dwPrice, wVer);

	if (wVer == MCM_NEW_VERSION1) {
		for (int i = NUM_OF_MMP - cNumSell; i < NUM_OF_MMP + cNumBuy; i++)
			stream.DecodeBigInt(axMMPVol[i], BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	}
	else {
		for (int i = NUM_OF_MMP - cNumBuy; i < NUM_OF_MMP + cNumSell; i++)
			stream.DecodeBigInt(axMMPVol[i], BC_DYNA_VOL_DIFF, BCNUM_DYNA_VOL_DIFF);
	}
}
template void CMultiQtCompressUnit::ExpandMMP<CStdCacheQt>(CBitStream&, CStdCacheQt*, const UINT16);
template void CMultiQtCompressUnit::ExpandMMP<CStdCacheQtS>(CBitStream&, CStdCacheQtS*, const UINT16);

template <typename T>
int CMultiQtCompressUnit::DecodeAllMMPPrice(CBitStream& stream, int NUM_OF_MMP, int nNumBuy, int nNumSell, PDWORD adwMMPPrice, UINT32 dwPrice, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		if (nNumBuy < 0 || nNumSell < 0 || (NUM_OF_MMP != 5 && NUM_OF_MMP != 10) || nNumBuy > NUM_OF_MMP || nNumSell > NUM_OF_MMP || adwMMPPrice == NULL)
			return -1;

		int POS_BITS = 4;
		if (NUM_OF_MMP == 10)
			POS_BITS = 5;

		int nNumOfGap = stream.ReadDWORD(2);

		// Pbuy1 / Psell1
		UINT32 dwEncodePrice = 0;
		stream.DecodeData(dwEncodePrice, BC_DYNA_PRICE_MMP, BCNUM_DYNA_PRICE_MMP, &dwPrice);

		int nBasePos = 0;
		if (dwEncodePrice == dwPrice + 999999) {
			nBasePos = NUM_OF_MMP - 1;
			adwMMPPrice[nBasePos] = dwPrice;
		}
		else {
			if (nNumBuy == 0)
				nBasePos = NUM_OF_MMP - 1;
			else
				nBasePos = NUM_OF_MMP;
			adwMMPPrice[nBasePos] = dwEncodePrice;
		}

		UINT32 dwGap = 0;
		if (nNumOfGap == 1)  {
			int nGapPos = stream.ReadDWORD(POS_BITS);
			dwGap = (1 << nGapPos);
		}
		else if (nNumOfGap == 2) {                         // Please notice that this is not the real value of number of gap, the value '2' is just a flag
			dwGap = stream.ReadDWORD(nNumBuy + nNumSell - 1); // indicating that the number of gap is bigger than 1.
			dwGap <<= (NUM_OF_MMP - nNumSell);               // To understand this statement, you should read the corresponding part in the EncodeAllMMPPrice().
		}

		for (int i = nBasePos - 1; i >= NUM_OF_MMP - nNumSell; i--) {
			if (dwGap & (1 << i))
				stream.DecodeData(adwMMPPrice[i], BC_DYNA_PRICE_GAP, BCNUM_DYNA_PRICE_GAP, &adwMMPPrice[i + 1]);
			else
				adwMMPPrice[i] = adwMMPPrice[i + 1] + 1;
		}
		for (int i = nBasePos; i < NUM_OF_MMP + nNumBuy - 1; i++) {
			if (dwGap & (1 << i))
				stream.DecodeData(adwMMPPrice[i + 1], BC_DYNA_PRICE_GAP, BCNUM_DYNA_PRICE_GAP, &adwMMPPrice[i], true);
			else
				adwMMPPrice[i + 1] = adwMMPPrice[i] - 1;
		}
	}
	else {
		if (nNumBuy < 0 || nNumSell < 0 || (NUM_OF_MMP != 5 && NUM_OF_MMP != 10)
			|| nNumBuy > NUM_OF_MMP || nNumSell > NUM_OF_MMP
			|| adwMMPPrice == NULL)
			return -1;

		int POS_BITS = 0;
		if (typeid (T) == typeid (CStdCacheQt)) {
			POS_BITS = (NUM_OF_MMP == 10) ? 5 : 4;
		}
		else if (typeid (T) == typeid (CStdCacheQtS)) {
			POS_BITS = (NUM_OF_MMP == 20) ? 10 : 9;
		}
		int nNumOfGap = stream.ReadDWORD(2);

		// Pbuy1 / Psell1
		UINT32 dwEncodePrice = 0;
		stream.DecodeData(dwEncodePrice, BC_DYNA_PRICE_MMP, BCNUM_DYNA_PRICE_MMP, &dwPrice);

		int nBasePos = 0;
		if (dwEncodePrice == dwPrice + 999999)
		{
			nBasePos = NUM_OF_MMP;
			adwMMPPrice[nBasePos] = dwPrice;
		}
		else
		{
			if (nNumBuy == 0)
				nBasePos = NUM_OF_MMP;
			else
				nBasePos = NUM_OF_MMP - 1;
			adwMMPPrice[nBasePos] = dwEncodePrice;
		}

		UINT32 dwGap = 0;
		if (nNumOfGap == 1)
		{
			int nGapPos = stream.ReadDWORD(POS_BITS);
			dwGap = (1 << nGapPos);
		}
		else if (nNumOfGap == 2)
		{
			dwGap = stream.ReadDWORD(nNumBuy + nNumSell - 1);
			dwGap <<= (NUM_OF_MMP - nNumBuy);
		}

		for (int i = nBasePos - 1; i >= NUM_OF_MMP - nNumBuy; i--)
		{
			if (dwGap & (1 << i))
				stream.DecodeData(adwMMPPrice[i], BC_DYNA_PRICE_GAP, BCNUM_DYNA_PRICE_GAP, &adwMMPPrice[i + 1], true);
			else
				adwMMPPrice[i] = adwMMPPrice[i + 1] - 1;
		}

		for (int i = nBasePos; i < NUM_OF_MMP + nNumSell - 1; i++)
		{
			if (dwGap & (1 << i))
				stream.DecodeData(adwMMPPrice[i + 1], BC_DYNA_PRICE_GAP, BCNUM_DYNA_PRICE_GAP, &adwMMPPrice[i]);
			else
				adwMMPPrice[i + 1] = adwMMPPrice[i] + 1;
		}
	}
	return 0;
}
template int CMultiQtCompressUnit::DecodeAllMMPPrice<CStdCacheQt>(CBitStream&, int, int, int, PDWORD, UINT32, const UINT16);
template int CMultiQtCompressUnit::DecodeAllMMPPrice<CStdCacheQtS>(CBitStream&, int, int, int, PDWORD, UINT32, const UINT16);

template <typename T>
void CMultiQtCompressUnit::CompressMMP(COutputBitStream& stream, const T *pDyna, const T *pOldDyna, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		// mmp consts
		int NUM_OF_MMP = 0;
		if (typeid (T) == typeid (CStdCacheQt)) {
			NUM_OF_MMP = 5;
		}
		if (typeid (T) == typeid (CStdCacheQtS)) {
			NUM_OF_MMP = 10;
		}
		int MAX_PCHANGE = 3;
		int MAX_VCHANGE = 2;
		int POS_BITS = NUM_OF_MMP - 1;
		int nArraySize = NUM_OF_MMP * 2;

		// mmp arrays
		const UINT32 *pdwMMP;
		CBigInt* pxMMPVol = new CBigInt[nArraySize];

		int nNumBuy = pDyna->m_cNumBuy;
		int nNumSell = pDyna->m_cNumSell;

		{
			pdwMMP = pDyna->m_pdwMMP;
			memcpy(pxMMPVol, pDyna->m_pxMMPVol, nArraySize * sizeof(CBigInt));
			if (nNumBuy > NUM_OF_MMP)
				nNumBuy = NUM_OF_MMP;
			if (nNumSell > NUM_OF_MMP)
				nNumSell = NUM_OF_MMP;
		}

		// old mmp arrays
		const UINT32 *adwLastMMPPrice;
		const CBigInt *axLastMMPVol;

		int nNumLastBuy = pOldDyna->m_cNumBuy;
		int nNumLastSell = pOldDyna->m_cNumSell;

		{
			adwLastMMPPrice = pOldDyna->m_pdwMMP;
			axLastMMPVol = pOldDyna->m_pxMMPVol;
			if (nNumLastBuy > NUM_OF_MMP)
				nNumLastBuy = NUM_OF_MMP;
			if (nNumLastSell > NUM_OF_MMP)
				nNumLastSell = NUM_OF_MMP;
		}

		const BYTE MMP_NOCHANGE = 0;	// 00
		const BYTE MMP_VCHANGE = 1;      // 01
		const BYTE MMP_PVCHANGE = 2;	// 10
		const BYTE MMP_PPVCHANGE = 3;	// 11, encode whole price array

		// In this case, there is no change in both the price array and the volume array, so we can just write a flag.
		if (memcmp(pdwMMP, adwLastMMPPrice, nArraySize * sizeof(UINT32)) == 0 && memcmp(pxMMPVol, axLastMMPVol, nArraySize * sizeof(CBigInt)) == 0) {
			stream.WriteDWORD(MMP_NOCHANGE, 2);
		}
		else {
			INT64* phMMPVol = new INT64[nArraySize];
			for (int i = 0; i < nArraySize; i++)
				phMMPVol[i] = pxMMPVol[i];

			// flag
			const BYTE MOD_INSERT = 0;		// 0
			const BYTE MOD_DELETE = 1;		// 1

			BYTE acPriceModPos[40];		// 2 * NUM_OF_MMP
			BYTE acPriceModType[40];

			int nNumOfPChange = 0;              // to record the number of changes of the price array

			if (nNumLastBuy == 0 && nNumLastSell == 0) {    // In this case, we have no base array of price, so we have to encode the whole price array
				nNumOfPChange = nArraySize;
			}
			else {                                        // compare price array
				int nPos = 0;
				int nLastPos = 0;

				int nTop = NUM_OF_MMP + nNumBuy;
				int nLastTop = NUM_OF_MMP + nNumLastBuy;

				while (nPos < nTop && nLastPos < nLastTop) {
					if (pdwMMP[nPos] == adwLastMMPPrice[nLastPos]) {
						if (nPos < NUM_OF_MMP && nLastPos < NUM_OF_MMP || nPos >= NUM_OF_MMP && nLastPos >= NUM_OF_MMP)
							phMMPVol[nPos] = phMMPVol[nPos] - axLastMMPVol[nLastPos].GetValue();

						nPos++;
						nLastPos++;
					}
					else if (pdwMMP[nPos] > adwLastMMPPrice[nLastPos]) { 	// delete
						acPriceModPos[nNumOfPChange] = nLastPos;
						acPriceModType[nNumOfPChange++] = MOD_DELETE;

						nLastPos++;
					}
					else {	// pdwMMP[nPos] < adwLastMMPPrice[nLastPos] // insert
						acPriceModPos[nNumOfPChange] = nPos;
						acPriceModType[nNumOfPChange++] = MOD_INSERT;

						nPos++;
					}
				}
				while (nPos < nTop) {
					acPriceModPos[nNumOfPChange] = nPos++;
					acPriceModType[nNumOfPChange++] = MOD_INSERT;
				}
				if (pdwMMP[NUM_OF_MMP - 1] == pDyna->m_dwPrice)         // sell1
					phMMPVol[NUM_OF_MMP - 1] = phMMPVol[NUM_OF_MMP - 1] + pDyna->m_xVolume.GetValue() - pOldDyna->m_xVolume.GetValue();
				else if (pdwMMP[NUM_OF_MMP] == pDyna->m_dwPrice)        // buy1
					phMMPVol[NUM_OF_MMP] = phMMPVol[NUM_OF_MMP] + pDyna->m_xVolume.GetValue() - pOldDyna->m_xVolume.GetValue();
			}   // price compare

			if (nNumOfPChange == 0 && nNumSell == nNumLastSell) {	// only volume change
				stream.WriteDWORD(MMP_VCHANGE, 2);
			}
			else {  // pv/ppv change, price streaming
				if (nNumOfPChange <= MAX_PCHANGE) {
					stream.WriteDWORD(MMP_PVCHANGE, 2);
				}
				else {
					stream.WriteDWORD(MMP_PPVCHANGE, 2);
				}
				// nNumBuy, nNumSell
				if (nNumBuy == NUM_OF_MMP && nNumSell == NUM_OF_MMP)
					stream.WriteBool(false);
				else {
					stream.WriteBool(true);

					stream.WriteDWORD(nNumBuy, POS_BITS - 1);
					stream.WriteDWORD(nNumSell, POS_BITS - 1);
				}

				// price change
				if (nNumOfPChange <= MAX_PCHANGE) { // part price encoding
					int nModCode = 8;
					int nInsPos = -1;
					if (nNumOfPChange == 2) {
						if (acPriceModPos[0] == 0 && acPriceModType[0] == MOD_INSERT && acPriceModPos[1] == nArraySize - 1 && acPriceModType[1] == MOD_DELETE) {
							nModCode = 0;		// insert sell 5/10, delete buy 5/10
							nInsPos = 0;
						}
						else if (acPriceModPos[0] == NUM_OF_MMP && acPriceModType[0] == MOD_INSERT && acPriceModPos[1] == nArraySize - 1 && acPriceModType[1] == MOD_DELETE) {
							nModCode = 1;		// insert buy 1,   delete buy 5/10
							nInsPos = NUM_OF_MMP;
						}
						else if (acPriceModPos[0] == 0 && acPriceModType[0] == MOD_INSERT && acPriceModPos[1] == NUM_OF_MMP - 1 && acPriceModType[1] == MOD_DELETE) {
							nModCode = 2;		// insert sell 5/10,   delete sell 1
							nInsPos = 0;
						}
						else if (acPriceModPos[0] == 0 && acPriceModType[0] == MOD_DELETE && acPriceModPos[1] == NUM_OF_MMP - 1 && acPriceModType[1] == MOD_INSERT) {
							nModCode = 3;		// delete sell 5/10,   insert sell 1
							nInsPos = NUM_OF_MMP - 1;
						}
						else if (acPriceModPos[0] == NUM_OF_MMP && acPriceModType[0] == MOD_DELETE && acPriceModPos[1] == nArraySize - 1 && acPriceModType[1] == MOD_INSERT) {
							nModCode = 4;		// delete buy 1,   insert buy 5/10
							nInsPos = nArraySize - 1;
						}
						else if (acPriceModPos[0] == 0 && acPriceModType[0] == MOD_DELETE && acPriceModPos[1] == nArraySize - 1 && acPriceModType[1] == MOD_INSERT) {
							nModCode = 5;		// delete sell 5/10,   insert buy5/10
							nInsPos = nArraySize - 1;
						}
					}

					// In this case, nNumOfChange == 2
					if (nModCode < 8) {	// special case, just insert one price
						stream.WriteDWORD(nModCode, 4);		// 0 + 3bit code
						UINT32 dwPriceInsGap = 999999;		// price = 0
						if (pdwMMP[nInsPos] > 0) {
							if (nInsPos > 0)
								dwPriceInsGap = pdwMMP[nInsPos - 1] - pdwMMP[nInsPos];
							else // that is, nInsPos ==0
								dwPriceInsGap = pdwMMP[0] - pdwMMP[1];
						}

						stream.EncodeData(dwPriceInsGap, BC_DYNA_PRICE_INS, BCNUM_DYNA_PRICE_INS);
					}
					else {				// encode price change one by one
						nModCode += nNumOfPChange;			// MAX_PCHANGE must < 8
						stream.WriteDWORD(nModCode, 4);		// 1 + 3bit nNumOfPChange
						for (int i = 0; i < nNumOfPChange; i++) {
							int nPricePos = acPriceModPos[i];
							stream.WriteDWORD(nPricePos, POS_BITS);
							stream.WriteDWORD(acPriceModType[i], 1);

							if (acPriceModType[i] == MOD_INSERT) {
								UINT32 dwPriceInsGap = 999999;
								if (pdwMMP[nPricePos] > 0) {
									if (nPricePos > 0)
										dwPriceInsGap = pdwMMP[nPricePos - 1] - pdwMMP[nPricePos];
									else		// pos == 0
										//                                        dwPriceInsGap = pDyna->m_dwPrice - pdwMMP[0];
										dwPriceInsGap = pdwMMP[0] - pDyna->m_dwPrice;
								}

								stream.EncodeData(dwPriceInsGap, BC_DYNA_PRICE_INS, BCNUM_DYNA_PRICE_INS);
							}
						}
					}
				}
				else {	// nNumOfPChange > MAX_PCHANGE, encode whole price array
					EncodeAllMMPPrice<T>(stream, NUM_OF_MMP, nNumBuy, nNumSell, pdwMMP, pDyna->m_dwPrice, wVer);
				}
			}   // p changed

			// volume streaming
			UINT32 dwVolChange = 0;
			int nNumOfVChange = 0;
			for (int i = 0; i < nArraySize; i++) {          // notice that we record the changed value of volume rather than the actual value of volume
				if (phMMPVol[i] != 0) {
					dwVolChange |= (1 << i);
					nNumOfVChange++;
				}
			}

			if (nNumOfVChange <= MAX_VCHANGE) {
				stream.WriteBool(false);
				stream.WriteDWORD(nNumOfVChange, 2);	// Because MAX_VCHANGE <= 3, so we only need 2 bits to store nNumOfVChange.

				for (int i = 0; i < nArraySize; i++) {
					if (dwVolChange & (1 << i)) {
						stream.WriteDWORD(i, POS_BITS);
						CBigInt xVol = phMMPVol[i];
						stream.EncodeBigInt(xVol, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS);
					}
				}
			}
			else {            // nNumOfChange > 2
				stream.WriteBool(true);
				stream.WriteDWORD((dwVolChange >> (NUM_OF_MMP - nNumSell)), nNumBuy + nNumSell);

				for (int i = 0; i < nArraySize; i++) {
					if (dwVolChange & (1 << i)) {
						CBigInt xVol = phMMPVol[i];
						stream.EncodeBigInt(xVol, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS);
					}
				}
			}
			delete[] pxMMPVol;
			delete[] phMMPVol;
		}// mmp changed
	}
	else {
		// mmp consts
		int NUM_OF_MMP = 0;
		if (typeid (T) == typeid (CStdCacheQt)) {
			NUM_OF_MMP = 5;
		}
		if (typeid (T) == typeid (CStdCacheQtS)) {
			NUM_OF_MMP = 10;
		}
		int MAX_PCHANGE = 3;
		int MAX_VCHANGE = 2;
		int POS_BITS = NUM_OF_MMP - 1;

		int nArraySize = NUM_OF_MMP * 2;

		// mmp arrays
		const UINT32 *  pdwMMP;
		CBigInt* pxMMPVol = new CBigInt[nArraySize];

		int nNumBuy = pDyna->m_cNumBuy;
		int nNumSell = pDyna->m_cNumSell;

		{
			pdwMMP = pDyna->m_pdwMMP;
			memcpy(pxMMPVol, pDyna->m_pxMMPVol, nArraySize*sizeof(CBigInt));

			if (nNumBuy > NUM_OF_MMP)
				nNumBuy = NUM_OF_MMP;
			if (nNumSell > NUM_OF_MMP)
				nNumSell = NUM_OF_MMP;
		}

		// old mmp arrays
		const UINT32 * adwLastMMPPrice;
		const CBigInt * axLastMMPVol;

		int nNumLastBuy = pOldDyna->m_cNumBuy;
		int nNumLastSell = pOldDyna->m_cNumSell;

		{
			adwLastMMPPrice = pOldDyna->m_pdwMMP;
			axLastMMPVol = pOldDyna->m_pxMMPVol;

			if (nNumLastBuy > NUM_OF_MMP)
				nNumLastBuy = NUM_OF_MMP;
			if (nNumLastSell > NUM_OF_MMP)
				nNumLastSell = NUM_OF_MMP;
		}

		const BYTE MMP_NOCHANGE = 0;	// 00
		const BYTE MMP_VCHANGE = 1;		// 01
		const BYTE MMP_PVCHANGE = 2;	// 10
		const BYTE MMP_PPVCHANGE = 3;	// 11, encode whole price array

		if (memcmp(pdwMMP, adwLastMMPPrice, nArraySize*sizeof(UINT32)) == 0
			&& memcmp(pxMMPVol, axLastMMPVol, nArraySize*sizeof(CBigInt)) == 0)
			stream.WriteDWORD(MMP_NOCHANGE, 2);
		else
		{
			INT64* phMMPVol = new INT64[nArraySize];
			for (int i = 0; i < nArraySize; i++)
				phMMPVol[i] = pxMMPVol[i];

			const BYTE MOD_INSERT = 0;		// 0
			const BYTE MOD_DELETE = 1;		// 1

			BYTE acPriceModPos[40];		// 2*10
			BYTE acPriceModType[40];

			int nNumOfPChange = 0;

			if (nNumLastBuy == 0 && nNumLastSell == 0)
				nNumOfPChange = nArraySize;		// encode whole price array
			else	// compare price array
			{
				int nPos = 0;
				int nLastPos = 0;

				int nTop = NUM_OF_MMP + nNumSell;
				int nLastTop = NUM_OF_MMP + nNumLastSell;

				while (nPos < nTop && nLastPos < nLastTop)
				{
					if (pdwMMP[nPos] == adwLastMMPPrice[nLastPos])
					{
						if (nPos < NUM_OF_MMP && nLastPos < NUM_OF_MMP
							|| nPos >= NUM_OF_MMP && nLastPos >= NUM_OF_MMP)
							phMMPVol[nPos] = phMMPVol[nPos] - axLastMMPVol[nLastPos].GetValue();

						nPos++;
						nLastPos++;
					}
					else if (pdwMMP[nPos] > adwLastMMPPrice[nLastPos])	// delete
					{
						acPriceModPos[nNumOfPChange] = nLastPos;
						acPriceModType[nNumOfPChange++] = MOD_DELETE;

						nLastPos++;
					}
					else	// pdwMMP[nPos] < adwLastMMPPrice[nLastPos] // insert
					{
						acPriceModPos[nNumOfPChange] = nPos;
						acPriceModType[nNumOfPChange++] = MOD_INSERT;

						nPos++;
					}
				}

				while (nPos < nTop)
				{
					acPriceModPos[nNumOfPChange] = nPos++;
					acPriceModType[nNumOfPChange++] = MOD_INSERT;
				}

				if (pdwMMP[NUM_OF_MMP - 1] == pDyna->m_dwPrice)
					phMMPVol[NUM_OF_MMP - 1] = phMMPVol[NUM_OF_MMP - 1] + pDyna->m_xVolume.GetValue() - pOldDyna->m_xVolume.GetValue();
				else if (pdwMMP[NUM_OF_MMP] == pDyna->m_dwPrice)
					phMMPVol[NUM_OF_MMP] = phMMPVol[NUM_OF_MMP] + pDyna->m_xVolume.GetValue() - pOldDyna->m_xVolume.GetValue();
			}// price compare

			if (nNumOfPChange == 0 && nNumSell == nNumLastSell)	// v change
				stream.WriteDWORD(MMP_VCHANGE, 2);
			else						// pv/ppv change, price streaming
			{
				if (nNumOfPChange <= MAX_PCHANGE)
					stream.WriteDWORD(MMP_PVCHANGE, 2);
				else
					stream.WriteDWORD(MMP_PPVCHANGE, 2);

				// nNumBuy, nNumSell
				if (nNumBuy == NUM_OF_MMP && nNumSell == NUM_OF_MMP)
					stream.WriteBool(false);
				else
				{
					stream.WriteBool(true);

					stream.WriteDWORD(nNumBuy, POS_BITS - 1);
					stream.WriteDWORD(nNumSell, POS_BITS - 1);
				}

				// price change
				if (nNumOfPChange <= MAX_PCHANGE)
				{
					int nModCode = 8;
					int nInsPos = -1;
					if (nNumOfPChange == 2)
					{
						if (acPriceModPos[0] == 0 && acPriceModType[0] == MOD_INSERT
							&& acPriceModPos[1] == nArraySize - 1 && acPriceModType[1] == MOD_DELETE)
						{
							nModCode = 0;		// ins buy5/10, del sell 5/10
							nInsPos = 0;
						}
						else if (acPriceModPos[0] == NUM_OF_MMP && acPriceModType[0] == MOD_INSERT
							&& acPriceModPos[1] == nArraySize - 1 && acPriceModType[1] == MOD_DELETE)
						{
							nModCode = 1;		// ins sell1,   del sell 5/10
							nInsPos = NUM_OF_MMP;
						}
						else if (acPriceModPos[0] == 0 && acPriceModType[0] == MOD_INSERT
							&& acPriceModPos[1] == NUM_OF_MMP - 1 && acPriceModType[1] == MOD_DELETE)
						{
							nModCode = 2;		// ins buy5/10,   del buy1
							nInsPos = 0;
						}
						else if (acPriceModPos[0] == 0 && acPriceModType[0] == MOD_DELETE
							&& acPriceModPos[1] == NUM_OF_MMP - 1 && acPriceModType[1] == MOD_INSERT)
						{
							nModCode = 3;		// del buy5/10,   ins buy1
							nInsPos = NUM_OF_MMP - 1;
						}
						else if (acPriceModPos[0] == NUM_OF_MMP && acPriceModType[0] == MOD_DELETE
							&& acPriceModPos[1] == nArraySize - 1 && acPriceModType[1] == MOD_INSERT)
						{
							nModCode = 4;		// del sell1,   ins sell5/10
							nInsPos = nArraySize - 1;
						}
						else if (acPriceModPos[0] == 0 && acPriceModType[0] == MOD_DELETE
							&& acPriceModPos[1] == nArraySize - 1 && acPriceModType[1] == MOD_INSERT)
						{
							nModCode = 5;		// del buy5/10,   ins sell5/10
							nInsPos = nArraySize - 1;
						}
					}

					if (nModCode < 8)	// special case, just insert one price
					{
						stream.WriteDWORD(nModCode, 4);		// 0 + 3bit code

						UINT32 dwPriceInsGap = 999999;		// price = 0
						if (pdwMMP[nInsPos] > 0)
						{
							if (nInsPos > 0)
								dwPriceInsGap = pdwMMP[nInsPos] - pdwMMP[nInsPos - 1];
							else
								dwPriceInsGap = pdwMMP[1] - pdwMMP[0];
						}

						stream.EncodeData(dwPriceInsGap, BC_DYNA_PRICE_INS, BCNUM_DYNA_PRICE_INS);
					}
					else				// encode price change one by one
					{
						nModCode += nNumOfPChange;			// MAX_PCHANGE must < 8
						stream.WriteDWORD(nModCode, 4);		// 1 + 3bit nNumOfPChange

						for (int i = 0; i < nNumOfPChange; i++)
						{
							int nPricePos = acPriceModPos[i];
							stream.WriteDWORD(nPricePos, POS_BITS);
							stream.WriteDWORD(acPriceModType[i], 1);

							if (acPriceModType[i] == MOD_INSERT)
							{
								UINT32 dwPriceInsGap = 999999;
								if (pdwMMP[nPricePos] > 0)
								{
									if (nPricePos > 0)
										dwPriceInsGap = pdwMMP[nPricePos] - pdwMMP[nPricePos - 1];
									else		// pos == 0
										dwPriceInsGap = pDyna->m_dwPrice - pdwMMP[0];
								}

								stream.EncodeData(dwPriceInsGap, BC_DYNA_PRICE_INS, BCNUM_DYNA_PRICE_INS);
							}
						}
					}
				}// part price encoding
				else	// nNumOfPChange > MAX_PCHANGE, encode whole price array
				{
					EncodeAllMMPPrice<T>(stream, NUM_OF_MMP, nNumBuy, nNumSell, pdwMMP, pDyna->m_dwPrice);
				}
			}// p changed

			// volume streaming
			UINT32 dwVolChange = 0;
			int nNumOfVChange = 0;
			for (int i = 0; i < nArraySize; i++)
			{
				if (phMMPVol[i] != 0)
				{
					dwVolChange |= (1 << i);
					nNumOfVChange++;
				}
			}

			if (nNumOfVChange <= MAX_VCHANGE)
			{
				stream.WriteBool(false);
				stream.WriteDWORD(nNumOfVChange, 2);	// MAX_VCHANGE <= 3

				for (int i = 0; i < nArraySize; i++)
				{
					if (dwVolChange & (1 << i))
					{
						stream.WriteDWORD(i, POS_BITS);
						CBigInt xVol = phMMPVol[i];
						stream.EncodeBigInt(xVol, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS);
					}
				}
			}
			else
			{
				stream.WriteBool(true);

				stream.WriteDWORD((dwVolChange >> (NUM_OF_MMP - nNumBuy)), nNumBuy + nNumSell);

				for (int i = 0; i < nArraySize; i++)
				{
					if (dwVolChange & (1 << i))
					{
						CBigInt xVol = phMMPVol[i];
						stream.EncodeBigInt(xVol, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS);
					}
				}
			}
			delete phMMPVol;
		}// mmp changed
		delete[] pxMMPVol;
	}
}
template void CMultiQtCompressUnit::CompressMMP<CStdCacheQt>(COutputBitStream&, const CStdCacheQt*, const CStdCacheQt*, const UINT16);
template void CMultiQtCompressUnit::CompressMMP<CStdCacheQtS>(COutputBitStream&, const CStdCacheQtS*, const CStdCacheQtS*, const UINT16);

template <typename T>
void CMultiQtCompressUnit::ExpandMMP(CBitStream& stream, T *pDyna, const T *pOldDyna, const UINT16 wVer)
{
	if (wVer == MCM_NEW_VERSION1) {
		// mmp consts
		int NUM_OF_MMP = 0;
		if (typeid (T) == typeid (CStdCacheQt)) {
			NUM_OF_MMP = 5;
		}
		if (typeid (T) == typeid (CStdCacheQtS)) {
			NUM_OF_MMP = 10;
		}
		int MAX_PCHANGE = 3;
		int MAX_VCHANGE = 2;
		int POS_BITS = NUM_OF_MMP - 1;
		int nArraySize = NUM_OF_MMP * 2;

		// mmp arrays
		PDWORD pdwMMP;
		CBigInt * pxMMPVol;


		{
			pdwMMP = pDyna->m_pdwMMP;
			pxMMPVol = pDyna->m_pxMMPVol;
		}

		BYTE &cNumBuy = pDyna->m_cNumBuy;
		BYTE &cNumSell = pDyna->m_cNumSell;

		// old mmp arrays
		const UINT32 * adwLastMMPPrice;
		const CBigInt * axLastMMPVol;

		BYTE cNumLastBuy = pOldDyna->m_cNumBuy;
		BYTE cNumLastSell = pOldDyna->m_cNumSell;

		{
			adwLastMMPPrice = pOldDyna->m_pdwMMP;
			axLastMMPVol = pOldDyna->m_pxMMPVol;

			if (cNumLastBuy > NUM_OF_MMP)
				cNumLastBuy = NUM_OF_MMP;
			if (cNumLastSell > NUM_OF_MMP)
				cNumLastSell = NUM_OF_MMP;
		}

		const BYTE MMP_NOCHANGE = 0;	// 00
		const BYTE MMP_VCHANGE = 1;	// 01
		const BYTE MMP_PVCHANGE = 2;	// 10
		const BYTE MMP_PPVCHANGE = 3;	// 11, decode whole price array

		BYTE cPVchange = (BYTE)stream.ReadDWORD(2);
		if (cPVchange == MMP_NOCHANGE) {
			memcpy(pdwMMP, adwLastMMPPrice, nArraySize * sizeof(UINT32));
			memcpy(pxMMPVol, axLastMMPVol, nArraySize * sizeof(CBigInt));
			cNumBuy = cNumLastBuy;
			cNumSell = cNumLastSell;
		}
		else {
			INT64* phMMPVol = new INT64[nArraySize];
			memset(phMMPVol, 0, sizeof(INT64) * nArraySize);

			if (cPVchange == MMP_VCHANGE) {
				memcpy(pdwMMP, adwLastMMPPrice, nArraySize * sizeof(UINT32));
				cNumBuy = cNumLastBuy;
				cNumSell = cNumLastSell;
			}
			else {	// pv/ppv
				bool bNumBuySell = stream.ReadDWORD(1);
				if (bNumBuySell) {
					cNumBuy = (BYTE)stream.ReadDWORD(POS_BITS - 1);
					cNumSell = (BYTE)stream.ReadDWORD(POS_BITS - 1);
				}
				else {
					cNumBuy = NUM_OF_MMP;
					cNumSell = NUM_OF_MMP;
				}
				if (cPVchange == MMP_PVCHANGE) {
					int nModCode = stream.ReadDWORD(4);
					if (nModCode < 8) {	// special case, just insert one price
						int nInsPos = -1;
						int nDelPos = -1;
						if (nModCode == 0) {		// insert sell 5/10, delete buy 5/10
							nInsPos = 0;
							nDelPos = nArraySize - 1;
						}
						else if (nModCode == 1)	 {      // insert buy 1, delete buy 5/10
							nInsPos = NUM_OF_MMP;
							nDelPos = nArraySize - 1;
						}
						else if (nModCode == 2) {	// insert sell 5/10, delete sell 1
							nInsPos = 0;
							nDelPos = NUM_OF_MMP - 1;
						}
						else if (nModCode == 3)	 {      // delete sell 5/10, insert sell 1
							nInsPos = NUM_OF_MMP - 1;
							nDelPos = 0;
						}
						else if (nModCode == 4)	 {      // delete buy 1, insert buy 5/10
							nInsPos = nArraySize - 1;
							nDelPos = NUM_OF_MMP;
						}
						else if (nModCode == 5)	{       // delete sell 5/10, insert buy 5/10
							nInsPos = nArraySize - 1;
							nDelPos = 0;
						}

						memcpy(pdwMMP, adwLastMMPPrice, nArraySize * sizeof(UINT32));
						if (nInsPos < nDelPos)
							memmove(&pdwMMP[nInsPos + 1], &pdwMMP[nInsPos], (nDelPos - nInsPos) * sizeof(UINT32));
						else
							memmove(&pdwMMP[nDelPos], &pdwMMP[nDelPos + 1], (nInsPos - nDelPos) * sizeof(UINT32));

						UINT32 dwPriceInsGap = 0;
						stream.DecodeData(dwPriceInsGap, BC_DYNA_PRICE_INS, BCNUM_DYNA_PRICE_INS);
						if (dwPriceInsGap == 999999)
							pdwMMP[nInsPos] = 0;
						else {
							if (nInsPos > 0)
								pdwMMP[nInsPos - 1] = pdwMMP[nInsPos] + dwPriceInsGap;
							else
								pdwMMP[1] = pdwMMP[0] - dwPriceInsGap;
						}
						for (int i = NUM_OF_MMP + cNumSell; i < nArraySize; i++)
							pdwMMP[i] = 0;
					}
					else {	// decode price change one by one
						int nNumOfPChange = nModCode - 8;
						if (nNumOfPChange == 0)
							memcpy(pdwMMP, adwLastMMPPrice, (NUM_OF_MMP + cNumBuy) * sizeof(UINT32));
						else {
							const BYTE MOD_INSERT = 0;		// 0
							const BYTE MOD_DELETE = 1;		// 1

							int nChngPos = 0;

							int nPricePos = stream.ReadDWORD(POS_BITS);
							int nPriceModType = stream.ReadDWORD(1);
							UINT32 dwPriceInsGap = 0;
							if (nPriceModType == MOD_INSERT)
								stream.DecodeData(dwPriceInsGap, BC_DYNA_PRICE_INS, BCNUM_DYNA_PRICE_INS);

							int nPos = 0;
							int nLastPos = 0;
							int nTop = NUM_OF_MMP + cNumBuy;
							int nLastTop = NUM_OF_MMP + cNumLastBuy;

							while (nPos < nTop) {
								bool bChange = false;
								if (nPricePos == nLastPos && nPriceModType == MOD_DELETE) {
									nLastPos++;
									bChange = true;
								}
								else if (nPricePos == nPos && nPriceModType == MOD_INSERT) {
									if (dwPriceInsGap == 999999)
										pdwMMP[nPos] = 0;
									else {
										if (nPos > 0)
											pdwMMP[nPos - 1] = pdwMMP[nPos] + dwPriceInsGap;
										else
											//                                      pdwMMP[0] = pDyna->m_dwPrice - dwPriceInsGap;
											pDyna->m_dwPrice = pdwMMP[0] - dwPriceInsGap;
									}

									nPos++;

									bChange = true;
								}
								else {
									if (nLastPos < nLastTop)
										pdwMMP[nPos] = adwLastMMPPrice[nLastPos];

									nPos++;
									nLastPos++;
								}
								if (bChange) {
									nChngPos++;
									if (nChngPos < nNumOfPChange)	{// read next one
										nPricePos = stream.ReadDWORD(POS_BITS);
										nPriceModType = stream.ReadDWORD(1);
										dwPriceInsGap = 0;
										if (nPriceModType == MOD_INSERT)
											stream.DecodeData(dwPriceInsGap, BC_DYNA_PRICE_INS, BCNUM_DYNA_PRICE_INS);
									}
									else		// change list end
										nPricePos = -1;
								}
							}
						}// nNumOfPChange > 0
					}// decode price change one by one
				}// cPVchange == MMP_PVCHANGE
				else {	// cPVchange == MMP_PPVCHANGE
					DecodeAllMMPPrice<T>(stream, NUM_OF_MMP, cNumBuy, cNumSell, pdwMMP, pDyna->m_dwPrice, wVer);
				}// cPVchange == MMP_PPVCHANGE
			}// p changed

			// volume decoding
			if (cNumLastBuy > 0 || cNumLastSell > 0) {
				int nPos = NUM_OF_MMP - cNumSell;
				int nLastPos = NUM_OF_MMP - cNumLastSell;

				while (nPos < NUM_OF_MMP + cNumBuy && nLastPos < NUM_OF_MMP + cNumLastBuy) {
					if (pdwMMP[nPos] == adwLastMMPPrice[nLastPos]) {
						if (nPos < NUM_OF_MMP && nLastPos < NUM_OF_MMP || nPos >= NUM_OF_MMP && nLastPos >= NUM_OF_MMP)
							phMMPVol[nPos] = axLastMMPVol[nLastPos];

						nPos++;
						nLastPos++;
					}
					else if (pdwMMP[nPos] > adwLastMMPPrice[nLastPos])	// delete
						nLastPos++;
					else	// pdwMMP[nPos] < adwLastMMPPrice[nLastPos] // insert
						nPos++;
				}

				if (pdwMMP[NUM_OF_MMP - 1] == pDyna->m_dwPrice)
					phMMPVol[NUM_OF_MMP - 1] = phMMPVol[NUM_OF_MMP - 1] - (pDyna->m_xVolume.GetValue() - pOldDyna->m_xVolume.GetValue());
				else if (pdwMMP[NUM_OF_MMP] == pDyna->m_dwPrice)
					phMMPVol[NUM_OF_MMP] = phMMPVol[NUM_OF_MMP] - (pDyna->m_xVolume.GetValue() - pOldDyna->m_xVolume.GetValue());
			}

			CBigInt xV = 0;

			bool bMaxVChange = stream.ReadDWORD(1);
			if (!bMaxVChange) {
				int nNumOfVChange = stream.ReadDWORD(2);
				for (int i = 0; i < nNumOfVChange; i++) {
					int nVPos = stream.ReadDWORD(POS_BITS);
					stream.DecodeBigInt(xV, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS);
					phMMPVol[nVPos] = phMMPVol[nVPos] + xV.GetValue();
				}
			}
			else {
				UINT32 dwVolChange = stream.ReadDWORD(cNumBuy + cNumSell);
				dwVolChange <<= (NUM_OF_MMP - cNumSell);

				for (int i = 0; i < nArraySize; i++) {
					if (dwVolChange & (1 << i)) {
						stream.DecodeBigInt(xV, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS);
						phMMPVol[i] = phMMPVol[i] + xV.GetValue();
					}
				}
			}

			for (int i = 0; i < nArraySize; i++)        // to make the volume be in the format of CBigInt
				pxMMPVol[i] = phMMPVol[i];
			delete[] phMMPVol;
		}// mmp changed
	}
	else {
		// mmp consts
		int NUM_OF_MMP = 0;
		if (typeid (T) == typeid (CStdCacheQt)) {
			NUM_OF_MMP = 5;
		}
		if (typeid (T) == typeid (CStdCacheQtS)) {
			NUM_OF_MMP = 10;
		}
		int MAX_PCHANGE = 3;
		int MAX_VCHANGE = 2;
		int POS_BITS = NUM_OF_MMP - 1;
		int nArraySize = NUM_OF_MMP * 2;

		// mmp arrays
		PDWORD pdwMMP;
		CBigInt * pxMMPVol;


		{
			pdwMMP = pDyna->m_pdwMMP;
			pxMMPVol = pDyna->m_pxMMPVol;
		}


		BYTE& cNumBuy = pDyna->m_cNumBuy;
		BYTE& cNumSell = pDyna->m_cNumSell;

		// old mmp arrays
		const UINT32 * adwLastMMPPrice;
		const CBigInt * axLastMMPVol;

		BYTE cNumLastBuy = pOldDyna->m_cNumBuy;
		BYTE cNumLastSell = pOldDyna->m_cNumSell;


		{
			adwLastMMPPrice = pOldDyna->m_pdwMMP;
			axLastMMPVol = pOldDyna->m_pxMMPVol;

			if (cNumLastBuy > NUM_OF_MMP)
				cNumLastBuy = NUM_OF_MMP;
			if (cNumLastSell > NUM_OF_MMP)
				cNumLastSell = NUM_OF_MMP;
		}

		const BYTE MMP_NOCHANGE = 0;	// 00
		const BYTE MMP_VCHANGE = 1;		// 01
		const BYTE MMP_PVCHANGE = 2;	// 10
		const BYTE MMP_PPVCHANGE = 3;	// 11, encode whole price array

		BYTE cPVchange = (BYTE)stream.ReadDWORD(2);
		if (cPVchange == MMP_NOCHANGE)
		{
			memcpy(pdwMMP, adwLastMMPPrice, nArraySize*sizeof(UINT32));
			memcpy(pxMMPVol, axLastMMPVol, nArraySize*sizeof(CBigInt));

			cNumBuy = cNumLastBuy;
			cNumSell = cNumLastSell;
		}
		else
		{
			INT64* phMMPVol = new INT64[nArraySize];
			memset(phMMPVol, 0, sizeof(INT64) * nArraySize);

			if (cPVchange == MMP_VCHANGE)
			{
				memcpy(pdwMMP, adwLastMMPPrice, nArraySize*sizeof(UINT32));

				cNumBuy = cNumLastBuy;
				cNumSell = cNumLastSell;
			}
			else	// pv/ppv
			{
				bool bNumBuySell = stream.ReadDWORD(1);
				if (bNumBuySell)
				{
					cNumBuy = (BYTE)stream.ReadDWORD(POS_BITS - 1);
					cNumSell = (BYTE)stream.ReadDWORD(POS_BITS - 1);
				}
				else
				{
					cNumBuy = NUM_OF_MMP;
					cNumSell = NUM_OF_MMP;
				}

				if (cPVchange == MMP_PVCHANGE)
				{
					int nModCode = stream.ReadDWORD(4);
					if (nModCode < 8)	// special case, just insert one price
					{
						int nInsPos = -1;
						int nDelPos = -1;

						if (nModCode == 0)		// ins buy5/10, del sell 5/10
						{
							nInsPos = 0;
							nDelPos = nArraySize - 1;
						}
						else if (nModCode == 1)	// ins sell1,   del sell 5/10
						{
							nInsPos = NUM_OF_MMP;
							nDelPos = nArraySize - 1;
						}
						else if (nModCode == 2)	// ins buy5/10,   del buy1
						{
							nInsPos = 0;
							nDelPos = NUM_OF_MMP - 1;
						}
						else if (nModCode == 3)	// del buy5/10,   ins buy1
						{
							nInsPos = NUM_OF_MMP - 1;
							nDelPos = 0;
						}
						else if (nModCode == 4)	// del sell1,   ins sell5/10
						{
							nInsPos = nArraySize - 1;
							nDelPos = NUM_OF_MMP;
						}
						else if (nModCode == 5)	// del buy5/10,   ins sell5/10
						{
							nInsPos = nArraySize - 1;
							nDelPos = 0;
						}

						memcpy(pdwMMP, adwLastMMPPrice, nArraySize*sizeof(UINT32));
						if (nInsPos < nDelPos)
							memmove(&pdwMMP[nInsPos + 1], &pdwMMP[nInsPos], (nDelPos - nInsPos)*sizeof(UINT32));
						else
							memmove(&pdwMMP[nDelPos], &pdwMMP[nDelPos + 1], (nInsPos - nDelPos)*sizeof(UINT32));

						UINT32 dwPriceInsGap = 0;
						stream.DecodeData(dwPriceInsGap, BC_DYNA_PRICE_INS, BCNUM_DYNA_PRICE_INS);
						if (dwPriceInsGap == 999999)
							pdwMMP[nInsPos] = 0;
						else
						{
							if (nInsPos > 0)
								pdwMMP[nInsPos] = pdwMMP[nInsPos - 1] + dwPriceInsGap;
							else
								pdwMMP[0] = pdwMMP[1] - dwPriceInsGap;
						}

						for (int i = NUM_OF_MMP + cNumSell; i < nArraySize; i++)
							pdwMMP[i] = 0;
					}
					else	// decode price change one by one
					{
						int nNumOfPChange = nModCode - 8;

						if (nNumOfPChange == 0)
							memcpy(pdwMMP, adwLastMMPPrice, (NUM_OF_MMP + cNumSell)*sizeof(UINT32));
						else
						{
							const BYTE MOD_INSERT = 0;		// 0
							const BYTE MOD_DELETE = 1;		// 1

							int nChngPos = 0;

							int nPricePos = stream.ReadDWORD(POS_BITS);
							int nPriceModType = stream.ReadDWORD(1);
							UINT32 dwPriceInsGap = 0;
							if (nPriceModType == MOD_INSERT)
								stream.DecodeData(dwPriceInsGap, BC_DYNA_PRICE_INS, BCNUM_DYNA_PRICE_INS);

							int nPos = 0;
							int nLastPos = 0;

							int nTop = NUM_OF_MMP + cNumSell;
							int nLastTop = NUM_OF_MMP + cNumLastSell;

							while (nPos < nTop)
							{
								bool bChange = false;

								if (nPricePos == nLastPos && nPriceModType == MOD_DELETE)
								{
									nLastPos++;
									bChange = true;
								}
								else if (nPricePos == nPos && nPriceModType == MOD_INSERT)
								{
									if (dwPriceInsGap == 999999)
										pdwMMP[nPos] = 0;
									else
									{
										if (nPos > 0)
											pdwMMP[nPos] = pdwMMP[nPos - 1] + dwPriceInsGap;
										else
											pdwMMP[0] = pDyna->m_dwPrice - dwPriceInsGap;
									}

									nPos++;

									bChange = true;
								}
								else
								{

									if (nLastPos < nLastTop)
										pdwMMP[nPos] = adwLastMMPPrice[nLastPos];

									nPos++;
									nLastPos++;
								}

								if (bChange)
								{
									nChngPos++;
									if (nChngPos < nNumOfPChange)	// read next one
									{
										nPricePos = stream.ReadDWORD(POS_BITS);
										nPriceModType = stream.ReadDWORD(1);
										dwPriceInsGap = 0;
										if (nPriceModType == MOD_INSERT)
											stream.DecodeData(dwPriceInsGap, BC_DYNA_PRICE_INS, BCNUM_DYNA_PRICE_INS);
									}
									else		// change list end
										nPricePos = -1;
								}
							}
						}// nNumOfPChange > 0
					}// decode price change one by one
				}// cPVchange == MMP_PVCHANGE
				else	// cPVchange == MMP_PPVCHANGE
				{
					DecodeAllMMPPrice<T>(stream, NUM_OF_MMP, cNumBuy, cNumSell, pdwMMP, pDyna->m_dwPrice);
				}// cPVchange == MMP_PPVCHANGE
			}// p changed

			// volume decoding
			if (cNumLastBuy > 0 || cNumLastSell > 0)
			{
				int nPos = NUM_OF_MMP - cNumBuy;
				int nLastPos = NUM_OF_MMP - cNumLastBuy;

				while (nPos < NUM_OF_MMP + cNumSell && nLastPos < NUM_OF_MMP + cNumLastSell)
				{
					if (pdwMMP[nPos] == adwLastMMPPrice[nLastPos])
					{
						if (nPos < NUM_OF_MMP && nLastPos < NUM_OF_MMP
							|| nPos >= NUM_OF_MMP && nLastPos >= NUM_OF_MMP)
							phMMPVol[nPos] = axLastMMPVol[nLastPos];

						nPos++;
						nLastPos++;
					}
					else if (pdwMMP[nPos] > adwLastMMPPrice[nLastPos])	// delete
						nLastPos++;
					else	// pdwMMP[nPos] < adwLastMMPPrice[nLastPos] // insert
						nPos++;
				}

				if (pdwMMP[NUM_OF_MMP - 1] == pDyna->m_dwPrice)
					phMMPVol[NUM_OF_MMP - 1] = phMMPVol[NUM_OF_MMP - 1] - (pDyna->m_xVolume.GetValue() - pOldDyna->m_xVolume.GetValue());
				else if (pdwMMP[NUM_OF_MMP] == pDyna->m_dwPrice)
					phMMPVol[NUM_OF_MMP] = phMMPVol[NUM_OF_MMP] - (pDyna->m_xVolume.GetValue() - pOldDyna->m_xVolume.GetValue());
			}

			CBigInt xV = 0;

			bool bMaxVChange = stream.ReadDWORD(1);
			if (!bMaxVChange)
			{
				int nNumOfVChange = stream.ReadDWORD(2);
				for (int i = 0; i < nNumOfVChange; i++)
				{
					int nVPos = stream.ReadDWORD(POS_BITS);
					stream.DecodeBigInt(xV, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS);
					phMMPVol[nVPos] = phMMPVol[nVPos] + xV.GetValue();
				}
			}
			else
			{
				UINT32 dwVolChange = stream.ReadDWORD(cNumBuy + cNumSell);
				dwVolChange <<= (NUM_OF_MMP - cNumBuy);

				for (int i = 0; i < nArraySize; i++)
				{
					if (dwVolChange & (1 << i))
					{
						stream.DecodeBigInt(xV, BC_DYNA_VOL_DIFFS, BCNUM_DYNA_VOL_DIFFS);
						phMMPVol[i] = phMMPVol[i] + xV.GetValue();
					}
				}
			}

			for (int i = 0; i < nArraySize; i++)
				pxMMPVol[i] = phMMPVol[i];
			delete[] phMMPVol;
		}// mmp changed
	}
}
template void CMultiQtCompressUnit::ExpandMMP<CStdCacheQt>(CBitStream&, CStdCacheQt*, const CStdCacheQt*, const UINT16);
template void CMultiQtCompressUnit::ExpandMMP<CStdCacheQtS>(CBitStream&, CStdCacheQtS*, const CStdCacheQtS*, const UINT16);

int CMultiMxCompressUnit::ExpandData(CBitStream& stream, vector<CStdCacheMx> & result, const unsigned short type)
{
	UINT32 dwNum = ExpandNum(stream);
	if (dwNum == 0)
		return 0;

	BYTE cCheckSum = 0;

	try
	{
		UINT32 dwCurPrice;
		UINT32 dwLastPrice;
		CStdCacheMx data;
		for (UINT32 dw = 0; dw < dwNum; dw++)
		{
			if (dw == 0)
			{
				data.m_dwDate = stream.ReadDWORD(26);
				data.m_dwTime = stream.ReadDWORD(18);
				stream.DecodeData(dwCurPrice, BC_TRADE_PRICE, BCNUM_TRADE_PRICE);
			}
			else
			{
				if (stream.ReadDWORD(1))
				{
					data.m_dwDate = stream.ReadDWORD(26);
					data.m_dwTime = stream.ReadDWORD(18);
				}
				else
				{
					UINT32 dwSecondsDiff = 0;
					stream.DecodeData(dwSecondsDiff, BC_BARGAIN_TIMEDIFF, BCNUM_BARGAIN_TIMEDIFF);
					data.m_dwTime = AddSeconds(data.m_dwTime, dwSecondsDiff);
				}

				stream.DecodeData(dwCurPrice, BC_TRADE_PRICE_DIFFS, BCNUM_TRADE_PRICE_DIFFS, &dwLastPrice);
			}

			stream.DecodeBigInt(data.m_xVolume, BC_BARGAIN_VOL, BCNUM_BARGAIN_VOL);
			stream.DecodeData(data.m_dwTradeNum, BC_TRADE_TRADENUM, BCNUM_TRADE_TRADENUM);
			if (stream.ReadDWORD(1))
			{
				UINT32 dwTmp = 0;
				stream.DecodeData(dwTmp, BC_TRADE_TRADENUM, BCNUM_TRADE_TRADENUM);
				data.m_xOI = -(int)dwTmp;
			}
			else
			{
				UINT32 dwTmp = 0;
				stream.DecodeData(dwTmp, BC_TRADE_TRADENUM, BCNUM_TRADE_TRADENUM);
				data.m_xOI = dwTmp;
			}

			if (type == 1)
			{
				data.m_cBS = stream.ReadDWORD(2) + 1;
			}
			else if (type == 2)
			{
				stream.DecodeData(data.m_cBS, BC_TRADE_PRICE, BCNUM_TRADE_PRICE);
			}
			else
			{
			}

			data.m_dwPrice = dwCurPrice;
			result.push_back(data);
			cCheckSum += data.GetCheckSum();

			dwLastPrice = dwCurPrice;
		}

		BYTE cCheck = (BYTE)stream.ReadDWORD(8);
		if (cCheck != cCheckSum)
			return ERR_COMPRESS_CHECK;
	}
	catch (int)
	{
		return -2;
	}

	return 0;
}

template <typename Container>
int CMultiMxCompressUnit::CompressData(COutputBitStream& stream,
                                       const Container &c,
                                       size_t offset,
                                       size_t size,
                                       unsigned short type)
{
	CompressNum(stream, size);
	BYTE cCheckSum = 0;
	if (size == 0) {
		return 0;
	}

	try {
        size_t last_index = offset;
		UINT32 dwLastPrice = 0;

		for (size_t dw = offset; dw < offset + size; dw++) {
            const CStdCacheMx &cur_mx = c[dw];
			UINT32 dwCurPrice = cur_mx.m_dwPrice;

			if (dw == offset) {
				stream.WriteDWORD(cur_mx.m_dwDate, 26);
				stream.WriteDWORD(cur_mx.m_dwTime, 18);
				stream.EncodeData(dwCurPrice, BC_TRADE_PRICE, BCNUM_TRADE_PRICE);
			}
			else {
                const CStdCacheMx &last_mx = c[last_index];
				if (cur_mx.m_dwDate == last_mx.m_dwDate) {
					stream.WriteBool(false);
					UINT32 dwSecondsDiff = GetSecondsDiff(last_mx.m_dwTime, cur_mx.m_dwTime);
					stream.EncodeData(dwSecondsDiff, BC_BARGAIN_TIMEDIFF, BCNUM_BARGAIN_TIMEDIFF);
				}
				else {
					stream.WriteBool(true);
					stream.WriteDWORD(cur_mx.m_dwDate, 26);
					stream.WriteDWORD(cur_mx.m_dwTime, 18);
				}

				stream.EncodeData(dwCurPrice, BC_TRADE_PRICE_DIFFS, BCNUM_TRADE_PRICE_DIFFS, &dwLastPrice);
			}

			stream.EncodeBigInt(cur_mx.m_xVolume, BC_BARGAIN_VOL, BCNUM_BARGAIN_VOL);
			stream.EncodeData(cur_mx.m_dwTradeNum, BC_TRADE_TRADENUM, BCNUM_TRADE_TRADENUM);
			if (cur_mx.m_xOI > 0) {
				stream.WriteBool(false);
				stream.EncodeData(cur_mx.m_xOI, BC_TRADE_TRADENUM, BCNUM_TRADE_TRADENUM);
			}
			else {
				stream.WriteBool(true);
				UINT32 tmp = -cur_mx.m_xOI;
				stream.EncodeData(tmp, BC_TRADE_TRADENUM, BCNUM_TRADE_TRADENUM);
			}

			if (type == 1) {
				stream.WriteDWORD(cur_mx.m_cBS - 1, 2);
			}
			else if (type == 2) {
				stream.EncodeData(cur_mx.m_cBS, BC_TRADE_PRICE, BCNUM_TRADE_PRICE);
			}
			else {
			}

			cCheckSum += cur_mx.GetCheckSum();
			dwLastPrice = dwCurPrice;
			last_index = dw;
		}

		stream.WriteDWORD(cCheckSum, 8);
	}
	catch (int) {
		return -2;
	}

	return 0;
}

int CMultiMxCompressUnit::CompressData(COutputBitStream& stream, const CStdCacheMx* pMinute, UINT32 size, const unsigned short type)
{
    return CMultiMxCompressUnit::CompressData(stream, pMinute, 0, size, type);
}

template
int CMultiMxCompressUnit::CompressData<std::deque<CStdCacheMx> >(
    COutputBitStream& stream, const std::deque<CStdCacheMx> &mxs,
    size_t offset, size_t size, const unsigned short type);

void CMultiMxCompressUnit::CompressNum(COutputBitStream& stream, UINT32 dwNum, const UINT16 wVer) {
	stream.EncodeData(dwNum, BC_TRADE_NUMBER, BCNUM_TRADE_NUMBER);
}

UINT32 CMultiMxCompressUnit::ExpandNum(CBitStream& stream, const UINT16 wVer) {
	UINT32 dwNum = 0;
	stream.DecodeData(dwNum, BC_TRADE_NUMBER, BCNUM_TRADE_NUMBER);
	return dwNum;
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheFFDeal* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        stream.EncodeData(ptrNewData->m_dwNumOfBuy, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.EncodeData(ptrNewData->m_dwNumOfSell, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.EncodeBigInt(ptrNewData->m_xVolOfBuy, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.EncodeBigInt(ptrNewData->m_xVolOfSell, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.EncodeBigInt(ptrNewData->m_xAmtOfBuy, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.EncodeBigInt(ptrNewData->m_xAmtOfSell, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::COMPRESS_SUCCESS;
    }

	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheFFDeal* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        stream.DecodeData(ptrNewData->m_dwNumOfBuy, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.DecodeData(ptrNewData->m_dwNumOfSell, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.DecodeBigInt(ptrNewData->m_xVolOfBuy, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.DecodeBigInt(ptrNewData->m_xVolOfSell, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.DecodeBigInt(ptrNewData->m_xAmtOfBuy, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.DecodeBigInt(ptrNewData->m_xAmtOfSell, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::UNCOMPRESS_SUCCESS;
    }

	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheFFDeal* ptrNewData, const CStdCacheFFDeal* ptrOldData, \
	unsigned short version) {
    if (version == MCM_VERSION) {
        stream.EncodeData(ptrNewData->m_dwNumOfBuy, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_dwNumOfBuy));
        stream.EncodeData(ptrNewData->m_dwNumOfSell, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_dwNumOfSell));
        stream.EncodeBigInt(ptrNewData->m_xVolOfBuy, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_xVolOfBuy));
        stream.EncodeBigInt(ptrNewData->m_xVolOfSell, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_xVolOfSell));
        stream.EncodeBigInt(ptrNewData->m_xAmtOfBuy, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_xAmtOfBuy));
        stream.EncodeBigInt(ptrNewData->m_xAmtOfSell, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_xAmtOfSell));

        return McmStatus::COMPRESS_SUCCESS;
    }

	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheFFDeal* ptrNewData, const CStdCacheFFDeal* ptrOldData, \
	const unsigned short version) {
    if (version == MCM_VERSION) {
        stream.DecodeData(ptrNewData->m_dwNumOfBuy, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_dwNumOfBuy));
        stream.DecodeData(ptrNewData->m_dwNumOfSell, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_dwNumOfSell));
        stream.DecodeBigInt(ptrNewData->m_xVolOfBuy, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_xVolOfBuy));
        stream.DecodeBigInt(ptrNewData->m_xVolOfSell, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_xVolOfSell));
        stream.DecodeBigInt(ptrNewData->m_xAmtOfBuy, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_xAmtOfBuy));
        stream.DecodeBigInt(ptrNewData->m_xAmtOfSell, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_xAmtOfSell));

        return McmStatus::UNCOMPRESS_SUCCESS;
    }

	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheFF* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        for (int i = 0; i < FFOrderType::MAX_ORDER; ++i) {
            CompressSnapData(stream, &(ptrNewData->orders[i]), version);
        }

        return McmStatus::COMPRESS_SUCCESS;
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheFF* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        for (int i = 0; i < FFOrderType::MAX_ORDER; ++i) {
            ExpandSnapData(stream, &(ptrNewData->orders[i]), version);
        }

        return McmStatus::UNCOMPRESS_SUCCESS;
    }

	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheFF* ptrNewData, const CStdCacheFF* ptrOldData, \
	unsigned short version) {
    if (version == MCM_VERSION) {
        for (int i = 0; i < FFOrderType::MAX_ORDER; ++i) {
            CompressAddData(stream, &(ptrNewData->orders[i]), &(ptrOldData->orders[i]), version);
        }

        return McmStatus::COMPRESS_SUCCESS;
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheFF* ptrNewData, const CStdCacheFF* ptrOldData, \
	const unsigned short version) {
    if (version == MCM_VERSION) {
        for (int i = 0; i < FFOrderType::MAX_ORDER; ++i) {
            ExpandAddData(stream, &(ptrNewData->orders[i]), &(ptrOldData->orders[i]), version);
        }

        return McmStatus::UNCOMPRESS_SUCCESS;
    }

	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheMinuteFF* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        CompressSnapData(stream, &(ptrNewData->m_flow), version);
        stream.EncodeData(ptrNewData->m_wTime, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::COMPRESS_SUCCESS;
    }

	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheMinuteFF* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        ExpandSnapData(stream, &(ptrNewData->m_flow), version);
        stream.DecodeData(ptrNewData->m_wTime, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::UNCOMPRESS_SUCCESS;
    }

	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheMinuteFF* ptrNewData, const CStdCacheMinuteFF* ptrOldData, \
	const unsigned short version) {
    if (version == MCM_VERSION) {
        CompressAddData(stream, &(ptrNewData->m_flow), &(ptrOldData->m_flow), version);
        stream.EncodeData(ptrNewData->m_wTime, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_wTime));

        return McmStatus::COMPRESS_SUCCESS;
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheMinuteFF* ptrNewData, const CStdCacheMinuteFF* ptrOldData, \
	const unsigned short version) {
    if (version == MCM_VERSION) {
        ExpandAddData(stream, &(ptrNewData->m_flow), &(ptrOldData->m_flow), version);
        stream.DecodeData(ptrNewData->m_wTime, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_wTime));

        return McmStatus::UNCOMPRESS_SUCCESS;
    }
	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheFFTrend* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        CompressSnapData(stream, &(ptrNewData->m_flow), version);
        stream.EncodeData(ptrNewData->m_wTime, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::COMPRESS_SUCCESS;
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheFFTrend* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        ExpandSnapData(stream, &(ptrNewData->m_flow), version);
        stream.DecodeData(ptrNewData->m_wTime, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::UNCOMPRESS_SUCCESS;
    }
	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheFFTrend* ptrNewData, const CStdCacheFFTrend* ptrOldData, \
	const unsigned short version) {
    if (version == MCM_VERSION) {
        CompressAddData(stream, &(ptrNewData->m_flow), &(ptrOldData->m_flow), version);
        stream.EncodeData(ptrNewData->m_wTime, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_wTime));

        return McmStatus::COMPRESS_SUCCESS;
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheFFTrend* ptrNewData, const CStdCacheFFTrend* ptrOldData, \
	const unsigned short version) {
    if (version == MCM_VERSION) {
        ExpandAddData(stream, &(ptrNewData->m_flow), &(ptrOldData->m_flow), version);
        stream.DecodeData(ptrNewData->m_wTime, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->m_wTime));

        return McmStatus::UNCOMPRESS_SUCCESS;
    }
	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheFFBriefInfo* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        CompactMemory::encodeMultipleUnit(stream, (CBigInt*)ptrNewData->orders, 4 * FFOrderType::MAX_ORDER, \
            GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::COMPRESS_SUCCESS;
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheFFBriefInfo* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        CompactMemory::decodeMultipleUnit(stream, (CBigInt*)ptrNewData->orders, 4 * FFOrderType::MAX_ORDER, \
        	GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::UNCOMPRESS_SUCCESS;
    }
	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheFFBriefInfo* ptrNewData, const CStdCacheFFBriefInfo* ptrOldData, \
	const unsigned short version) {
    if (version == MCM_VERSION) {
        CompactMemory::encodeMultipleUnit(stream, (CBigInt*)ptrNewData->orders, 4 * FFOrderType::MAX_ORDER, \
            GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, (CBigInt*)ptrOldData->orders);

        return McmStatus::COMPRESS_SUCCESS;
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheFFBriefInfo* ptrNewData, const CStdCacheFFBriefInfo* ptrOldData, \
	const unsigned short version) {
    if (version == MCM_VERSION) {
        CompactMemory::decodeMultipleUnit(stream, (CBigInt*)ptrNewData->orders, 4 * FFOrderType::MAX_ORDER, \
        	GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, (CBigInt*)ptrOldData->orders);

        return McmStatus::UNCOMPRESS_SUCCESS;
    }
	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheListBuyinRank* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        for (int i = 0; i < FFDayType::MAX_DAY; ++i) {
            stream.EncodeData(ptrNewData->days[i].prevClose, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
            CompactMemory::encodeMultipleUnit(stream, (INT32*)&ptrNewData->days[i].buyinRatio, 3, \
                GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER);
        }
        stream.EncodeData(ptrNewData->price, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::COMPRESS_SUCCESS;
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheListBuyinRank* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        for (int i = 0; i < FFDayType::MAX_DAY; ++i) {
            stream.DecodeData(ptrNewData->days[i].prevClose, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
            CompactMemory::decodeMultipleUnit(stream, (INT32*)&ptrNewData->days[i].buyinRatio, 3, \
                GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER);
        }
        stream.DecodeData(ptrNewData->price, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::UNCOMPRESS_SUCCESS;
    }
	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheListBuyinRank* ptrNewData, const CStdCacheListBuyinRank* ptrOldData,\
    const unsigned short version) {
    if (version == MCM_VERSION) {
        for (int i = 0; i < FFDayType::MAX_DAY; ++i) {
            stream.EncodeData(ptrNewData->days[i].prevClose, \
                GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->days[i].prevClose));
            CompactMemory::encodeMultipleUnit(stream, (INT32*)&ptrNewData->days[i].buyinRatio, 3, \
                GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, (INT32*)&ptrOldData->days[i].buyinRatio);
        }
        stream.EncodeData(ptrNewData->price, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->price));

        return McmStatus::COMPRESS_SUCCESS;
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheListBuyinRank* ptrNewData, const CStdCacheListBuyinRank* ptrOldData,\
    const unsigned short version) {
    if (version == MCM_VERSION) {
        for (int i = 0; i < FFDayType::MAX_DAY; ++i) {
            stream.DecodeData(ptrNewData->days[i].prevClose, \
                GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->days[i].prevClose));
            CompactMemory::decodeMultipleUnit(stream, (INT32*)&ptrNewData->days[i].buyinRatio, 3, \
                GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, (INT32*)&ptrOldData->days[i].buyinRatio);
        }
        stream.DecodeData(ptrNewData->price, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldData->price));

        return McmStatus::UNCOMPRESS_SUCCESS;
    }
	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheDayFF* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        CompressSnapData(stream, &ptrNewData->detail, version);
        CompactMemory::encodeMultipleUnit(stream, ptrNewData->day_ranks, FFDayType::MAX_DAY,
            GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        CompactMemory::encodeMultipleUnit(stream, ptrNewData->day_ratios, FFDayType::MAX_DAY,
            GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::COMPRESS_SUCCESS;
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheDayFF* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        ExpandSnapData(stream, &ptrNewData->detail, version);
        CompactMemory::decodeMultipleUnit(stream, ptrNewData->day_ranks, FFDayType::MAX_DAY,
            GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        CompactMemory::decodeMultipleUnit(stream, ptrNewData->day_ratios, FFDayType::MAX_DAY,
            GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::UNCOMPRESS_SUCCESS;
    }
	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheDayFF* ptrNewData, const CStdCacheDayFF* ptrOldData, \
	const unsigned short version) {
    if (version == MCM_VERSION) {
        CompressAddData(stream, &ptrNewData->detail, &ptrOldData->detail, version);
        CompactMemory::encodeMultipleUnit(stream, ptrNewData->day_ranks, FFDayType::MAX_DAY,
            GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, ptrOldData->day_ranks);
        CompactMemory::encodeMultipleUnit(stream, ptrNewData->day_ratios, FFDayType::MAX_DAY,
            GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, ptrOldData->day_ratios);

        return McmStatus::COMPRESS_SUCCESS;
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheDayFF* ptrNewData, const CStdCacheDayFF* ptrOldData, \
	const unsigned short version) {
    if (version == MCM_VERSION) {
        ExpandAddData(stream, &ptrNewData->detail, &ptrOldData->detail, version);
        CompactMemory::decodeMultipleUnit(stream, ptrNewData->day_ranks, FFDayType::MAX_DAY,
            GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, ptrOldData->day_ranks);
        CompactMemory::decodeMultipleUnit(stream, ptrNewData->day_ratios, FFDayType::MAX_DAY,
            GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, ptrOldData->day_ratios);

        return McmStatus::UNCOMPRESS_SUCCESS;
    }
	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheListFFItem* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        CompressSnapData(stream, &(ptrNewData->flow), version);
        stream.EncodeBigInt(ptrNewData->callAuctionAmount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.EncodeData(ptrNewData->price, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.EncodeData(ptrNewData->prevClose, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::COMPRESS_SUCCESS;
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheListFFItem* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        ExpandSnapData(stream, &(ptrNewData->flow), version);
        stream.DecodeBigInt(ptrNewData->callAuctionAmount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.DecodeData(ptrNewData->price, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.DecodeData(ptrNewData->prevClose, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);

        return McmStatus::UNCOMPRESS_SUCCESS;
    }
	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheListFFItem* ptrNewData, const CStdCacheListFFItem* ptrOldData,\
    const unsigned short version) {
    if (version == MCM_VERSION) {
        CompressAddData(stream, &(ptrNewData->flow), &(ptrOldData->flow), version);
        stream.EncodeBigInt(ptrNewData->callAuctionAmount, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &(ptrOldData->callAuctionAmount));
        stream.EncodeData(ptrNewData->price, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &(ptrOldData->price));
        stream.EncodeData(ptrNewData->prevClose, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &(ptrOldData->prevClose));

        return McmStatus::COMPRESS_SUCCESS;
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheListFFItem* ptrNewData, const CStdCacheListFFItem* ptrOldData,\
    const unsigned short version) {
    if (version == MCM_VERSION) {
        ExpandAddData(stream, &(ptrNewData->flow), &(ptrOldData->flow), version);
        stream.DecodeBigInt(ptrNewData->callAuctionAmount, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &(ptrOldData->callAuctionAmount));
        stream.DecodeData(ptrNewData->price, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &(ptrOldData->price));
        stream.DecodeData(ptrNewData->prevClose, GENERAL_UNSIGNED_DATA_DIFF, GENERAL_UNSIGNED_DATA_DIFF_NUMBER, &(ptrOldData->prevClose));

        return McmStatus::UNCOMPRESS_SUCCESS;
    }
	return McmStatus::UNCOMPRESS_FAIL;
}

int encodeMarketTurnoverInternalItem(COutputBitStream& stream, const CStdCacheMarketTurnover::Item& newItem, const CStdCacheMarketTurnover::Item* ptrOldItem=NULL) {
    if (ptrOldItem) {
        CompactMemory::encodeDate(stream, newItem.date_in_yymmdd, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldItem->date_in_yymmdd));
        CompactMemory::encodeTime(stream, newItem.time_in_hhmmss, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldItem->time_in_hhmmss));
        stream.EncodeBigInt(newItem.buyin_amount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldItem->buyin_amount));
        stream.EncodeBigInt(newItem.sold_amount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldItem->sold_amount));
        stream.EncodeBigInt(newItem.total_amount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldItem->total_amount));
    } else {
        CompactMemory::encodeDate(stream, newItem.date_in_yymmdd, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        CompactMemory::encodeTime(stream, newItem.time_in_hhmmss, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.EncodeBigInt(newItem.buyin_amount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.EncodeBigInt(newItem.sold_amount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.EncodeBigInt(newItem.total_amount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
    }
    return McmStatus::COMPRESS_SUCCESS;
}

int decodeMarketTurnoverInternalItem(CBitStream& stream, CStdCacheMarketTurnover::Item& newItem, const CStdCacheMarketTurnover::Item* ptrOldItem=NULL) {
    if (ptrOldItem) {
        CompactMemory::decodeDate(stream, newItem.date_in_yymmdd, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldItem->date_in_yymmdd));
        CompactMemory::decodeTime(stream, newItem.time_in_hhmmss, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldItem->time_in_hhmmss));
        stream.DecodeBigInt(newItem.buyin_amount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldItem->buyin_amount));
        stream.DecodeBigInt(newItem.sold_amount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldItem->sold_amount));
        stream.DecodeBigInt(newItem.total_amount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER, &(ptrOldItem->total_amount));
    } else {
        CompactMemory::decodeDate(stream, newItem.date_in_yymmdd, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        CompactMemory::decodeTime(stream, newItem.time_in_hhmmss, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.DecodeBigInt(newItem.buyin_amount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.DecodeBigInt(newItem.sold_amount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
        stream.DecodeBigInt(newItem.total_amount, GENERAL_UNSIGNED_DATA_MATCH, GENERAL_UNSIGNED_DATA_MATCH_NUMBER);
    }
    return McmStatus::UNCOMPRESS_SUCCESS;    
}

int CMultiQtCompressUnit::CompressSnapData(COutputBitStream& stream, const CStdCacheMarketTurnover* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        if (encodeMarketTurnoverInternalItem(stream, ptrNewData->north_turnover) &&
            encodeMarketTurnoverInternalItem(stream, ptrNewData->south_turnover)) {
            return McmStatus::COMPRESS_SUCCESS;
        }
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandSnapData(CBitStream& stream, CStdCacheMarketTurnover* ptrNewData, const unsigned short version) {
    if (version == MCM_VERSION) {
        if (decodeMarketTurnoverInternalItem(stream, ptrNewData->north_turnover) &&
            decodeMarketTurnoverInternalItem(stream, ptrNewData->south_turnover)) {
            return McmStatus::UNCOMPRESS_SUCCESS;
        }
    }
	return McmStatus::UNCOMPRESS_FAIL;
}

int CMultiQtCompressUnit::CompressAddData(COutputBitStream& stream, const CStdCacheMarketTurnover* ptrNewData, const CStdCacheMarketTurnover* ptrOldData,\
    const unsigned short version) {
    if (version == MCM_VERSION) {
        if (encodeMarketTurnoverInternalItem(stream, ptrNewData->north_turnover, &(ptrOldData->north_turnover)) &&
            encodeMarketTurnoverInternalItem(stream, ptrNewData->south_turnover, &(ptrOldData->south_turnover))) {
            return McmStatus::COMPRESS_SUCCESS;
        }
    }
	return McmStatus::COMPRESS_FAIL;
}

int CMultiQtCompressUnit::ExpandAddData(CBitStream& stream, CStdCacheMarketTurnover* ptrNewData, const CStdCacheMarketTurnover* ptrOldData,\
    const unsigned short version) {
    if (version == MCM_VERSION) {
        if (decodeMarketTurnoverInternalItem(stream, ptrNewData->north_turnover, &(ptrOldData->north_turnover)) &&
            decodeMarketTurnoverInternalItem(stream, ptrNewData->south_turnover, &(ptrOldData->south_turnover))) {
            return McmStatus::UNCOMPRESS_SUCCESS;
        }
    }
	return McmStatus::UNCOMPRESS_FAIL;
}

