// BitStream.cpp: implementation of the CBitStream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BitStream.h"
#include "string.h"


//to set the members to their default values
CBitStream::CBitStream()
{
    m_cBufType = 2;

    m_pcBuf = NULL;
    m_nBitLen = 0;

    m_pBuffer = NULL;
    m_uStartPos = 0;

    m_nCurPos = 0;
}
//to set the CBitStream as a buffer, using the parameter pBuffer's memory space.
CBitStream::CBitStream(CPackBuffer* pBuffer, bool bRead)
{

    m_pcBuf = NULL;
    m_nBitLen = 0;

    m_pBuffer = pBuffer;
    if (m_pBuffer)
    {
        if (bRead)
            m_uStartPos = m_pBuffer->m_nReadPos;
        else
            m_uStartPos = m_pBuffer->GetBufferLen();                //to set the starting position at the back of the buffer
    }

    m_nCurPos = 0;
    m_cBufType = 0;
}

//to set the CBitStream as a bit stream using the parameter pcBuf's memory space
CBitStream::CBitStream(PBYTE pcBuf, int nBufLen)
{
    SetBuffer(pcBuf, nBufLen);
}

//According to the parameter nBufLen, it allocates some memory space to set up a bit stream.
CBitStream::CBitStream(int nBufLen)
{
    if (nBufLen > 0)
    {
        m_pcBuf = new BYTE[nBufLen];
        m_nBitLen = nBufLen * 8;
    }
    else
    {
        m_pcBuf = NULL;
        m_nBitLen = 0;
    }

    m_pBuffer = NULL;
    m_uStartPos = 0;

    m_nCurPos = 0;
    m_cBufType = 2;
}


//to retrieve the allocated memory space
CBitStream::~CBitStream()
{
    if (m_cBufType==2 && m_pcBuf)
        delete[] m_pcBuf;
}

//to use the memory space in the parameter pcBuf instead of applying new space to construct a bit stream
void CBitStream::SetBuffer(PBYTE pcBuf, int nBufLen)
{

    if (m_cBufType==2 && m_pcBuf)
        delete[] m_pcBuf;

    m_pcBuf = pcBuf;
    m_nBitLen = nBufLen*8;

    m_pBuffer = NULL;
    m_uStartPos = 0;

    m_nCurPos = 0;
    m_cBufType = 1;
}

//This function acts to make the bits buffer big enough to hold the data.
int CBitStream::ReallocBuf(int nBits)                   //The function's name is short name of "re-allocate buffer".
{
    if(m_cBufType!=2)                               //We only care the case when m_cBufType == 2.
        return 0;

    int nBufBytes = m_nBitLen/8;
    int nDataBytes = (nBits+7)/8;

    if(nBufBytes >= nDataBytes)                 //In this case, the buffer size is big enough to hold the data.
        return 0;

    const int nAddBytes = 4096;                             //the basic unit is 4k

//When we arrive here, notice that the data's size is now bigger than the buf's size, yet the actual data is not in the buffer.
    while(nBufBytes < nDataBytes)
        nBufBytes += nAddBytes;

    PBYTE pcNewBuf = new BYTE[nBufBytes];

    if(m_pcBuf)
    {
        if(m_nCurPos > 0)
            memcpy(pcNewBuf, m_pcBuf, (m_nCurPos+7)/8);             //I guess the m_nCurPos is in bits

        delete[] m_pcBuf;
    }

    m_pcBuf = pcNewBuf;
    m_nBitLen = nBufBytes * 8;

    return nBufBytes;
}
//static int fuck = 0;
//To write nBits bits of the UNIT32 dw into the bit stream.
BYTE CBitStream::WriteDWORD(UINT32 dw, int nBits)
{

    if (nBits <= 0 || nBits > 32)                       //it means nBits is illegal
        return 0;

    BYTE* pcBufBase = NULL;
    if (m_cBufType == 0)                              //in the format of buffer
    {
        if (m_pBuffer == NULL)
        {
            return 0;
        }

        if (nBits > m_nBitLen)
        {
            UINT32 dw = 0;
            m_pBuffer->Write(&dw, 4);
            m_nBitLen += 32;
        }

        pcBufBase = m_pBuffer->GetBuffer(m_uStartPos);
    }
    else
    {
        if (m_cBufType==2)                          //in the format of bit stream
        {
            if (m_pcBuf==NULL || m_nCurPos+nBits>m_nBitLen)
                ReallocBuf(m_nCurPos+nBits);
        }
        if (m_pcBuf==NULL || m_nCurPos+nBits>m_nBitLen)
        {
            return 0;
        }
        else
            pcBufBase = m_pcBuf;
    }

    int nRet = nBits;

    dw <<= (32 - nBits);
    PBYTE pcHigh = ((PBYTE)&dw) + 3;

    PBYTE pcBuf = pcBufBase + m_nCurPos/8;
    int nLastBits = m_nCurPos%8;

    if(nLastBits != 0)          // fill in the last byte, because there is some idle bits in the last byte of the bit stream
    {

        int nFillBits = 8 - nLastBits;                  //the number of idle bits
        *pcBuf &= (0xFF << nFillBits);                  //set the corresponding bits int the bit stream to zero for later use

        *pcBuf += (BYTE)((*pcHigh) >> nLastBits);             //Pay attention to this statement. It does not change the actual bits in the byte *pcHigh.
        pcBuf++;

        if(nFillBits < nBits)
            dw <<= nFillBits;   // else, finish             //to leave out the bits we have stored

        nBits -= nFillBits;
    }

    while(nBits > 0)                    //to store the data byte to byte
    {
        *pcBuf++ = *pcHigh--;
        nBits -= 8;
    }

    m_nCurPos += nRet;
    if (m_cBufType==0)                          // ???    I don't understand this statement
        m_nBitLen -= nRet;

    return nRet;
}


