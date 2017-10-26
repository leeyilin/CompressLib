
#ifndef CPACK_H
#define CPACK_H

#ifdef LD_WINDOWS
#define INT64 _int64
#else 
#define INT64 long long int
#define BYTE unsigned char
#endif

#if _MSC_VER > 1000
#pragma warning(disable: 4786)
#pragma warning(disable: 4503)
#include "..\common\staticstring.h"
using namespace BackCom;
#else
#include "staticstring.h"
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
using namespace std;

namespace BackCom
{

struct SPackHead
{
    int             m_intPackSize;
    int             m_intPackNo;
    unsigned short  m_shtVer;
    unsigned short  m_shtType;
};

class CPack
{
public:
    enum    {    _const_pack_head_size = 12 };
    enum ErrorCode    {  Err_NoErr=0
                        ,Err_OutOfMem
                        ,Err_UnsupportVer
                        ,Err_UnknownType
                        ,Err_ContentErr
                        ,Err_Internal
                      };

    //
public:
    static ErrorCode PackHead(char *pchPackHead
                             ,int & nOutPackHeadSize
                             ,const int intPackSize
                             ,const int intPackNo
                             ,const unsigned short shtVersion
                             ,const unsigned short shtType
                             );
    static ErrorCode UnpackHead(int & intOutPackSize
                               ,int & intPackNo
                               ,unsigned short & shtVersion
                               ,unsigned short & shtType
                               ,const char *pchPack
                               ,const int intPackSize
                               );
    //
    static ErrorCode PackHead(char *pchPackHead
                             ,int & nOutPackHeadSize
                             ,const SPackHead *pHead
                             );
    static ErrorCode UnpackHead(SPackHead *pHead
                               ,const char *pchPack
                               ,const int intPackSize
                               );
    //
    static int SafeUnpackString(const char * pBuf, const int intLen, string & str);

    static int UnpackStringList(const char * pBuf, const int intLen, vector<string> & data);

    static bool UnpackHead(const char * pBuf, const int intLen, const unsigned short shtType, const unsigned short shtVer, int & intPackNo);
    
    template<class _Tp>
    static ErrorCode Pack1(char *pchBuf
                          ,int & intSize
                          ,const int intPackNo
                          ,const unsigned short shtVersion
                          ,const unsigned short shtType
                          ,const _Tp & value
                          )
    {
        int nTmp = _const_pack_head_size + 0;
        nTmp += Output(pchBuf+ nTmp,value);
        if( intSize < nTmp )
        {
            intSize = nTmp;
            return Err_OutOfMem;
        }
        return PackHead(pchBuf,intSize,nTmp,intPackNo,shtVersion,shtType);
    }

    template<class _Tp>
    static ErrorCode Unpack1(int & intPackNo
                            ,unsigned short & shtVersion
                            ,unsigned short & shtType
                            ,_Tp & value
                            ,const char *pchPack
                            ,const int intPackSize
                            )
    {
        int nTmp = _const_pack_head_size + 0;
        nTmp += Input(pchPack+ nTmp,value);
        if( intPackSize < nTmp )
        {
            return Err_ContentErr;
        }
        return UnpackHead(nTmp,intPackNo,shtVersion,shtType,pchPack,intPackSize);
    }

    template<class _Tp1,class _Tp2>
    static ErrorCode Pack2(char *pchBuf
                          ,int & intSize
                          ,const int intPackNo
                          ,const unsigned short shtVersion
                          ,const unsigned short shtType
                          ,const _Tp1 & value1
                          ,const _Tp2 & value2
                          )
    {
        int nTmp = _const_pack_head_size + 0;
        nTmp += Output(pchBuf+ nTmp,value1);
        nTmp += Output(pchBuf+ nTmp,value2);
        if( intSize < nTmp )
        {
            intSize = nTmp;
            return Err_OutOfMem;
        }
        return PackHead(pchBuf,intSize,nTmp,intPackNo,shtVersion,shtType);
    }

