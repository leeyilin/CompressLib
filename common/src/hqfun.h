#pragma once
#include "QtTypeDef.h"
#include "StdQtStruct.h"
#include <string>
#include <cstdlib>
#include <cmath>

using namespace std;

inline string CreateUniqueStockID(const string & strMarket, const string & strCode)
{
    string strResult;
    strResult = strMarket;
    strResult += "|";
    strResult += strCode;
    return  strResult;
}

// Doesn't create anything. Just format the unique code into the buffer.
inline void CreateUniqueStockID(char* buffer, int size,
    const string & strMarket, const string & strCode)
{
    snprintf(buffer, size, "%s|%s", strMarket.c_str(), strCode.c_str());
}

inline string GetMarketCodeFromUniqueID(const string & strUniqueID)
{
    int intPos = strUniqueID.find("|");
    return strUniqueID.substr(0, intPos);
}

inline string GetStockCodeFromUniqueID(const string & strUniqueID)
{
    int intPos = strUniqueID.find("|");
    return strUniqueID.substr(intPos + 1, strUniqueID.size());
}

inline void GetMarketStockFromUniqueID(const string &uniqueID, string &market, char *buf, size_t len)
{
    std::string::size_type pos = uniqueID.find("|");
    if (pos != std::string::npos) {
        market = uniqueID.substr(0, pos);
        snprintf(buf, len, "%s", uniqueID.c_str() + pos + 1);
    } else {
        market = "";
        buf[0] = '\0';
    }
}

inline void GetStockCodeFromUniqueID(const string & uniqueID, char *buf, size_t len)
{
    std::string::size_type pos = uniqueID.find("|");
    if (pos != std::string::npos) {
        snprintf(buf, len, "%s", uniqueID.c_str() + pos + 1);
    } else {
        buf[0] = '\0';
    }
}

inline int PriceToInt(const double f, const int intScale)
{
    int x = 1;
    double y = 0.5;
    for(int i = 0; i < intScale; i++)
    {
        x *= 10;
    }

    y /= x;
    if(f > 0)
    {
        return static_cast<int>((f + y)*x);
    }
    else if(f == 0)
    {
        return 0;
    }
    else
    {
        return static_cast<int>((f - y)*x);
    }
}

inline double IntToPrice(const int p, const int intScale)
{
    double x = 1;
    for(int i = 0; i < intScale; i++)
    {
        x *= 10;
    }

    if(p != 0)
    {
        return p/x;
    }
    else
    {
        return 0;
    }
}

inline unsigned int PriceToUInt(const double f, const int intScale)
{
    int x = 1;
    double y = 0.5;
    for(int i = 0; i < intScale; i++)
    {
        x *= 10;
    }

    y /= x;
    if(f > 0)
    {
        if( (f + y)*x >(unsigned int)(-1))
            return 0;
        return static_cast<unsigned int>((f + y)*x);
    }
    else if(f == 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned int>((f - y)*x);
    }
}

inline double UIntToPrice(const unsigned int p, const int intScale)
{
    double x = 1;
    for(int i = 0; i < intScale; i++)
    {
        x *= 10;
    }

    if(p != 0)
    {
        return p/x;
    }
    else
    {
        return 0;
    }
}

inline int NormalizeFloat(const double f)
{
    if(f > 0)
    {
        return static_cast<int>((f+0.00005)*10000);
    }
    else if(f == 0)
    {
        return 0;
    }
    else
    {
        return static_cast<int>((f - 0.00005)*10000);
    }
}

inline void ConvertToKline(const CStdRtMin* pSrc, const int intSize, vector<CStdRtMin> & vectResult, const int intPeriod/*1 5 30 120 ...*/)
{
    for(int i = 0; i < intSize; i++)
    {
        const CStdRtMin & src = pSrc[i];
        if(i == 0)
        {
            vectResult.push_back(src);
        }
        else
        {
            if(i > vectResult.size() * intPeriod)
            {
                vectResult.push_back(src);
            }
            else
            {
                CStdRtMin & dest = vectResult.back();
                dest.m_wTime = src.m_wTime;
                dest.m_dwHigh    = max(dest.m_dwHigh, src.m_dwHigh);
                dest.m_dwLow     = min(dest.m_dwLow, src.m_dwLow);
                dest.m_dwClose  = src.m_dwClose;
                dest.m_dwAve = src.m_dwAve;
                dest.m_xExt1 = src.m_xExt1;
                dest.m_xExt2 += src.m_xExt2;
                dest.m_dwExt1 = src.m_dwExt1;
                dest.m_xAmount += src.m_xAmount.GetValue();
                dest.m_xVolume += src.m_xVolume.GetValue();
                dest.m_xWaiPan += src.m_xWaiPan.GetValue();
                dest.m_xTradeNum += src.m_xTradeNum.GetValue();
            }
        }
    }
}

inline void ConvertKlineToKline(const CStdRtMin* pSrc, const int intSize, vector<CStdRtMin> & vectResult, const int intRatio/*�Ŵ���*/)
{
    if(intRatio <= 1)
    {
        vectResult.insert(vectResult.end(), pSrc, pSrc + intSize);
        return;
    }

    for(int i = 0; i < intSize; i++)
    {
        const CStdRtMin & src = pSrc[i];
        if(i >= vectResult.size() * intRatio)
        {
            vectResult.push_back(src);
        }
        else
        {
            CStdRtMin & dest = vectResult.back();
            dest.m_wTime = src.m_wTime;
            dest.m_dwHigh    = max(dest.m_dwHigh, src.m_dwHigh);
            dest.m_dwLow     = min(dest.m_dwLow, src.m_dwLow);
            dest.m_dwClose  = src.m_dwClose;
            dest.m_dwAve = src.m_dwAve;
            dest.m_xExt1 = src.m_xExt1;
            dest.m_xExt2 += src.m_xExt2;
            dest.m_dwExt1 = src.m_dwExt1;
            dest.m_xAmount += src.m_xAmount.GetValue();
            dest.m_xVolume += src.m_xVolume.GetValue();
            dest.m_xWaiPan += src.m_xWaiPan.GetValue();
            dest.m_xTradeNum += src.m_xTradeNum.GetValue();
        }
    }
}