UINT32 CBitStream::ReadDWORD(int nBits, bool bMovePos)
{

    if (nBits <= 0 || nBits > 32)                       //In this case nBits is illegal.
        return 0;

    BYTE* pcBufBase = NULL;

    if (m_cBufType == 0)
    {
        if (m_pBuffer == NULL)
        {
            return 0;
        }

        if (nBits > m_nBitLen)
        {
            try
            {
                if (bMovePos)
                {
                    m_pBuffer->SkipData(4);

                    m_nBitLen += 32;
                }
                else
                {
                    if (m_pBuffer->GetBufferLen() > 0)
                        m_nBitLen += 32;
                    else
                    {
                        return 0;
                    }
                }
            }
            catch(int n)
            {
                if (n == DATA_LACK)
                {
                    return 0;
                }
                else
                    throw;
            }
        }

        pcBufBase = m_pBuffer->GetBuffer(m_uStartPos);
    }
    else
    {
        if (m_pcBuf == NULL || m_nCurPos+nBits > m_nBitLen)
        {
            return 0;
        }

        pcBufBase = m_pcBuf;
    }


    PBYTE pcBuf = pcBufBase + m_nCurPos / 8;                  //What is pcBuf pointing at now ?
    int nLastBits = m_nCurPos % 8;                            //nLastBits is less than or equal to 7.

    if (bMovePos)
    {
        m_nCurPos += nBits;
        if (m_cBufType == 0)
            m_nBitLen -= nBits;
    }

    UINT32 dw = 0;

    if(nLastBits != 0)                                    //to deal with the cass that there is incomplete byte in the bit stream
    {
        dw = (*pcBuf) & (0xFF >> nLastBits);
        pcBuf++;

        int nReadBits = 8 - nLastBits;
        if(nReadBits  > nBits)                          //to deal with the case that we have read more than the requested bits
            dw >>= (nReadBits - nBits);

        nBits -= nReadBits;
    }

    while(nBits > 0)                                //to read bit stream byte by byte
    {
        if(nBits >= 8)
        {
            dw = (dw << 8) + *pcBuf++;
            nBits -= 8;
        }
        else
        {
            dw = (dw << nBits) + (*pcBuf >> (8-nBits));
            nBits = 0;
        }
    }

    return dw;
}