    template<class _Tp1,class _Tp2>
    static ErrorCode Unpack2(int & intPackNo
                            ,unsigned short & shtVersion
                            ,unsigned short & shtType
                            ,_Tp1 & value1
                            ,_Tp2 & value2
                            ,const char *pchPack
                            ,const int intPackSize
                            )
    {
        int nTmp = _const_pack_head_size + 0;
        nTmp += Input(pchPack+ nTmp,value1);
        nTmp += Input(pchPack+ nTmp,value2);
        if( intPackSize < nTmp )
        {
            return Err_ContentErr;
        }
        return UnpackHead(nTmp,intPackNo,shtVersion,shtType,pchPack,intPackSize);
    }

    template<class _Tp1,class _Tp2,class _Tp3>
    static ErrorCode Pack3(char *pchBuf
                          ,int & intSize
                          ,const int intPackNo
                          ,const unsigned short shtVersion
                          ,const unsigned short shtType
                          ,const _Tp1 & value1
                          ,const _Tp2 & value2
                          ,const _Tp3 & value3
                          )
    {
        int nTmp = _const_pack_head_size + 0;
        nTmp += Output(pchBuf+ nTmp,value1);
        nTmp += Output(pchBuf+ nTmp,value2);
        nTmp += Output(pchBuf+ nTmp,value3);
        if( intSize < nTmp )
        {
            intSize = nTmp;
            return Err_OutOfMem;
        }
        return PackHead(pchBuf,intSize,nTmp,intPackNo,shtVersion,shtType);
    }

    template<class _Tp1,class _Tp2,class _Tp3>
    static ErrorCode Unpack3(int & intPackNo
                            ,unsigned short & shtVersion
                            ,unsigned short & shtType
                            ,_Tp1 & value1
                            ,_Tp2 & value2
                            ,_Tp3 & value3
                            ,const char *pchPack
                            ,const int intPackSize
                            )
    {
        int nTmp = _const_pack_head_size + 0;
        nTmp += Input(pchPack+ nTmp,value1);
        nTmp += Input(pchPack+ nTmp,value2);
        nTmp += Input(pchPack+ nTmp,value3);
        if( intPackSize < nTmp )
        {
            return Err_ContentErr;
        }
        return UnpackHead(nTmp,intPackNo,shtVersion,shtType,pchPack,intPackSize);
    }

    template<class _Tp1,class _Tp2,class _Tp3,class _Tp4>
    static ErrorCode Pack4(char *pchBuf
                          ,int & intSize
                          ,const int intPackNo
                          ,const unsigned short shtVersion
                          ,const unsigned short shtType
                          ,const _Tp1 & value1
                          ,const _Tp2 & value2
                          ,const _Tp3 & value3
                          ,const _Tp4 & value4
                          )
    {
        int nTmp = _const_pack_head_size + 0;
        nTmp += Output(pchBuf+ nTmp,value1);
        nTmp += Output(pchBuf+ nTmp,value2);
        nTmp += Output(pchBuf+ nTmp,value3);
        nTmp += Output(pchBuf+ nTmp,value4);
        if( intSize < nTmp )
        {
            intSize = nTmp;
            return Err_OutOfMem;
        }
        return PackHead(pchBuf,intSize,nTmp,intPackNo,shtVersion,shtType);
    }

    template<class _Tp1,class _Tp2,class _Tp3,class _Tp4>
    static ErrorCode Unpack4(int & intPackNo
                            ,unsigned short & shtVersion
                            ,unsigned short & shtType
                            ,_Tp1 & value1
                            ,_Tp2 & value2
                            ,_Tp3 & value3
                            ,_Tp4 & value4
                            ,const char *pchPack
                            ,const int intPackSize
                            )
    {
        int nTmp = _const_pack_head_size + 0;
        nTmp += Input(pchPack+ nTmp,value1);
        nTmp += Input(pchPack+ nTmp,value2);
        nTmp += Input(pchPack+ nTmp,value3);
        nTmp += Input(pchPack+ nTmp,value4);
        if( intPackSize < nTmp )
        {
            return Err_ContentErr;
        }
        return UnpackHead(nTmp,intPackNo,shtVersion,shtType,pchPack,intPackSize);
    }

