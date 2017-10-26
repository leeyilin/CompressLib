
#include <map>
#include <set>
#include <string.h>

#include "MultiCompressObj.h"
#include "MultiCompress.h"
#include "MultiCompressUnit.h"

CRawMultiQt Convert(const CStdCacheQt & src)
{
	CRawMultiQt dest;
	dest.m_dwTradeSequence = src.m_dwTradeSequence;
	dest.m_dwDate = src.m_dwDate;	//交易日YYYYMMDD
	dest.m_dwTime = src.m_dwTime;//交易时间hhmmss
	strncpy(dest.m_pcName, src.m_pcName, sizeof(dest.m_pcName) - 1); //股票名称
	strncpy(dest.m_pchCode, src.m_pchCode, sizeof(dest.m_pchCode) - 1);//股票代码
        dest.m_bytMarket = static_cast<unsigned char>(src.m_wMarket);
        dest.m_bytType = static_cast<unsigned char>(src.m_wType);

	dest.m_bytTradeFlag = src.m_cTradeFlag;
        dest.m_dwClose = src.m_dwClose;	// 前日收盘价(算昨涨跌幅)
	dest.m_dwOpen = src.m_dwOpen;//开盘价
	dest.m_dwHigh = src.m_dwHigh;//最高价
	dest.m_dwLow = src.m_dwLow;//最低价
	dest.m_dwPrice = src.m_dwPrice;//最新价

	dest.m_xVolume = src.m_xVolume;	//成交量
	dest.m_xAmount = src.m_xAmount;//成交额
	dest.m_xTradeNum = src.m_xTradeNum;//成交笔数
	for(size_t i = 0; i < 10; i++)
	{
		dest.m_pdwMMP[i] = src.m_pdwMMP[i];//买卖盘价格
		dest.m_pxMMPVol[i] = src.m_pxMMPVol[i];//买卖盘量
	}

	dest.m_xWaiPan = src.m_xWaiPan;	// 内盘
	dest.m_xCurVol = src.m_xCurVol;	// 现手
	dest.m_cCurVol = src.m_cCurVol;	// 现手方向
	dest.m_xCurOI = src.m_xCurOI;	// 持仓量变化
	dest.m_dwAvg = src.m_dwAvg;

	dest.m_xOpenInterest = src.m_xOpenInterest;	//持仓量
	dest.m_dwSettlementPrice = src.m_dwAvgPrice;
	dest.m_xPreOpenInterest = src.m_xPreOpenInterest;///昨持仓量
	dest.m_dwPreSettlementPrice = src.m_dwPreAvgPrice;

	dest.m_dwPriceZT = src.m_dwPriceZT;		// 涨停价
	dest.m_dwPriceDT = src.m_dwPriceDT;		// 跌停价
	return dest;
}

CRawMultiQtEx Convert(const CStdCacheQtEx& src)
{
	CRawMultiQtEx dest;

    dest.m_bTradingStatus = src.m_bTradingStatus;
    dest.m_dwLotSize = src.m_dwLotSize;

    memcpy(dest.m_cCurrencyCode,
           src.m_cCurrencyCode,
           sizeof(dest.m_cCurrencyCode));

    memcpy(dest.m_cSpreadTable,
           src.m_cSpreadTable,
           sizeof(dest.m_cSpreadTable));

    dest.m_cCASFlag = src.m_cCASFlag;
    dest.m_cVCMFlag = src.m_cVCMFlag;
    dest.m_cShortSellFlag = src.m_cShortSellFlag;
    dest.m_cCCASSFlag = src.m_cCCASSFlag;
    dest.m_cDummySecurityFlag = src.m_cDummySecurityFlag;
    dest.m_cStampDutyFlag = src.m_cStampDutyFlag;
    dest.m_dwListingDate = src.m_dwListingDate;
    dest.m_dwDelistingDate = src.m_dwDelistingDate;
    memcpy(dest.m_cFiller, src.m_cFiller, sizeof(dest.m_cFiller));
    dest.m_cEFNFalg = src.m_cEFNFalg;
    dest.m_dwCouponRate = src.m_dwCouponRate;
    dest.m_dwAccruedInt = src.m_dwAccruedInt;
    dest.m_dwYield = src.m_dwYield;
    dest.m_cCallPutFlag = src.m_cCallPutFlag;
    dest.m_dwConversionRatio = src.m_dwConversionRatio;
    dest.m_dwStrikePrice = src.m_dwStrikePrice;
    dest.m_dwMaturityDate = src.m_dwMaturityDate;

    dest.m_bytUnderlyingMarket = src.m_cUnderlyingMarket;

    memcpy(dest.m_pchUnderlyingCode,
           src.m_pchUnderlyingCode,
           sizeof(dest.m_pchUnderlyingCode));

    dest.m_cStyle = src.m_cStyle;
    dest.m_dwIEPrice = src.m_dwIEPrice;
    dest.m_xIEVolume = src.m_xIEVolume;

    return dest;
}

CRawMultiCAS Convert(const CStdCacheCAS& src)
{
	CRawMultiCAS dest;

    dest.m_cOrderImbalanceDirection = src.m_cOrderImbalanceDirection;
    dest.m_xOrderImbalanceQuantity = src.m_xOrderImbalanceQuantity;
    dest.m_dwReferencePrice = src.m_dwReferencePrice;
    dest.m_dwLowerPrice = src.m_dwLowerPrice;
    dest.m_xUpperPrice = src.m_xUpperPrice;

    return dest;
}