//to match the appropriate data type in the array of CBitCode
//except for the DataType 'O', dw is always the minute difference
//return the matched Node
CBitCode* CBitStream::MatchCodeEncode(UINT32& dw, CBitCode* pBitCodes, int nNumOfCodes)
{
    if(pBitCodes == NULL || nNumOfCodes <= 0)
        return NULL;

    CBitCode* pCode = pBitCodes;
    UINT32 dwSave = 0;
    int i = 0;
    for(; i < nNumOfCodes; i++, pCode++)
    {
        if(pCode->m_cDataType == 'C')           // const
        {
            if(dw == pCode->m_dwDataBias)
                break;
        }
        else if(pCode->m_cDataType == 'D')      // UINT32
        {
            dwSave = dw - pCode->m_dwDataBias;
            if((dwSave >> pCode->m_cDataLen) == 0)
            {
                dw = dwSave;
                break;
            }
        }
        else if(pCode->m_cDataType == 'I')      // INT          Because in this case dw is a signed type, so we deal with it in two separate ways.
        {
            if(dw & 0x80000000)
            {
                dwSave = dw + pCode->m_dwDataBias;
                if((dwSave >> (pCode->m_cDataLen-1)) == (0xFFFFFFFF >> (pCode->m_cDataLen-1)))
                {
                    dw = dwSave;
                    break;
                }
            }
            else
            {
                dwSave = dw - pCode->m_dwDataBias;
                if((dwSave >> (pCode->m_cDataLen-1)) == 0)
                {
                    dw = dwSave;
                    break;
                }
            }
        }
        else if(pCode->m_cDataType == 'H')      // HUNDRED UINT32
        {
            if(dw % 100 == 0)
            {
                dwSave = dw/100 - pCode->m_dwDataBias;
                if((dwSave >> pCode->m_cDataLen) == 0)
                {
                    dw = dwSave;
                    break;
                }
            }
        }
        else if(pCode->m_cDataType == 'h')      // HUNDRED INT
        {
            INT32 iValue = dw;
            if(iValue % 100 == 0)
            {
                if(iValue < 0)
                {
                    dwSave = iValue/100 + pCode->m_dwDataBias;
                    if((dwSave >> (pCode->m_cDataLen-1)) == (0xFFFFFFFF >> (pCode->m_cDataLen-1)))
                    {
                        dw = dwSave;
                        break;
                    }
                }
                else
                {
                    dwSave = iValue/100 - pCode->m_dwDataBias;
                    if((dwSave >> (pCode->m_cDataLen-1)) == 0)
                    {
                        dw = dwSave;
                        break;
                    }
                }
            }
        }
        else if(pCode->m_cDataType == 'O')      // ORIGINAL
            break;
    }

    if(i == nNumOfCodes)
        return NULL;

    return pCode;
}

BYTE CBitStream::EncodeData(UINT32 dwData, CBitCode* pBitCodes, int nNumOfCodes, const UINT32 * pdwBase, const bool bReverse)
{
    UINT32 dw = dwData;
    if(pdwBase != NULL)
    {
        if(bReverse)
            dw = *pdwBase - dwData;
        else
            dw = dwData - *pdwBase;
    }

    CBitCode* pCode = MatchCodeEncode(dw, pBitCodes, nNumOfCodes);
    if(pCode == NULL)
    {
        return 0;
    }

    if(pCode->m_cDataType == 'O')           //to encode the original data into the bit stream
    {
        if(pCode->m_cDataLen == 0 || pCode->m_cDataLen != 32 && (dwData >> pCode->m_cDataLen) != 0)
        {
            return 0;
        }
        WriteDWORD(pCode->m_wCodeBits, pCode->m_cCodeLen);
        WriteDWORD(dwData, pCode->m_cDataLen);
    }
    else                                    //to encode the minute difference into the bit stream
    {
        WriteDWORD(pCode->m_wCodeBits, pCode->m_cCodeLen);
        if(pCode->m_cDataLen > 0)
            WriteDWORD(dw, pCode->m_cDataLen);
    }

    return (pCode->m_cCodeLen + pCode->m_cDataLen);
}