    template<class _Tp1,class _Tp2,class _Tp3,class _Tp4,class _Tp5>
    static ErrorCode Pack5(char *pchBuf
                          ,int & intSize
                          ,const int intPackNo
                          ,const unsigned short shtVersion
                          ,const unsigned short shtType
                          ,const _Tp1 & value1
                          ,const _Tp2 & value2
                          ,const _Tp3 & value3
                          ,const _Tp4 & value4
                          ,const _Tp5 & value5
                          )
    {
        int nTmp = _const_pack_head_size + 0;
        nTmp += Output(pchBuf+ nTmp,value1);
        nTmp += Output(pchBuf+ nTmp,value2);
        nTmp += Output(pchBuf+ nTmp,value3);
        nTmp += Output(pchBuf+ nTmp,value4);
        nTmp += Output(pchBuf+ nTmp,value5);
        if( intSize < nTmp )
        {
            intSize = nTmp;
            return Err_OutOfMem;
        }
        return PackHead(pchBuf,intSize,nTmp,intPackNo,shtVersion,shtType);
    }

    template<class _Tp1,class _Tp2,class _Tp3,class _Tp4,class _Tp5>
    static ErrorCode Unpack5(int & intPackNo
                            ,unsigned short & shtVersion
                            ,unsigned short & shtType
                            ,_Tp1 & value1
                            ,_Tp2 & value2
                            ,_Tp3 & value3
                            ,_Tp4 & value4
                            ,_Tp5 & value5
                            ,const char *pchPack
                            ,const int intPackSize
                            )
    {
        int nTmp = _const_pack_head_size + 0;
        nTmp += Input(pchPack+ nTmp,value1);
        nTmp += Input(pchPack+ nTmp,value2);
        nTmp += Input(pchPack+ nTmp,value3);
        nTmp += Input(pchPack+ nTmp,value4);
        nTmp += Input(pchPack+ nTmp,value5);
        if( intPackSize < nTmp )
        {
            return Err_ContentErr;
        }
        return UnpackHead(nTmp,intPackNo,shtVersion,shtType,pchPack,intPackSize);
    }

    template<class _Tp1,class _Tp2,class _Tp3,class _Tp4,class _Tp5,class _Tp6>
    static ErrorCode Pack6(char *pchBuf
                          ,int & intSize
                          ,const int intPackNo
                          ,const unsigned short shtVersion
                          ,const unsigned short shtType
                          ,const _Tp1 & value1
                          ,const _Tp2 & value2
                          ,const _Tp3 & value3
                          ,const _Tp4 & value4
                          ,const _Tp5 & value5
                          ,const _Tp6 & value6
                          )
    {
        int nTmp = _const_pack_head_size + 0;
        nTmp += Output(pchBuf+ nTmp,value1);
        nTmp += Output(pchBuf+ nTmp,value2);
        nTmp += Output(pchBuf+ nTmp,value3);
        nTmp += Output(pchBuf+ nTmp,value4);
        nTmp += Output(pchBuf+ nTmp,value5);
        nTmp += Output(pchBuf+ nTmp,value6);
        if( intSize < nTmp )
        {
            intSize = nTmp;
            return Err_OutOfMem;
        }
        return PackHead(pchBuf,intSize,nTmp,intPackNo,shtVersion,shtType);
    }