CRawMultiVCM Convert(const CStdCacheVCM& src)
{
	CRawMultiVCM dest;

    dest.m_dwVCMDate = src.m_dwVCMDate;
    dest.m_dwVCMStartTime = src.m_dwVCMStartTime;
    dest.m_dwVCMEndTime = src.m_dwVCMEndTime;
    dest.m_dwVCMReferencePrice = src.m_dwVCMReferencePrice;
    dest.m_xVCMLowerPrice = src.m_xVCMLowerPrice;
    dest.m_xVCMUpperPrice = src.m_xVCMUpperPrice;

    return dest;
}

CRawMultiQtS Convert(const CStdCacheQtS & src)
{
	CRawMultiQtS dest;
	dest.m_dwTradeSequence = src.m_dwTradeSequence;
	dest.m_dwDate = src.m_dwDate;	//交易日YYYYMMDD
	dest.m_dwTime = src.m_dwTime;//交易时间hhmmss
	strncpy(dest.m_pcName, src.m_pcName, sizeof(dest.m_pcName) - 1); //股票名称
	strncpy(dest.m_pchCode, src.m_pchCode, sizeof(dest.m_pchCode) - 1);//股票代码
        dest.m_bytMarket = static_cast<unsigned char>(src.m_wMarket);
        dest.m_bytType = static_cast<unsigned char>(src.m_wType);

	dest.m_bytTradeFlag = src.m_cTradeFlag;
        dest.m_dwClose = src.m_dwClose;	// 前日收盘价(算昨涨跌幅)
	dest.m_dwOpen = src.m_dwOpen;//开盘价
	dest.m_dwHigh = src.m_dwHigh;//最高价
	dest.m_dwLow = src.m_dwLow;//最低价
	dest.m_dwPrice = src.m_dwPrice;//最新价

	dest.m_xVolume = src.m_xVolume;	//成交量
	dest.m_xAmount = src.m_xAmount;//成交额
	dest.m_xTradeNum = src.m_xTradeNum;//成交笔数
	for(size_t i = 0; i < 20; i++)
	{
		dest.m_pdwMMP[i] = src.m_pdwMMP[i];//买卖盘价格
		dest.m_pxMMPVol[i] = src.m_pxMMPVol[i];//买卖盘量
	}

	dest.m_xWaiPan = src.m_xWaiPan;	// 内盘
	dest.m_xCurVol = src.m_xCurVol;	// 现手
	dest.m_cCurVol = src.m_cCurVol;	// 现手方向
	dest.m_xCurOI = src.m_xCurOI;	// 持仓量变化
	dest.m_dwAvg = src.m_dwAvg;

	dest.m_xOpenInterest = src.m_xOpenInterest;	//持仓量
	dest.m_dwSettlementPrice = src.m_dwAvgPrice;
	dest.m_xPreOpenInterest = src.m_xPreOpenInterest;///昨持仓量
	dest.m_dwPreSettlementPrice = src.m_dwPreAvgPrice;

	dest.m_dwPriceZT = src.m_dwPriceZT;		// 涨停价
	dest.m_dwPriceDT = src.m_dwPriceDT;		// 跌停价
	return dest;
}

CRawMultiBlockQt Convert(const CStdCacheBlockQt & src)
{
    CRawMultiBlockQt dest;
    dest.m_dwTradeSequence = src.m_dwTradeSequence;
    dest.m_dwDate = src.m_dwDate;	//交易日YYYYMMDD
    dest.m_dwTime = src.m_dwTime;//交易时间hhmmss
    strncpy(dest.m_pcName, src.m_pcName, sizeof(dest.m_pcName) - 1); //股票名称
    strncpy(dest.m_pchCode, src.m_pchCode, sizeof(dest.m_pchCode) - 1);//股票代码
    dest.m_wMarket = (unsigned char)src.m_wMarket;
    dest.m_wType = (unsigned char)src.m_wType;
    dest.m_dwClose = src.m_dwClose;	// 前日收盘价(算昨涨跌幅)
    dest.m_dwOpen = src.m_dwOpen;//开盘价
    dest.m_dwHigh = src.m_dwHigh;//最高价
    dest.m_dwLow = src.m_dwLow;//最低价
    dest.m_dwPrice = src.m_dwPrice;//最新价

    dest.m_xVolume = src.m_xVolume;	//成交量
    dest.m_xAmount = src.m_xAmount;//成交额

    strncpy(dest.m_pchTopStockCode, src.m_pchTopStockCode, sizeof(dest.m_pchTopStockCode)-1);
    dest.m_pchTopStockMarketID = src.m_pchTopStockMarketID;
    dest.m_dwStockNum = src.m_dwStockNum;
    dest.m_dwUpNum = src.m_dwUpNum;
    dest.m_dwDownNum = src.m_dwDownNum;
    //dest.m_dwStrongNum = src.m_dwStrongNum;
    //dest.m_dwWeakNum = src.m_dwWeakNum;
    dest.m_xZGB = src.m_xZGB;
    dest.m_xZSZ = src.m_xZSZ;
    dest.m_xAvgProfit = src.m_xAvgProfit;
    dest.m_xPeRatio = static_cast<int>(src.m_xPeRatio);
    //dest.m_xAvgAmount5Day = src.m_xAvgAmount5Day;
    //dest.m_xAvgAmount20Day = src.m_xAvgAmount20Day;
    dest.m_xTurnover2Day = src.m_xTurnover2Day;
    //dest.m_xTurnover20Day = src.m_xTurnover20Day;
    dest.m_xPercent3Day = static_cast<unsigned int>(src.m_xPercent3Day);
    //dest.m_xPercent20Day = src.m_xPercent20Day;
    for(int s = 0; s < 5; s++)
    {
        dest.m_dwPre5MinPrice[s] = src.m_dwPre5MinPrice[s];
    }


    return dest;
}

