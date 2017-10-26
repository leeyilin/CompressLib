// CBigInt.cpp: implementation of the CBigInt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QtTypeDef.h"
#include "BigInt.h"


INT32 CBigInt::m_nMaxBase = 0x0FFFFFFF;
INT32 CBigInt::m_nMinBase = 0xF0000000;
BYTE  CBigInt::m_cMaxMul = 7;

CBigInt::CBigInt()
{
    m_nBase = 0;
    m_cMul = 0;
}

CBigInt::CBigInt(const INT64 n)
{
    *this = n;                      //Here we use the overridden operaotr " = ".
}

//return a INT64 number, the actual value in CBigInt
INT64 CBigInt::GetValue()const      //1 in m_cMul weights 16(2^4) times of m_nBase;
{
    INT64 n = m_nBase;

    for(BYTE c = 0; c < m_cMul; c++)
        n *= 16;

    return n;
}

CBigInt CBigInt::operator=(const CBigInt x)
{
    m_nBase = x.m_nBase;
    m_cMul = x.m_cMul;

    return *this;
}

CBigInt CBigInt::operator=(const INT64 n)           //to discompose the big number 'n' and store it in the format of CBigInt.
{
    INT64 nBase = n;
    m_cMul = 0;

    INT32 nMod = 0;

    while(nBase > m_nMaxBase || nBase < m_nMinBase)
    {
        nMod = (INT32)(nBase % 16);
        nBase /= 16;
        if(nMod >= 8)                       //do what like the rounding up and down in decimal.
            nBase++;
        else if(nMod <= -8)
            nBase--;

        m_cMul++;
        if(m_cMul >= m_cMaxMul)             // In this case, the parameter 'n' is too large for CBigInt to hold
            break;
    }

    m_nBase = (INT32)nBase;

    return *this;
}

CBigInt CBigInt::operator+=(const INT64 n)
{
//  *this = *this+n;
    *this = GetValue() + n;                 //Here we still use the overridden operator "="
    return *this;
}

CBigInt CBigInt::operator-=(const INT64 n)
{
    *this = GetValue() - n;
    return *this;
}

//to get the value in the format of CBigInt
INT32 CBigInt::GetRawData()const
{
    return *(INT32*)this;
}

void CBigInt::SetRawData(INT32 n)
{
    *(INT32*)this = n;
}

bool CBigInt::operator==(CBigInt x)const
{
    INT64 n64 = *this;
    INT64 nX = x;

    return (n64 == nX);
}

CBigInt CBigInt::operator/(const INT32 n)const
{

    CBigInt result;
    result = GetValue()/n;                      //Here we use the overridden operator "="
    return result;
}