    template<class _Tp1,class _Tp2,class _Tp3,class _Tp4,class _Tp5,class _Tp6>
    static ErrorCode Unpack6(int & intPackNo
                            ,unsigned short & shtVersion
                            ,unsigned short & shtType
                            ,_Tp1 & value1
                            ,_Tp2 & value2
                            ,_Tp3 & value3
                            ,_Tp4 & value4
                            ,_Tp5 & value5
                            ,_Tp6 & value6
                            ,const char *pchPack
                            ,const int intPackSize
                            )
    {
        int nTmp = _const_pack_head_size + 0;
        nTmp += Input(pchPack+ nTmp,value1);
        nTmp += Input(pchPack+ nTmp,value2);
        nTmp += Input(pchPack+ nTmp,value3);
        nTmp += Input(pchPack+ nTmp,value4);
        nTmp += Input(pchPack+ nTmp,value5);
        nTmp += Input(pchPack+ nTmp,value6);
        if( intPackSize < nTmp )
        {
            return Err_ContentErr;
        }
        return UnpackHead(nTmp,intPackNo,shtVersion,shtType,pchPack,intPackSize);
    }

public:
    static int Input(const char* pchSrc, char & chrVal) ;
    static int Input(const char* pchSrc, BYTE & chrVal) ;
    static int Input(const char* pchSrc, short & shtVal) ;
    static int Input(const char* pchSrc, unsigned short & shtVal) ;
    static int Input(const char* pchSrc, int & intVal) ;
    static int Input(const char* pchSrc, unsigned int & intVal) ;
    static int Input(const char* pchSrc, float & fltVal) ;
    static int Input(const char* pchSrc, double & dblVal) ;
    static int Input(const char* pchSrc, INT64 & int64Val) ;
    static int Input(const char* pchSrc, string & strVal) ;
    static int Input(const char* pchSrc, SPackHead & head) ;
    template <int __size>
    static int Input(const char* pchSrc, CStaticString<__size> & fixStr) ;
    //
    static int Output(char* pchDest, const char chrVal)  ;
    static int Output(char* pchDest, const BYTE chrVal)  ;
    static int Output(char* pchDest, const short shtVal)  ;
    static int Output(char* pchDest, const unsigned short shtVal)  ;
    static int Output(char* pchDest, const int intVal)  ;
    static int Output(char* pchDest, const unsigned int intVal)  ;
    static int Output(char* pchDest, const float fltVal)  ;
    static int Output(char* pchDest, const double dblVal)  ;
    static int Output(char* pchDest, const INT64 int64Val) ;
    static int Output(char* pchDest, const string & strVal)  ;
    static int Output(char* pchDest, const SPackHead & head) ;
    template <int __size>
    static int Output(char* pchDest, const CStaticString<__size> & fixStr);