BYTE CBitStream::EncodeBigInt(CBigInt xData, CBitCode* pBitCodes, int nNumOfCodes, const CBigInt * pxBase, const bool bReverse)
{
    INT64 ll = 0;
    UINT32 dw = 0;
    if(pxBase != NULL)
    {
        if(bReverse)
            ll = (INT64)(*pxBase) - (INT64)xData;
        else
            ll = (INT64)xData - (INT64)(*pxBase);
    }
    else
        ll = (INT64)xData;

    CBitCode* pCode = NULL;

    if(ll >= CBigInt::m_nMinBase && ll <= CBigInt::m_nMaxBase)          // In this case ll is not big, so we can encode it as a normal number.
    {
        dw = (UINT32)ll;
        pCode = MatchCodeEncode(dw, pBitCodes, nNumOfCodes);
    }
    else                                                                // In this case ll is very big, so we encode it in the format of CBigInt.
    {
        CBitCode* pC = pBitCodes;
        for(int i = 0; i < nNumOfCodes; i++, pC++)
        {
            if(pC->m_cDataType == 'O')
            {
                pCode = pC;
                break;
            }
        }
    }

    if(pCode == NULL)
    {
        return 0;
    }
//When arriving here, it means pCode != NULL
    if(pCode->m_cDataType == 'O')           //to encode the original data in the format of CBigInt into the bit stream
    {
        if(pCode->m_cDataLen != 32)
        {
            return 0;
        }
        WriteDWORD(pCode->m_wCodeBits, pCode->m_cCodeLen);
        WriteDWORD(xData.GetRawData(), pCode->m_cDataLen);
    }
    else
    {
        WriteDWORD(pCode->m_wCodeBits, pCode->m_cCodeLen);
        if(pCode->m_cDataLen > 0)
            WriteDWORD(dw, pCode->m_cDataLen);
    }
    return (pCode->m_cCodeLen + pCode->m_cDataLen);
}

//to read the bit part and the data part in the bit stream respectively and match them to the appropriate DataType in the CBitCode.
//to restore the data in the dw and record the length of the whole part in cBytes.
//return the matched Node.
CBitCode* CBitStream::MatchCodeDecode(UINT32& dw, BYTE& cBytes, CBitCode* pBitCodes, int nNumOfCodes)
{
    dw = 0;
    cBytes = 0;

    if(pBitCodes == NULL || nNumOfCodes <= 0)
    {
        return NULL;
    }

    int nReadBits;
    if (m_cBufType == 0)
    {
        if (m_pBuffer)
        {
            nReadBits = m_nBitLen;
            if (nReadBits < 16)
                nReadBits += m_pBuffer->GetBufferLen()*8;
        }
        else
            nReadBits = 0;
    }
    else
        nReadBits = m_nBitLen - m_nCurPos;
    if(nReadBits <= 0)
    {
        return NULL;
    }
    if (nReadBits>16)                                   //to make sure that nReadBits is less than or equal to 16
        nReadBits = 16;

// to read nReadBits bits from the bit stream
    WORD wCode = (WORD)ReadDWORD(nReadBits, false);         // to read the whole part, including the code bit part and the data part

    CBitCode* pCode = pBitCodes;
    int i = 0;
    for(; i < nNumOfCodes; i++, pCode++)
    {
        if(pCode->m_wCodeBits == (wCode >> (nReadBits-pCode->m_cCodeLen)))      // to remove the data part so that we can compare the two code bit part
            break;
    }

    if(i == nNumOfCodes)
    {
        return NULL;
    }

    m_nCurPos += pCode->m_cCodeLen;
    if (m_cBufType == 0)
    {
        if (m_nBitLen >= 32)
        {
            m_nBitLen -= pCode->m_cCodeLen;
            if (m_nBitLen>=32)
                m_nBitLen -= 32;
            else
                m_pBuffer->SkipData(4);
        }
        else
            m_nBitLen -= pCode->m_cCodeLen;
    }

    UINT32 dwData = 0;
    if(pCode->m_cDataLen > 0)
        dwData = ReadDWORD(pCode->m_cDataLen);          //and then read the data part

    if(pCode->m_cDataType == 'C')           // const
        dw = pCode->m_dwDataBias;
    else if(pCode->m_cDataType == 'D')      // UINT32
        dw = dwData + pCode->m_dwDataBias;
    else if(pCode->m_cDataType == 'I')      // INT
    {
        if(dwData >> (pCode->m_cDataLen-1))
        {
            dwData |= (0xFFFFFFFF << pCode->m_cDataLen);
            dw = dwData - pCode->m_dwDataBias;
        }
        else
            dw = dwData + pCode->m_dwDataBias;
    }
    else if(pCode->m_cDataType == 'H')      // HUNDRED
        dw = (dwData + pCode->m_dwDataBias)*100;
    else if(pCode->m_cDataType == 'h')      // HUNDRED INT
    {
        if(dwData >> (pCode->m_cDataLen-1))
        {
            dwData |= (0xFFFFFFFF << pCode->m_cDataLen);
            INT32 iValue = dwData;
            iValue -= pCode->m_dwDataBias;

            dw = iValue * 100;
        }
        else
        {
            dw = (dwData + pCode->m_dwDataBias) * 100;
        }
    }
    else if(pCode->m_cDataType == 'O')      // ORIGINAL
        dw = dwData;

    cBytes = pCode->m_cCodeLen + pCode->m_cDataLen;

    return pCode;
}