SRawBlockOverView Convert(const CStdCacheBlockOverView & src) {
    SRawBlockOverView dest;
    strncpy(dest.m_pchCode, src.m_pchCode, sizeof(dest.m_pchCode) - 1);//股票代码
    dest.market = src.m_wMarket;
    dest.type = src.m_wType;
    dest.m_xNetFlow = src.m_xNetFlow.GetValue();
    dest.m_dwPercent = src.m_dwPercent;
    dest.m_dwUpDay = src.m_dwUpDay;
    strncpy(dest.m_pchTopStockCode, src.m_pchTopStockCode, sizeof(dest.m_pchTopStockCode)-1);
    dest.m_pchTopStockMarketID = src.m_pchTopStockMarketID;
    dest.m_dwTopPercent = src.m_dwTopPercent;

    return dest;
}

SRawMultiRtMin Convert(const CStdCacheRtMin & src)
{
	SRawMultiRtMin dest;
	dest.m_wTime = src.m_wTime;
	dest.m_dwOpen = src.m_dwOpen;
	dest.m_dwHigh = src.m_dwHigh;
	dest.m_dwLow = src.m_dwLow;
	dest.m_dwClose = src.m_dwClose;
	dest.m_dwAve = src.m_dwAve;
	dest.m_xVolume = src.m_xVolume;
	dest.m_xAmount = src.m_xAmount;
	dest.m_xTradeNum = src.m_xTradeNum;
	dest.m_xWaiPan = src.m_xWaiPan;
	dest.m_xExt1 = src.m_xExt1;
	dest.m_xExt2 = src.m_xExt2;
	dest.m_dwExt1 = src.m_dwExt1;
	dest.m_dwExt2 = src.m_dwExt2;
	return dest;
}

SRawStockMultiMin Convert(const SStockMultiMin & src)
{
	SRawStockMultiMin dest;
	strncpy(dest.m_strStockID, src.m_strStockID, sizeof(dest.m_strStockID) - 1);
	dest.m_bytMarket = src.m_bytMarket;
	dest.m_data.Resize(src.m_data.size());
	for(size_t i = 0; i < src.m_data.size(); i++)
	{
		dest.m_data.m_pData[i] = Convert(src.m_data[i]);
	}
	return dest;
}

SRawListMultiFFItem Convert(const CStdCacheListFFItem& src) {
    SRawListMultiFFItem dest;
    memcpy(dest.stockCode, src.stockCode, sizeof(dest.stockCode));
    dest.marketID = src.marketID;
    for (int i = 0; i < SMultiFFOrderType::MAX_ORDER; ++i) {
        dest.m_orders[i].m_buyAmount = src.flow.orders[i].buyAmount.GetValue();
        dest.m_orders[i].m_sellAmount = src.flow.orders[i].sellAmount.GetValue();
        dest.m_orders[i].m_buyVolume = src.flow.orders[i].buyVolume.GetValue();
        dest.m_orders[i].m_sellVolume = src.flow.orders[i].sellVolume.GetValue();
    }
    dest.m_callAuctionAmount = src.callAuctionAmount.GetValue();
    dest.m_latestPrice = src.price;
    dest.m_prevClose = src.prevClose;

    return dest;
}

SRawListMultiBuyinRankItem Convert(const CStdCacheListBuyinRank& src) {
    SRawListMultiBuyinRankItem dest;
    memcpy(dest.stockCode, src.stockCode, sizeof(dest.stockCode));
    dest.marketID = src.marketID;    
    for (int i = 0; i < SMultiFFDayType::MAX_DAY; ++i) {
        dest.m_days[i].m_prevClose = src.days[i].prevClose;
        dest.m_days[i].m_buyinRatio = src.days[i].buyinRatio / 10000.0;
        dest.m_days[i].m_rank = src.days[i].buyinRank;
        dest.m_days[i].m_prevRank = src.days[i].prevBuyinRank;
    }
    dest.m_latestPrice = src.price;

    return dest;
}