    static void ConvertOrder (short &shtVal);
    static void ConvertOrder( int &intVal);
    static void ConvertOrder( float &fltVal);
    static void ConvertOrder( double &dblVal);
    static int GetStrBufSize(const string & str){return str.size() + sizeof(unsigned short);};
    static int GetBigStrBufSize(const string & str){return str.size() + sizeof(int);};
    static int PackBigStr(char* pchDest, const string & strVal);
    static int PackString(vector<char> & buf, const string & strVal);
    static int PackBigStr(vector<char> & buf, const string & strVal);
};

inline int CPack::Input(const char* pchSrc, char & chrVal)
{
    chrVal = *pchSrc;
    return sizeof(chrVal);
}

inline int CPack::Input(const char* pchSrc, BYTE & chrVal)
{
    chrVal = *pchSrc;
    return sizeof(chrVal);
}

inline int CPack::Input(const char* pchSrc, short & shtVal)
{
    char *pch = (char*)&shtVal;
#if defined(_BIG_ENDIAN)
    *(pch+0) = *(pchSrc+0);
    *(pch+1) = *(pchSrc+1);
#else
    *(pch+1) = *(pchSrc+0);
    *(pch+0) = *(pchSrc+1);
#endif
    return sizeof(shtVal);
}

inline int CPack::Input(const char* pchSrc, unsigned short & shtVal)
{
    char *pch = (char*)&shtVal;
#if defined(_BIG_ENDIAN)
    *(pch+0) = *(pchSrc+0);
    *(pch+1) = *(pchSrc+1);
#else
    *(pch+1) = *(pchSrc+0);
    *(pch+0) = *(pchSrc+1);
#endif
    return sizeof(shtVal);
}

inline int CPack::Input(const char* pchSrc, int & intVal)
{
    char *pch = (char*)&intVal;
#if defined(_BIG_ENDIAN)
    *(pch+0) = *(pchSrc+0);
    *(pch+1) = *(pchSrc+1);
    *(pch+2) = *(pchSrc+2);
    *(pch+3) = *(pchSrc+3);
#else
    *(pch+3) = *(pchSrc+0);
    *(pch+2) = *(pchSrc+1);
    *(pch+1) = *(pchSrc+2);
    *(pch+0) = *(pchSrc+3);
#endif
    return sizeof(intVal);

}

inline int CPack::Input(const char* pchSrc, unsigned int & intVal)
{
    char *pch = (char*)&intVal;
#if defined(_BIG_ENDIAN)
    *(pch+0) = *(pchSrc+0);
    *(pch+1) = *(pchSrc+1);
    *(pch+2) = *(pchSrc+2);
    *(pch+3) = *(pchSrc+3);
#else
    *(pch+3) = *(pchSrc+0);
    *(pch+2) = *(pchSrc+1);
    *(pch+1) = *(pchSrc+2);
    *(pch+0) = *(pchSrc+3);
#endif
    return sizeof(intVal);

}

inline int CPack::Input(const char* pchSrc, float & fltVal)
{
    char *pch = (char*)&fltVal;
#if defined(_BIG_ENDIAN)
    *(pch+0) = *(pchSrc+0);
    *(pch+1) = *(pchSrc+1);
    *(pch+2) = *(pchSrc+2);
    *(pch+3) = *(pchSrc+3);
#else
    *(pch+3) = *(pchSrc+0);
    *(pch+2) = *(pchSrc+1);
    *(pch+1) = *(pchSrc+2);
    *(pch+0) = *(pchSrc+3);
#endif
    return sizeof(fltVal);
}

inline int CPack::Input(const char* pchSrc, double & dblVal)
{
    char *pch = (char*)&dblVal;
#if defined(_BIG_ENDIAN)
    *(pch+0) = *(pchSrc+0);
    *(pch+1) = *(pchSrc+1);

    *(pch+2) = *(pchSrc+2);
    *(pch+3) = *(pchSrc+3);
    *(pch+4) = *(pchSrc+4);
    *(pch+5) = *(pchSrc+5);
    *(pch+6) = *(pchSrc+6);
    *(pch+7) = *(pchSrc+7);
#else
    *(pch+7) = *(pchSrc+0);
    *(pch+6) = *(pchSrc+1);
    *(pch+5) = *(pchSrc+2);
    *(pch+4) = *(pchSrc+3);
    *(pch+3) = *(pchSrc+4);
    *(pch+2) = *(pchSrc+5);
    *(pch+1) = *(pchSrc+6);
    *(pch+0) = *(pchSrc+7);
#endif
    return sizeof(dblVal);
}

inline int CPack::Input(const char* pchSrc, INT64 & int64Val)
{
    char *pch = (char*)&int64Val;
#if defined(_BIG_ENDIAN)
    *(pch+0) = *(pchSrc+0);
    *(pch+1) = *(pchSrc+1);

    *(pch+2) = *(pchSrc+2);
    *(pch+3) = *(pchSrc+3);
    *(pch+4) = *(pchSrc+4);
    *(pch+5) = *(pchSrc+5);
    *(pch+6) = *(pchSrc+6);
    *(pch+7) = *(pchSrc+7);
#else
    *(pch+7) = *(pchSrc+0);
    *(pch+6) = *(pchSrc+1);
    *(pch+5) = *(pchSrc+2);
    *(pch+4) = *(pchSrc+3);
    *(pch+3) = *(pchSrc+4);
    *(pch+2) = *(pchSrc+5);
    *(pch+1) = *(pchSrc+6);
    *(pch+0) = *(pchSrc+7);
#endif
    return sizeof(int64Val);
}

inline int CPack::Input(const char* pchSrc, string & strVal)
{
    unsigned short size = 0;
    int intLen = Input(pchSrc, size);
    vector<char> tmp;
    tmp.resize(size + 1);
    memcpy(&*tmp.begin(), pchSrc + intLen, size);
    strVal = &*tmp.begin();
    intLen += size;
    return intLen;
}


inline int CPack::Input(const char* pchSrc, SPackHead & head)
{
    Input(pchSrc+ 0,head.m_intPackSize);
    Input(pchSrc+ 4,head.m_intPackNo);
    Input(pchSrc+ 8,head.m_shtVer);
    Input(pchSrc+10,head.m_shtType);
    return sizeof(head);
}

template <int __size>
inline int CPack::Input(const char* pchSrc, CStaticString<__size> & fixStr)
{
    unsigned short size = 0;
    int intLen = Input(pchSrc, size);
    vector<char> tmp;
    tmp.resize(size + 1);
    memcpy(&*tmp.begin(), pchSrc + intLen, size);
    fixStr = &*tmp.begin();
    intLen += size;
    return intLen;
}

inline int CPack::Output(char* pchDest, const char chrVal)
{
    const char *pch = (const char*)&chrVal;
    *(pchDest+0) = *(pch+0);
    return sizeof(chrVal);
}

inline int CPack::Output(char* pchDest, const BYTE chrVal)
{
    const char *pch = (const char*)&chrVal;
    *(pchDest+0) = *(pch+0);
    return sizeof(chrVal);
}

inline int CPack::Output(char* pchDest, const short shtVal)
{
    const char *pch = (const char*)&shtVal;
#if defined(_BIG_ENDIAN)
    *(pchDest+0) = *(pch+0);
    *(pchDest+1) = *(pch+1);
#else
    *(pchDest+0) = *(pch+1);
    *(pchDest+1) = *(pch+0);
#endif
    return sizeof(shtVal);
}

inline int CPack::Output(char* pchDest, const unsigned short shtVal)
{
    const char *pch = (const char*)&shtVal;
#if defined(_BIG_ENDIAN)
    *(pchDest+0) = *(pch+0);
    *(pchDest+1) = *(pch+1);
#else
    *(pchDest+0) = *(pch+1);
    *(pchDest+1) = *(pch+0);
#endif
    return sizeof(shtVal);
}

inline int CPack::Output(char* pchDest, const int intVal)
{
    const char *pch = (const char*)&intVal;
#if defined(_BIG_ENDIAN)
    *(pchDest+0) = *(pch+0);
    *(pchDest+1) = *(pch+1);
    *(pchDest+2) = *(pch+2);
    *(pchDest+3) = *(pch+3);
#else
    *(pchDest+0) = *(pch+3);
    *(pchDest+1) = *(pch+2);
    *(pchDest+2) = *(pch+1);
    *(pchDest+3) = *(pch+0);
#endif
    return sizeof(intVal);
}

inline int CPack::Output(char* pchDest, const unsigned int intVal)
{
    const char *pch = (const char*)&intVal;
#if defined(_BIG_ENDIAN)
    *(pchDest+0) = *(pch+0);
    *(pchDest+1) = *(pch+1);
    *(pchDest+2) = *(pch+2);
    *(pchDest+3) = *(pch+3);
#else
    *(pchDest+0) = *(pch+3);
    *(pchDest+1) = *(pch+2);
    *(pchDest+2) = *(pch+1);
    *(pchDest+3) = *(pch+0);
#endif
    return sizeof(intVal);
}

inline int CPack::Output(char* pchDest, const float fltVal)
{
    const char *pch = (const char*)&fltVal;
#if defined(_BIG_ENDIAN)
    *(pchDest+0) = *(pch+0);
    *(pchDest+1) = *(pch+1);
    *(pchDest+2) = *(pch+2);
    *(pchDest+3) = *(pch+3);
#else
    *(pchDest+0) = *(pch+3);
    *(pchDest+1) = *(pch+2);
    *(pchDest+2) = *(pch+1);
    *(pchDest+3) = *(pch+0);
#endif
    return sizeof(fltVal);
}

inline int CPack::Output(char* pchDest, const double dblVal)
{
    const char *pch = (const char*)&dblVal;
#if defined(_BIG_ENDIAN)
    *(pchDest+0) = *(pch+0);
    *(pchDest+1) = *(pch+1);
    *(pchDest+2) = *(pch+2);
    *(pchDest+3) = *(pch+3);
    *(pchDest+4) = *(pch+4);
    *(pchDest+5) = *(pch+5);
    *(pchDest+6) = *(pch+6);
    *(pchDest+7) = *(pch+7);
#else
    *(pchDest+0) = *(pch+7);
    *(pchDest+1) = *(pch+6);
    *(pchDest+2) = *(pch+5);
    *(pchDest+3) = *(pch+4);
    *(pchDest+4) = *(pch+3);
    *(pchDest+5) = *(pch+2);
    *(pchDest+6) = *(pch+1);
    *(pchDest+7) = *(pch+0);
#endif
    return sizeof(dblVal);
}

inline int CPack::Output(char* pchDest, const INT64 int64Val)
{
     const char *pch = (const char*)&int64Val;
#if defined(_BIG_ENDIAN)
    *(pchDest+0) = *(pch+0);
    *(pchDest+1) = *(pch+1);
    *(pchDest+2) = *(pch+2);
    *(pchDest+3) = *(pch+3);
    *(pchDest+4) = *(pch+4);
    *(pchDest+5) = *(pch+5);
    *(pchDest+6) = *(pch+6);
    *(pchDest+7) = *(pch+7);
#else
    *(pchDest+0) = *(pch+7);
    *(pchDest+1) = *(pch+6);
    *(pchDest+2) = *(pch+5);
    *(pchDest+3) = *(pch+4);
    *(pchDest+4) = *(pch+3);
    *(pchDest+5) = *(pch+2);
    *(pchDest+6) = *(pch+1);
    *(pchDest+7) = *(pch+0);
#endif
    return sizeof(int64Val);
}

inline int CPack::Output(char* pchDest, const string & strVal)
{
    unsigned short size = strVal.size();
    int intLen = Output(pchDest, size);
    memcpy(pchDest + intLen, strVal.c_str(), size);
    intLen += size;
    return intLen;
}

inline int CPack::PackBigStr(char* pchDest, const string & strVal)
{
    int size = strVal.size();
    int intLen = Output(pchDest, size);
    memcpy(pchDest + intLen, strVal.c_str(), size);
    intLen += size;
    return intLen;
}

inline int CPack::PackString(vector<char> & buf, const string & strVal)
{
    int intOldSize = buf.size();
    buf.resize(intOldSize + GetStrBufSize(strVal));
    return Output(&*buf.begin() + intOldSize, strVal);

}

inline int CPack::PackBigStr(vector<char> & buf, const string & strVal)
{
    int intOldSize = buf.size();
    buf.resize(intOldSize + GetBigStrBufSize(strVal));
    return PackBigStr(&*buf.begin() + intOldSize, strVal);
}

inline int CPack::Output(char* pchDest, const SPackHead & head)
{
    Output(pchDest+ 0,head.m_intPackSize);
    Output(pchDest+ 4,head.m_intPackNo);
    Output(pchDest+ 8,head.m_shtVer);
    Output(pchDest+10,head.m_shtType);
    return sizeof(head);
}

template <int __size>
inline int CPack::Output(char* pchDest, const CStaticString<__size> & fixStr)
{
    unsigned short size = fixStr.size();
    int intLen = Output(pchDest, size);
    memcpy(pchDest + intLen, fixStr.c_str(), size);
    intLen += size;
    return intLen;
}

//add 2008-03-17 by zhangcx
inline void CPack::ConvertOrder (short &shtVal)
{
    char *pchSrc = ( char*)&shtVal;
    char pchTemp;
    pchTemp = *(pchSrc+0);
    *(pchSrc+0) = *(pchSrc+1);
    *(pchSrc+1) = pchTemp;
}
inline void CPack::ConvertOrder( int &intVal)
{
    char *pchSrc = (char*)&intVal;
    char pchTemp;
    pchTemp = *(pchSrc+0);
    *(pchSrc+0) = *(pchSrc+3);
    *(pchSrc+3) = pchTemp;

    pchTemp = *(pchSrc+1);
    *(pchSrc+1) = *(pchSrc+2);
    *(pchSrc+2) = pchTemp;
}
inline void CPack::ConvertOrder( float &fltVal)
{
    char *pchSrc = (char*)&fltVal;
    char pchTemp;
    pchTemp = *(pchSrc+0);
    *(pchSrc+0) = *(pchSrc+3);
    *(pchSrc+3) = pchTemp;

    pchTemp = *(pchSrc+1);
    *(pchSrc+1) = *(pchSrc+2);
    *(pchSrc+2) = pchTemp;
}


inline void CPack::ConvertOrder( double &dblVal)
{
    char *pchSrc = ( char*)&dblVal;
    char pchTemp;
    pchTemp = *(pchSrc+0);
    *(pchSrc+0) = *(pchSrc+7);
    *(pchSrc+7) = pchTemp;

    pchTemp = *(pchSrc+1);
    *(pchSrc+1) = *(pchSrc+6);
    *(pchSrc+6) = pchTemp;

    pchTemp = *(pchSrc+2);
    *(pchSrc+2) = *(pchSrc+5);
    *(pchSrc+5) = pchTemp;

    pchTemp = *(pchSrc+3);
    *(pchSrc+3) = *(pchSrc+4);
    *(pchSrc+4) = pchTemp;
}

//add 2008-03-17 end



}// namespace BackCom
#endif