//to restore the data in dwData
//return the length of the whole part
BYTE CBitStream::DecodeData(UINT32& dwData, CBitCode* pBitCodes, int nNumOfCodes, const UINT32 * pdwBase, bool bReverse)
{
    dwData = 0;
    BYTE cBytes = 0;

    CBitCode* pCode = MatchCodeDecode(dwData, cBytes, pBitCodes, nNumOfCodes);
    if(pCode != NULL)
    {
        if(pCode->m_cDataType != 'O' && pdwBase != NULL)
        {
            if(bReverse)
                dwData = *pdwBase - dwData;
            else
                dwData = *pdwBase + dwData;
        }

    }

    return cBytes;
}

//to restore the data in dwData
//return the length of the whole part
BYTE CBitStream::DecodeBigInt(CBigInt& xData, CBitCode* pBitCodes, int nNumOfCodes, const CBigInt * pxBase, bool bReverse)
{
    xData = 0;
    BYTE cBytes = 0;

    UINT32 dwData = 0;
    CBitCode* pCode = MatchCodeDecode(dwData, cBytes, pBitCodes, nNumOfCodes);
    if(pCode != NULL)
    {
        if(pCode->m_cDataType == 'O')
            xData.SetRawData(dwData);
        else
        {
            if(pxBase != NULL)
            {
                if(bReverse)
                    xData = (INT64)(*pxBase) - (INT32)dwData;
                else
                    xData = (INT64)(*pxBase) + (INT32)dwData;
            }
            else
                xData = (INT32)dwData;
        }
    }

    return cBytes;
}

BYTE  CBitStream::WriteBool(bool bValue)
{
    if(bValue)
        return WriteDWORD(1, 1);
    else
        return WriteDWORD(0, 1);
}

int CBitStream::decodeData(CBitCode* pBitCodes, int nNumOfCodes, const int* ptrBaseData) {
    int restoredData = 0;
    bool isPositive = ReadDWORD(1);
    if (isPositive) {
        if (ptrBaseData) {
            unsigned int tmp = 0;
            unsigned int baseTmp = *ptrBaseData;
            DecodeData(tmp, pBitCodes, nNumOfCodes, &baseTmp);
            restoredData = tmp;
        } else {
            unsigned int tmp = 0;
            DecodeData(tmp, pBitCodes, nNumOfCodes);
            restoredData = tmp;            
        }
    } else {
        if (ptrBaseData) {
            unsigned int tmp = 0;
            unsigned int baseTmp = -(*ptrBaseData);
            DecodeData(tmp, pBitCodes, nNumOfCodes, &baseTmp);
            restoredData = -tmp;
        } else {
            unsigned int tmp = 0;
            DecodeData(tmp, pBitCodes, nNumOfCodes);
            restoredData = -tmp;
        }
    }
    
    return restoredData;
}


COutputBitStream::COutputBitStream(OutputBuffer& buffer)
    : output_(buffer),
      m_nBitLen(0),
      m_uStartPos(buffer.size()),
      m_nCurPos(0)
{
}