SRawMultiBaseBQ Convert(const CStdCacheBrokerQueue & src)
{
    SRawMultiBaseBQ dest;
    std::map<BYTE, std::vector<unsigned short> > mapBuyBrokerNo;
    std::map<BYTE, std::vector<unsigned short> > mapSellBrokerNo;
    std::vector<unsigned short> vecBuyBrokerNo;
    std::vector<unsigned short> vecSellBrokerNo;
    mapBuyBrokerNo.clear();
    mapSellBrokerNo.clear();
    vecBuyBrokerNo.clear();
    vecSellBrokerNo.clear();

    int BuyIndex = 0;
    for(size_t i = 0; i < src.m_btBuyItemCount; i++)
    {
        if(src.m_stItemBuy[i].m_pcBSType == 'B')
        {
            vecBuyBrokerNo.push_back(src.m_stItemBuy[i].m_wBSBrokerNo);
            //mapBuyBrokerNo[BuyIndex] = vecBuyBrokerNo;
            if(i == (src.m_btBuyItemCount - 1))
            {
                mapBuyBrokerNo[BuyIndex] = vecBuyBrokerNo;
            }
        }
        else if(src.m_stItemBuy[i].m_pcBSType == 'S' && src.m_stItemBuy[i].m_wBSBrokerNo != 0)
        {
            mapBuyBrokerNo[BuyIndex] = vecBuyBrokerNo;
            vecBuyBrokerNo.clear();
            BuyIndex = src.m_stItemBuy[i].m_wBSBrokerNo;
        }
    }

    int SellIndex = 0;
    for(size_t i = 0; i < src.m_btSellItemCount; i++)
    {
        if(src.m_stItemSell[i].m_pcBSType == 'B')
        {
            vecSellBrokerNo.push_back(src.m_stItemSell[i].m_wBSBrokerNo);
            //mapSellBrokerNo[SellIndex] = vecSellBrokerNo;
            if(i == (src.m_btSellItemCount - 1))
            {
                mapSellBrokerNo[SellIndex] = vecSellBrokerNo;
            }

        }
        else if(src.m_stItemSell[i].m_pcBSType == 'S' && src.m_stItemSell[i].m_wBSBrokerNo != 0)
        {
            mapSellBrokerNo[SellIndex] = vecSellBrokerNo;
            vecSellBrokerNo.clear();
            SellIndex = src.m_stItemSell[i].m_wBSBrokerNo;
        }
    }

    for(std::map<BYTE, std::vector<unsigned short> >::iterator itbuy = mapBuyBrokerNo.begin(); itbuy != mapBuyBrokerNo.end(); itbuy++)
    {
        SRawMultiBaseBQ::SBrokerNoItem tmp;
        tmp.cTabIndex = itbuy->first + 1;
        tmp.vecBrokerNoItem.insert(tmp.vecBrokerNoItem.end(), itbuy->second.begin(), itbuy->second.end());
        dest.m_vecBuyBrokerNo.push_back(tmp);
    }

    for(std::map<BYTE, std::vector<unsigned short> >::iterator itsell = mapSellBrokerNo.begin(); itsell != mapSellBrokerNo.end(); itsell++)
    {
        SRawMultiBaseBQ::SBrokerNoItem tmp;
        tmp.cTabIndex = itsell->first + 1;
        tmp.vecBrokerNoItem.insert(tmp.vecBrokerNoItem.end(), itsell->second.begin(), itsell->second.end());
        dest.m_vecSellBrokerNo.push_back(tmp);
    }

    return dest;
}

SRawMultiBQTrace Convert(const CStdCacheBrokerTrace & src)
{
    SRawMultiBQTrace dest;
    for(map<BYTE, set<CStdCacheBrokerTrace::STrace> >::const_iterator itbuy= src.mapBuyBT.begin(); itbuy != src.mapBuyBT.end(); itbuy++)
    {
        SRawMultiBQTrace::SBQTraceItem Item;
        Item.bTabIndex = itbuy->first + 1;
        set<CStdCacheBrokerTrace::STrace> vecTrace = itbuy->second;
//        for(size_t i = 0; i < vecTrace.size(); i++)
//        {
//            SRawMultiBQTrace::STrace tmp;
//            char cMarket[16] = {0};
//            sprintf(cMarket, "%05d", vecTrace[i].dwStockID);
//            strncpy(tmp.strStockID, cMarket, sizeof(tmp.strStockID) - 1);
//            tmp.bytMarket = vecTrace[i].bytMarket;
//            tmp.dwPrice = vecTrace[i].dwPrice;
//            Item.vecBQTraceItem.push_back(tmp);
//        }
       for(set<CStdCacheBrokerTrace::STrace>::iterator it = vecTrace.begin(); it != vecTrace.end(); it++)
       {
            SRawMultiBQTrace::STrace tmp;
            char cCode[16] = {0};
            sprintf(cCode, "%05d", it->dwStockID);
            strncpy(tmp.strStockID, cCode, sizeof(tmp.strStockID) - 1);
            tmp.ucFlag = it->ucFlag;
            tmp.dwPrice = it->dwPrice;
            Item.vecBQTraceItem.push_back(tmp);
        }
        dest.m_vecBuyBQTrace.push_back(Item);
    }

    for(map<BYTE, set<CStdCacheBrokerTrace::STrace> >::const_iterator itsell= src.mapSellBT.begin(); itsell != src.mapSellBT.end(); itsell++)
    {
        SRawMultiBQTrace::SBQTraceItem Item;
        Item.bTabIndex = itsell->first + 1;
        set<CStdCacheBrokerTrace::STrace> vecTrace = itsell->second;
//        for(size_t i = 0; i < vecTrace.size(); i++)
//        {
//            SRawMultiBQTrace::STrace tmp;
//
//            char cMarket[16] = {0};
//            sprintf(cMarket, "%05d", vecTrace[i].dwStockID);
//            strncpy(tmp.strStockID, cMarket, sizeof(tmp.strStockID) - 1);
//            tmp.bytMarket = vecTrace[i].bytMarket;
//            tmp.dwPrice = vecTrace[i].dwPrice;
//            Item.vecBQTraceItem.push_back(tmp);
//        }
        for(set<CStdCacheBrokerTrace::STrace>::iterator it = vecTrace.begin(); it != vecTrace.end(); it++)
       {
            SRawMultiBQTrace::STrace tmp;
            char cCode[16] = {0};
            sprintf(cCode, "%05d", it->dwStockID);
            strncpy(tmp.strStockID, cCode, sizeof(tmp.strStockID) - 1);
            tmp.ucFlag = it->ucFlag;
            tmp.dwPrice = it->dwPrice;
            Item.vecBQTraceItem.push_back(tmp);
        }
        dest.m_vecSellBQTrace.push_back(Item);
    }

    return dest;

}

