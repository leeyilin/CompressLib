#ifndef BIGINT_H
#define BIGINT_H
#include "QtTypeDef.h"
/*
 * CBigInt is a self-defined data structure to represent big numbers in relatively small memory space
 *
 * Data in this range:-268,435,456 ~ 268,435,455 (268 millions)
 *    namely:                -2^28 ~ 2^28-1
 * can be compressed and decompressed precisely.
 *
 * Data in this range:-72,057,594,037,927,936 ~ -268,435,456
 *               Or :             268,435,455 ~ 72,057,593,769,492,480 (72 quadrillions)
 * can not be compressed and decompressed precisely.
 *
 * We do not deal with data beyond this range.
 */

 class CLASSINDLL_CLASS_DECL CBigInt                //CLASSINDLL_CLASS_DECL is invalid in Linux.
{
public:
    CBigInt();
    CBigInt(const INT64 n);

    static INT32    m_nMaxBase;                     //representing the maximum number(positive) CBigInt can hold without losing data precision
    static INT32    m_nMinBase;                    //representing the minimum number(negative) CBigInt can hold without losing data precision
    static BYTE     m_cMaxMul;                      //One byte in memory, it can represent a number from 0 to 256.
                                                   //the member "m_cMaxMul" represents the maximum myltiple rate, like exponent in mathematics

    INT32           m_nBase:29;                     //to store base number, like base in mathematics
    UINT32          m_cMul:3;                       //to store multiple rate, like exponent in mathematics

    INT64 GetValue()const;
    operator INT64()const { return GetValue();}         //a kind of function of operator overridden
    CBigInt operator=(const CBigInt x);

    CBigInt operator=(const INT64 n);
    CBigInt operator+=(const INT64 n);
    CBigInt operator-=(const INT64 n);
    INT32 GetRawData()const;
    void SetRawData(INT32 n);

    bool operator==(CBigInt x)const;
    CBigInt operator/(const INT32 n)const;
};

#endif