//To write nBits bits of the UNIT32 dw into the bit stream.
BYTE COutputBitStream::WriteDWORD(UINT32 dw, int nBits)
{
    if (nBits <= 0 || nBits > 32)
        return 0;

    if (nBits > m_nBitLen) {
        output_.append<UINT32>(0);
        m_nBitLen += 32;
    }

    PBYTE pcBuf = output_.buffer(m_uStartPos) + m_nCurPos / 8;

    int nRet = nBits;
    dw <<= (32 - nBits);
    PBYTE pcHigh = ((PBYTE)&dw) + 3;

    int nLastBits = m_nCurPos%8;
    if(nLastBits != 0)          // fill in the last byte, because there is some idle bits in the last byte of the bit stream
    {
        int nFillBits = 8 - nLastBits;                  //the number of idle bits
        *pcBuf &= (0xFF << nFillBits);                  //set the corresponding bits int the bit stream to zero for later use

        *pcBuf += (BYTE)((*pcHigh) >> nLastBits);             //Pay attention to this statement. It does not change the actual bits in the byte *pcHigh.
        pcBuf++;

        if(nFillBits < nBits)
            dw <<= nFillBits;   // else, finish             //to leave out the bits we have stored

        nBits -= nFillBits;
    }

    while (nBits > 0) {
        *pcBuf++ = *pcHigh--;
        nBits -= 8;
    }

    m_nCurPos += nRet;
    m_nBitLen -= nRet;

    return nRet;
}

//to match the appropriate data type in the array of CBitCode
//except for the DataType 'O', dw is always the minute difference
//return the matched Node
CBitCode* COutputBitStream::MatchCodeEncode(UINT32& dw, CBitCode* pBitCodes, int nNumOfCodes)
{
    if(pBitCodes == NULL || nNumOfCodes <= 0)
        return NULL;

    CBitCode* pCode = pBitCodes;
    UINT32 dwSave = 0;
    int i = 0;
    for(; i < nNumOfCodes; i++, pCode++)
    {
        if(pCode->m_cDataType == 'C')           // const
        {
            if(dw == pCode->m_dwDataBias)
                break;
        }
        else if(pCode->m_cDataType == 'D')      // UINT32
        {
            dwSave = dw - pCode->m_dwDataBias;
            if((dwSave >> pCode->m_cDataLen) == 0)
            {
                dw = dwSave;
                break;
            }
        }
        else if(pCode->m_cDataType == 'I')      // INT          Because in this case dw is a signed type, so we deal with it in two separate ways.
        {
            if(dw & 0x80000000)
            {
                dwSave = dw + pCode->m_dwDataBias;
                if((dwSave >> (pCode->m_cDataLen-1)) == (0xFFFFFFFF >> (pCode->m_cDataLen-1)))
                {
                    dw = dwSave;
                    break;
                }
            }
            else
            {
                dwSave = dw - pCode->m_dwDataBias;
                if((dwSave >> (pCode->m_cDataLen-1)) == 0)
                {
                    dw = dwSave;
                    break;
                }
            }
        }
        else if(pCode->m_cDataType == 'H')      // HUNDRED UINT32
        {
            if(dw % 100 == 0)
            {
                dwSave = dw/100 - pCode->m_dwDataBias;
                if((dwSave >> pCode->m_cDataLen) == 0)
                {
                    dw = dwSave;
                    break;
                }
            }
        }
        else if(pCode->m_cDataType == 'h')      // HUNDRED INT
        {
            INT32 iValue = dw;
            if(iValue % 100 == 0)
            {
                if(iValue < 0)
                {
                    dwSave = iValue/100 + pCode->m_dwDataBias;
                    if((dwSave >> (pCode->m_cDataLen-1)) == (0xFFFFFFFF >> (pCode->m_cDataLen-1)))
                    {
                        dw = dwSave;
                        break;
                    }
                }
                else
                {
                    dwSave = iValue/100 - pCode->m_dwDataBias;
                    if((dwSave >> (pCode->m_cDataLen-1)) == 0)
                    {
                        dw = dwSave;
                        break;
                    }
                }
            }
        }
        else if(pCode->m_cDataType == 'O')      // ORIGINAL
            break;
    }

    if(i == nNumOfCodes)
        return NULL;

    return pCode;
}