/*
SRawMultiBQTrace Convert(const CStdCacheBrokerTrace & src)
{
    SRawMultiBQTrace dest;
    for(map<BYTE, vector<CStdCacheBrokerTrace::STrace> >::const_iterator itbuy= src.mapBuyBT.begin(); itbuy != src.mapBuyBT.end(); itbuy++)
    {
        SRawMultiBQTrace::SBQTraceItem Item;
        Item.bTabIndex = itbuy->first + 1;
        vector<CStdCacheBrokerTrace::STrace> vecTrace = itbuy->second;
        for(size_t i = 0; i < vecTrace.size(); i++)
        {
            SRawMultiBQTrace::STrace tmp;
            char cMarket[16] = {0};
            sprintf(cMarket, "%05d", vecTrace[i].dwStockID);
            strncpy(tmp.strStockID, cMarket, sizeof(tmp.strStockID) - 1);
            //tmp.bytMarket = vecTrace[i].bytMarket;
            tmp.dwPrice = vecTrace[i].dwPrice;
            Item.vecBQTraceItem.push_back(tmp);
        }
        dest.m_vecBuyBQTrace.push_back(Item);
    }

    for(map<BYTE, vector<CStdCacheBrokerTrace::STrace> >::const_iterator itsell= src.mapSellBT.begin(); itsell != src.mapSellBT.end(); itsell++)
    {
        SRawMultiBQTrace::SBQTraceItem Item;
        Item.bTabIndex = itsell->first + 1;
        vector<CStdCacheBrokerTrace::STrace> vecTrace = itsell->second;
        for(size_t i = 0; i < vecTrace.size(); i++)
        {
            SRawMultiBQTrace::STrace tmp;

            char cMarket[16] = {0};
            sprintf(cMarket, "%05d", vecTrace[i].dwStockID);
            strncpy(tmp.strStockID, cMarket, sizeof(tmp.strStockID) - 1);
            //tmp.bytMarket = vecTrace[i].bytMarket;
            tmp.dwPrice = vecTrace[i].dwPrice;
            Item.vecBQTraceItem.push_back(tmp);
        }
        dest.m_vecSellBQTrace.push_back(Item);
    }

    return dest;

}
*/

SRawMultiFFOrder Convert(const CStdCacheFFDeal& src) {
    SRawMultiFFOrder dest;
    dest.m_dwNumOfBuy = src.m_dwNumOfBuy;
    dest.m_dwNumOfSell = src.m_dwNumOfSell;
    dest.m_xVolOfBuy = src.m_xVolOfBuy.GetValue();
    dest.m_xVolOfSell = src.m_xVolOfSell.GetValue();
    dest.m_xAmtOfBuy = src.m_xAmtOfBuy.GetValue();
    dest.m_xAmtOfSell = src.m_xAmtOfSell.GetValue();

    return dest;
}

SRawMultiDayFF Convert(const CStdCacheDayFF& src) {
    SRawMultiDayFF dest;
    for (int i = 0; i < SMultiFFOrderType::MAX_ORDER; ++i) {
        dest.m_orders[i] = Convert(src.detail.orders[i]);
    }

    for (int i = 0; i < SMultiFFDayType::MAX_DAY; ++i) {
        dest.m_ranks[i].rank = src.day_ranks[i];
        dest.m_ranks[i].ratio = src.day_ratios[i] / 10000.0;
    }

    return dest;
}

SRawMarketTurnover::Item convert(const CStdCacheMarketTurnover::Item& src) {
    SRawMarketTurnover::Item dest;
    dest.date_in_yymmdd = src.date_in_yymmdd;
    dest.time_in_hhmmss = src.time_in_hhmmss;
    dest.buyin_amount = src.buyin_amount.GetValue();
    dest.sold_amount = src.sold_amount.GetValue();
    dest.total_amount = src.total_amount.GetValue();
    return dest;
}

SRawMarketTurnover convert(const CStdCacheMarketTurnover& src) {
    SRawMarketTurnover dest;
    dest.north_turnover = convert(src.north_turnover);
    dest.south_turnover = convert(src.south_turnover);
    return dest;
}

