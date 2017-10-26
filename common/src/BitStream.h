// BitStream.h: interface for the CBitStream class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BITSTREAM_H
#define BITSTREAM_H
#include "QtTypeDef.h"
#include "BigInt.h"
#include "Buffer.h"
#include "output.h"
/*
 * To understand the principle of designing the m_wCodeBits, it is advisable to read the decoding part in the function MatchCodeDecode().
 *
 * According to MatchCodeDecode(), we know that the m_wCodeBits in the back part of the array CBitCode can not "include" the m_wCodeBits in the front part
 * so that we can match the code correctly.
 *
 * By saying "including" we mean the m_wCodeBits is in the format of binary. For examples, we say "1101" includes "110", and we also say "1110" does not include "101".
 *
 */
struct CBitCode
{
    WORD    m_wCodeBits;
    BYTE    m_cCodeLen;
    char    m_cDataType;            // 'C'-const, 'D'-UINT32, 'I'-INT32, 'X'-CBigInt, 'H'-hundred UINT32, 'h'-hundred INT
    BYTE    m_cDataLen;
    UINT32  m_dwDataBias;
};

class CBitStream
{
public:
    CBitStream();                                                           //m_cBufType = 2;
    CBitStream(CPackBuffer*, bool bRead);                                    //m_cBufType = 0;
    CBitStream(PBYTE pcBuf, int nBufLen);                                    //m_cBufType = 1;
    CBitStream(int nBufLen);                                                //m_cBufType = 2;

    virtual ~CBitStream();

    void SetBuffer(PBYTE pcBuf, int nBufLen);

    int  GetCurPos()    { return m_nCurPos; }

    BYTE  WriteDWORD(UINT32 dw, int nBit);
    UINT32 ReadDWORD(int nBit, bool bMovePos = true);           //make sure the m_nCurPos is in the correct position so that we can read the data we want successfully

    BYTE EncodeData(UINT32 dwData, CBitCode* pBitCodes, int nNumOfCodes, const UINT32 * pdwBase=NULL, const bool bReverse=false);
    BYTE DecodeData(UINT32& dwData, CBitCode* pBitCodes, int nNumOfCodes, const UINT32 * pdwBase=NULL, const bool bReverse=false);

    BYTE EncodeBigInt(CBigInt xData, CBitCode* pBitCodes, int nNumOfCodes, const CBigInt * pxBase=NULL, const bool bReverse=false);
    BYTE DecodeBigInt(CBigInt& xData, CBitCode* pBitCodes, int nNumOfCodes, const CBigInt * pxBase=NULL, const bool bReverse=false);

    BYTE  WriteBool(bool bValue);

    int decodeData(CBitCode* pBitCodes, int nNumOfCodes, const int* ptrBaseData=NULL);

private:
    int ReallocBuf(int nBits);
    CBitCode* MatchCodeEncode(UINT32& dw, CBitCode* pBitCodes, int nNumOfCodes);
    CBitCode* MatchCodeDecode(UINT32& dw, BYTE& cBytes, CBitCode* pBitCodes, int nNumOfCodes);

private:
    char            m_cBufType;      // 0: CBitStream(CPackBuffer*)
                                    // 1: CBitStream(PBYTE pcBuf, int nBufLen) or SetBuffer(PBYTE pcBuf, int nBufLen)
                                    // 2: CBitStream(int nBufLen)
    PBYTE           m_pcBuf;
    int             m_nBitLen;

    CPackBuffer*    m_pBuffer;
    UINT            m_uStartPos;

    int             m_nCurPos;       //its unit is bit.
};

class COutputBitStream
{
public:
    explicit COutputBitStream(OutputBuffer& buffer);

    virtual ~COutputBitStream() {}

    int GetCurPos()    { return m_nCurPos; }

    BYTE WriteDWORD(UINT32 dw, int nBit);
    BYTE EncodeData(UINT32 dwData, CBitCode* pBitCodes, int nNumOfCodes,
                    const UINT32 * pdwBase=NULL, const bool bReverse=false);
    BYTE EncodeBigInt(CBigInt xData, CBitCode* pBitCodes, int nNumOfCodes,
                      const CBigInt * pxBase=NULL, const bool bReverse=false);
    BYTE WriteBool(bool bValue);

    // This interface will be deprecated in the near future.
    void encodeData(int data, CBitCode* pBitCodes, int nNumOfCodes,
        const int* ptrBaseData=NULL);
private:
    CBitCode* MatchCodeEncode(UINT32& dw, CBitCode* pBitCodes, int nNumOfCodes);

private:
    OutputBuffer& output_;
    int           m_nBitLen;
    UINT          m_uStartPos;
    int           m_nCurPos;       //its unit is bit.
};

#endif