BYTE COutputBitStream::EncodeData(UINT32 dwData, CBitCode* pBitCodes, int nNumOfCodes, const UINT32 * pdwBase, const bool bReverse)
{
    UINT32 dw = dwData;
    if(pdwBase != NULL)
    {
        if(bReverse)
            dw = *pdwBase - dwData;
        else
            dw = dwData - *pdwBase;
    }

    CBitCode* pCode = MatchCodeEncode(dw, pBitCodes, nNumOfCodes);
    if(pCode == NULL)
    {
        return 0;
    }

    if(pCode->m_cDataType == 'O')           //to encode the original data into the bit stream
    {
        if(pCode->m_cDataLen == 0 || pCode->m_cDataLen != 32 && (dwData >> pCode->m_cDataLen) != 0)
        {
            return 0;
        }
        WriteDWORD(pCode->m_wCodeBits, pCode->m_cCodeLen);
        WriteDWORD(dwData, pCode->m_cDataLen);
    }
    else                                    //to encode the minute difference into the bit stream
    {
        WriteDWORD(pCode->m_wCodeBits, pCode->m_cCodeLen);
        if(pCode->m_cDataLen > 0)
            WriteDWORD(dw, pCode->m_cDataLen);
    }

    return (pCode->m_cCodeLen + pCode->m_cDataLen);
}

BYTE COutputBitStream::EncodeBigInt(CBigInt xData, CBitCode* pBitCodes, int nNumOfCodes, const CBigInt * pxBase, const bool bReverse)
{
    INT64 ll = 0;
    UINT32 dw = 0;
    if(pxBase != NULL)
    {
        if(bReverse)
            ll = (INT64)(*pxBase) - (INT64)xData;
        else
            ll = (INT64)xData - (INT64)(*pxBase);
    }
    else
        ll = (INT64)xData;

    CBitCode* pCode = NULL;

    if(ll >= CBigInt::m_nMinBase && ll <= CBigInt::m_nMaxBase)          //In this case ll is not big, so we can encode it as a normal number.
    {
        dw = (UINT32)ll;
        pCode = MatchCodeEncode(dw, pBitCodes, nNumOfCodes);
    }
    else                                                            //In this case ll is very big, so we encode it in the format of CBigInt.
    {
        CBitCode* pC = pBitCodes;
        for(int i = 0; i < nNumOfCodes; i++, pC++)
        {
            if(pC->m_cDataType == 'O')
            {
                pCode = pC;
                break;
            }
        }
    }

    if(pCode == NULL)
    {
        return 0;
    }
//When arriving here, it means pCode != NULL
    if(pCode->m_cDataType == 'O')           //to encode the original data in the format of CBigInt into the bit stream
    {
        if(pCode->m_cDataLen != 32)
        {
            return 0;
        }
        WriteDWORD(pCode->m_wCodeBits, pCode->m_cCodeLen);
        WriteDWORD(xData.GetRawData(), pCode->m_cDataLen);
    }
    else
    {
        WriteDWORD(pCode->m_wCodeBits, pCode->m_cCodeLen);
        if(pCode->m_cDataLen > 0)
            WriteDWORD(dw, pCode->m_cDataLen);
    }
    return (pCode->m_cCodeLen + pCode->m_cDataLen);
}

BYTE COutputBitStream::WriteBool(bool bValue)
{
    if (bValue)
        return WriteDWORD(1, 1);
    else
        return WriteDWORD(0, 1);
}

void COutputBitStream::encodeData(int data, CBitCode* pBitCodes, int nNumOfCodes,
    const int* ptrBaseData) {
    bool isPositive = (data > 0);
    WriteBool(isPositive);

    if (isPositive) {
        if (ptrBaseData) {
            unsigned int rawData = data;
            unsigned int baseData = (*ptrBaseData);
            EncodeData(rawData, pBitCodes, nNumOfCodes, &baseData);
        } else {
            unsigned int rawData = data;
            EncodeData(rawData, pBitCodes, nNumOfCodes);
        }        
    } else {
        if (ptrBaseData) {
            unsigned int rawData = -data;
            unsigned int baseData = -(*ptrBaseData);
            EncodeData(rawData, pBitCodes, nNumOfCodes, &baseData);
        } else {
            unsigned int rawData = -data;
            EncodeData(rawData, pBitCodes, nNumOfCodes);
        }
    }    
}