SRawStockMultiDayKline Convert(const SStockMultiDayKLine & src)
{
	SRawStockMultiDayKline dest;
	strncpy(dest.m_strStockID, src.m_strStockID, sizeof(dest.m_strStockID) - 1);
	dest.m_bytMarket = src.m_bytMarket;
	dest.m_data.Resize(src.m_data.size());
	for(size_t i = 0; i < src.m_data.size(); i++)
	{
		dest.m_data.m_pData[i] = Convert(src.m_data[i]);
	}
	return dest;
}

SRawMultiMx Convert(const CStdCacheMx & src)
{
	SRawMultiMx dest;
	dest.m_dwDate = src.m_dwDate;
	dest.m_dwTime = src.m_dwTime;
	dest.m_dwPrice = src.m_dwPrice;
	dest.m_xVolume = src.m_xVolume;
	dest.m_dwTradeNum = src.m_dwTradeNum;
	dest.m_xOI = src.m_xOI;
	dest.m_cBS = src.m_cBS;
	return dest;
}

SRawMultiMinuteFF Convert(const CStdCacheMinuteFF & src)
{
	SRawMultiMinuteFF dest;
    dest.m_dwTime = src.m_wTime;

    for (int i = 0; i < SMultiFFOrderType::MAX_ORDER; ++i) {
        dest.m_orders[i] = Convert(src.m_flow.orders[i]);
    }

	return dest;
}

RawInferiorFundFlow convert(const InferiorFundFlow& src) {
    RawInferiorFundFlow dest;
    dest.tradeTime_ = src.tradeTime_;
    for (int i = 0; i < FFOrderType::MAX_ORDER; ++i) {
        dest.numberOfBuys_[i] = src.numberOfBuys_[i];
        dest.numberOfSells_[i] = src.numberOfSells_[i];
        dest.volumeOfBuys_[i] = src.volumeOfBuys_[i].GetValue();
        dest.volumeOfSells_[i] = src.volumeOfSells_[i].GetValue();
        dest.amountOfBuys_[i] = src.amountOfBuys_[i].GetValue();
        dest.amountOfSells_[i] = src.amountOfSells_[i].GetValue();
    }
    
    return dest;
}

RawMultiStockMinuteFundFlow Convert(const MultiStockMinuteFundFlow& src) {
	RawMultiStockMinuteFundFlow dest;
	memcpy(dest.stockID_, src.stockID_, sizeof(dest.stockID_));
	dest.marketID_ = src.marketID_;
	dest.rawInferiorFundFlows_.Resize(src.inferiorFundFlows_.size());
	for(size_t i = 0; i < src.inferiorFundFlows_.size(); ++i) {
		dest.rawInferiorFundFlows_.m_pData[i] = convert(src.inferiorFundFlows_[i]);
	}
    
	return dest;
}

template<typename Tpsrc, typename Tpdest>
bool UncompressConvert(const char * pBuf, const int intLen, vector<Tpdest> & vectDest, const unsigned short wVer) {
    vectDest.clear();

	vector<Tpsrc> vectSrc;
	bool blnResult = CMultiCompress::Uncompress(pBuf, intLen, vectSrc, wVer);
	if (!blnResult) {
	    vectSrc.clear();
	    vectDest.clear();
		return false;
	}

	vectDest.clear();
	for (size_t i = 0; i < vectSrc.size(); i++) {
		Tpdest dest = Convert(vectSrc[i]);
		vectDest.push_back(dest);
	}
	return true;
}


template<typename Tpsrc, typename Tpdest>
bool UncompressConvert(const char * pBuf, const int intLen, vector<Tpsrc> & vectSrc, vector<Tpdest> & vectDest, bool & blnIsSnap, const unsigned short wVer) {
	vector<int> vectIndex;
	bool blnResult = CMultiCompress::Uncompress(pBuf, intLen, vectSrc, vectIndex, blnIsSnap, wVer);
	if (!blnResult) {
	    vectSrc.clear();
	    vectIndex.clear();
	    vectDest.clear();
	    return false;
	}

	vectDest.clear();
	if (blnIsSnap) {
		for (size_t i = 0; i < vectSrc.size(); i++) {
			Tpdest dest = Convert(vectSrc[i]);
			vectDest.push_back(dest);
		}
	} else {
		for (size_t i = 0; i < vectIndex.size(); i++) {
			Tpdest dest = Convert(vectSrc[vectIndex[i]]);
			vectDest.push_back(dest);
		}
	}
	return true;
}

//template <typename Tpsrc, typename Tpdest>
//bool uncompressListData(const char* buffer, const int bufferLength, std::vector<Tpsrc>& srcDatas, \
//    std::vector<Tpdest>& destDatas, bool& isSnapped, const unsigned short version) {
//	std::vector<int> indexs;
//	bool isResult = CMultiCompress::uncompressListData<Tpsrc>(buffer, bufferLength, \
//        srcDatas, indexs, isSnapped, version);
//	if (!isResult) {
//	    srcDatas.clear();
//	    destDatas.clear();
//	    return false;
//	}
//
//	destDatas.clear();
//	if (isSnapped) {
//		for (size_t i = 0; i < srcDatas.size(); ++i) {
//			Tpdest dest = convert(srcDatas[i]);
//			destDatas.push_back(dest);
//		}
//	} else {
//		for (size_t i = 0; i < indexs.size(); ++i) {
//			Tpdest dest = convert(srcDatas[indexs[i]]);
//			destDatas.push_back(dest);
//		}
//	}
//	return true;
//}

CMultiCompressObj::CMultiCompressObj(void)
    :m_wVer(1)
{
}

CMultiCompressObj::~CMultiCompressObj(void)
{
}

CMultiCompressIf * CMultiCompressIf::CreateInstance()
{
	return new CMultiCompressObj;
}

void CMultiCompressIf::FreeInstance(CMultiCompressIf * pNew)
{
	delete pNew;
}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, CRawMultiQt & data)
{
	bool blnResult = CMultiCompress::Uncompress(pBuf, intLen, m_qt, m_wVer);
	if(blnResult)
	{
	    data = Convert(m_qt);
	    return true;
	}
	else
	{
	    return false;
	}

}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, CRawMultiQtS & data)
{
	bool blnResult = CMultiCompress::Uncompress(pBuf, intLen, m_qts, m_wVer);
	if(blnResult)
	{
	    data = Convert(m_qts);
	    return true;
	}
	else
	{
	    return false;
	}

}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, CRawMultiBlockQt & data)
{
	bool blnResult = CMultiCompress::Uncompress(pBuf, intLen, m_blockqt, m_wVer);
	if(blnResult)
	{
	    data = Convert(m_blockqt);
	    return true;
	}
	else
	{
	    return false;
	}

}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, SRawMultiRtMin *& pNew, int & intSize)
{
    m_rtmins.clear();
	if(UncompressConvert<CStdCacheRtMin, SRawMultiRtMin>(pBuf, intLen, m_rtmins, m_wVer) == false)
	{
		return false;
	}

	if(m_rtmins.size())
	{
		pNew = &*m_rtmins.begin();
		intSize = m_rtmins.size();
	}
	else
	{
		pNew = NULL;
		intSize = 0;
	}

	return true;
}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, SRawMultiMx *& pNew, int & intSize)
{
    m_mxs.clear();
	if(UncompressConvert<CStdCacheMx, SRawMultiMx>(pBuf, intLen, m_mxs, 1) == false)
	{
		return false;
	}

	if(m_mxs.size())
	{
		pNew = &*m_mxs.begin();
		intSize = m_mxs.size();
	}
	else
	{
		pNew = NULL;
		intSize = 0;
	}

	return true;
}

bool CMultiCompressObj::Uncompress2(const char * pBuf, const int intLen, SRawMultiMx *& pNew, int & intSize)
{
                  m_newmxs.clear();
	if(UncompressConvert<CStdCacheMx, SRawMultiMx>(pBuf, intLen, m_newmxs, 2) == false)
	{
		return false;
	}

	if(m_newmxs.size())
	{
		pNew = &*m_newmxs.begin();
		intSize = m_newmxs.size();
	}
	else
	{
		pNew = NULL;
		intSize = 0;
	}

	return true;
}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, SRawStockMultiMin *& pNew, int & intSize)
{
    m_rtmin.clear();
	if(UncompressConvert<SStockMultiMin, SRawStockMultiMin>(pBuf, intLen, m_rtmin, m_wVer) == false)
	{
		return false;
	}

	if(m_rtmin.size())
	{
		pNew = &*m_rtmin.begin();
		intSize = m_rtmin.size();
	}
	else
	{
		pNew = NULL;
		intSize = 0;
	}

	return true;
}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, SRawStockMultiDayKline *& pNew, int & intSize)
{
    m_kline.clear();
	if(UncompressConvert<SStockMultiDayKLine, SRawStockMultiDayKline>(pBuf, intLen, m_kline, m_wVer) == false)
	{
		return false;
	}

	if(m_kline.size())
	{
		pNew = &*m_kline.begin();
		intSize = m_kline.size();
	}
	else
	{
		pNew = NULL;
		intSize = 0;
	}

	return true;
}


bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, SRawMultiBaseBQ & data)
{
    bool blnResult = CMultiCompress::Uncompress(pBuf, intLen, m_bq, m_wVer);
    if(blnResult)
    {
        data = Convert(m_bq);
        return true;
    }
    else
    {
        return false;
    }

    return true;
}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, SRawMultiBQTrace & data)
{
    bool blnResult = CMultiCompress::Uncompress(pBuf, intLen, m_trace, m_wVer);
    if(blnResult)
    {
        data = Convert(m_trace);
        return true;
    }
    else
    {
        return false;
    }

    return true;
}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, CRawMultiQt *& pNew, int & intSize, bool & blnIsSnap)
{
	if(UncompressConvert(pBuf, intLen, m_qtlist, m_newqtlist, blnIsSnap, m_wVer) == false)
	{
		return false;
	}

	if(m_newqtlist.size())
	{
		pNew = &*m_newqtlist.begin();
		intSize = m_newqtlist.size();
	}
	else
	{
		pNew = NULL;
		intSize = 0;
	}

	return true;
}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, CRawMultiBlockQt *& pNew, int & intSize, bool & blnIsSnap)
{
	if(UncompressConvert(pBuf, intLen, m_qtblocklist, m_newqtblocklist, blnIsSnap, m_wVer) == false)
	{
		return false;
	}

	if(m_newqtblocklist.size())
	{
		pNew = &*m_newqtblocklist.begin();
		intSize = m_newqtblocklist.size();
	}
	else
	{
		pNew = NULL;
		intSize = 0;
	}

	return true;
}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, \
    SRawBlockOverView*& pNew, int & intSize, bool & blnIsSnap, const unsigned short version) {
	if (!UncompressConvert(pBuf, intLen, m_qtviewlist, m_newqtviewlist, blnIsSnap, version))
		return false;

	if (!m_newqtviewlist.empty()) {
		pNew = &*m_newqtviewlist.begin();
		intSize = m_newqtviewlist.size();
	} else {
		pNew = NULL;
		intSize = 0;
	}

	return true;
}

bool CMultiCompressObj::Uncompress(const char* pBuf, const int intLen, CRawMultiQtEx& data)
{
	bool result = CMultiCompress::Uncompress(pBuf, intLen, m_qtex, m_wVer);
	if(result) {
	    data = Convert(m_qtex);
	}

    return result;
}

bool CMultiCompressObj::Uncompress(const char* pBuf, const int intLen, CRawMultiCAS& data)
{
	bool result = CMultiCompress::Uncompress(pBuf, intLen, m_cas, m_wVer);
	if(result) {
	    data = Convert(m_cas);
	}

    return result;
}

bool CMultiCompressObj::Uncompress(const char* pBuf, const int intLen, CRawMultiVCM& data)
{
	bool result = CMultiCompress::Uncompress(pBuf, intLen, m_vcm, m_wVer);
	if(result) {
	    data = Convert(m_vcm);
	}

    return result;
}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, SRawMultiDayFF& data, const unsigned short version) {
    bool result = false;
    if (version == MCM_VERSION) {
        result = CMultiCompress::Uncompress(pBuf, intLen, cacheDayFF_, version);
        if (result) {
            data = Convert(cacheDayFF_);
        }
    }
    return result;
}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, SRawMarketTurnover& data, int marketID, const char* stockID, unsigned short version) {
    bool result = false;
    if (version == MCM_VERSION) {
        result = CMultiCompress::Uncompress(pBuf, intLen, marketTurnovers_[marketID][stockID], version);
        if (result) {
            data = convert(marketTurnovers_[marketID][stockID]);
        }
    }
    return result;
}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, SRawMultiMinuteFF*& data, int& size, const unsigned short version) {
    if (version == MCM_VERSION) {
        if (!UncompressConvert<CStdCacheMinuteFF>(pBuf, intLen, minuteFF_, version)) {
            return false;
        }

        size = minuteFF_.size();
        data = size ? &*minuteFF_.begin() : NULL;
    }

    return true;
}

bool CMultiCompressObj::Uncompress(const char* buffer, const int bufferLength, SRawListMultiFFItem*& ptrData, int& dataSize, \
    bool& isSnapped, const unsigned short version) {
    if (version == MCM_VERSION) {
        if (!UncompressConvert(buffer, bufferLength, cacheListFFItems_, rawListFFItems_, isSnapped, version))
            return false;

        if (!rawListFFItems_.empty()) {
            ptrData = &*rawListFFItems_.begin();
            dataSize = rawListFFItems_.size();
        } else {
            ptrData = NULL;
            dataSize = 0;
        }
    }

    return true;
}

bool CMultiCompressObj::Uncompress(const char* buffer, const int bufferLength, SRawListMultiBuyinRankItem*& ptrData, int& dataSize, \
    bool& isSnapped, const unsigned short version) {
    if (version == MCM_VERSION) {
        if (!UncompressConvert(buffer, bufferLength, cacheListBuyinRankItems_, rawListBuyinRankItems_, isSnapped, version))
            return false;

        if (!rawListBuyinRankItems_.empty()) {
            ptrData = &*rawListBuyinRankItems_.begin();
            dataSize = rawListBuyinRankItems_.size();
        } else {
            ptrData = NULL;
            dataSize = 0;
        }
    }

    return true;
}

bool CMultiCompressObj::Uncompress(const char* pBuf, const int intLen, CRawMultiBlockQt & data,
    const unsigned short version) {
	bool blnResult = CMultiCompress::Uncompress(pBuf, intLen, m_blockqt, version);
	if (blnResult) {
	    data = Convert(m_blockqt);
	    return true;
	} else {
	    return false;
	}
}

bool CMultiCompressObj::Uncompress(const char * pBuf, const int intLen, RawMultiStockMinuteFundFlow*& ptrData, int& dataSize,
    const unsigned short version) {
    if (version == MCM_VERSION) {
        rawMultiStockMinuteFundFlows_.clear();
        if(!UncompressConvert<MultiStockMinuteFundFlow, RawMultiStockMinuteFundFlow>(pBuf, intLen, rawMultiStockMinuteFundFlows_, version)) {
            return false;
        }

        if (!rawMultiStockMinuteFundFlows_.empty()) {
            ptrData = &*rawMultiStockMinuteFundFlows_.begin();
            dataSize = rawMultiStockMinuteFundFlows_.size();
        } else {
            ptrData = NULL;
            dataSize = 0;
        }

        return true;
    }
}


