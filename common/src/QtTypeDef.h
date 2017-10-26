#ifndef QTTYPEDEF_H
#define QTTYPEDEF_H

#ifdef _MSC_VER //win

    #define INT64 __int64
    #define UINT64 unsigned __int64
    #define UINT unsigned int
    #define BYTE unsigned char
    #define WORD unsigned short

#else //*nix

    #define INT64 long long int
    #define UINT64 unsigned long long int
    
    #ifdef _LINUX_SYS //your own define
        #define UINT unsigned int
        #define BYTE unsigned char
        #define WORD unsigned short
    #endif

#endif

#define INT16 short
#define UINT16 unsigned short
#define INT32 int
#define UINT32 unsigned int
#define PBYTE unsigned char *
#define PDWORD unsigned int *

#define MARKET_QQZS  100

const int ERR_COMPRESS_PARAM = -1;
const int ERR_COMPRESS_BITBUF = -2;
const int ERR_COMPRESS_SRCDATA = -3;
const int ERR_COMPRESS_ENCODEDATA = -4;
const int ERR_COMPRESS_CHECK = -5;
const int ERR_COMPRESS_NODATA = -100;

inline BYTE GetBytesCheckSum(const PBYTE pcData, int nNumBytes)
{
    BYTE cCheckSum = 0;

    const PBYTE pcEnd = pcData + nNumBytes;
    for(const PBYTE pc = pcData; pc < pcEnd; pc++)
        cCheckSum += *pc;

    return cCheckSum;
}

inline UINT32 GetSecondsDiff(UINT32 dwTime1, UINT32 dwTime2, const bool blnCanBack = false)     // hhmmss
{
    if(blnCanBack == false)
    {
        if(dwTime2 < dwTime1)
            return 0;
    }
    
    return (dwTime2/10000-dwTime1/10000)*3600 + (dwTime2%10000/100-dwTime1%10000/100)*60 + (dwTime2%100-dwTime1%100);
}


inline UINT32 AddSeconds(UINT32 dwTime1, UINT32 dwAddSeconds)       // hhmmss
{
    UINT32 dwSeconds = dwTime1/10000*3600 + dwTime1%10000/100*60 + dwTime1%100;

    dwSeconds += dwAddSeconds;

    return dwSeconds/3600*10000 + dwSeconds%3600/60*100 + dwSeconds%60;
}

inline WORD GetMinutesDiff(WORD wTime1, WORD wTime2)            // hhmm
{
    if(wTime2 < wTime1)
        return 0;

    return (wTime2/100-wTime1/100)*60 + (wTime2%100-wTime1%100);
}

inline WORD AddMinutes(WORD wTime1, WORD wAddMinutes)           // hhmm
{
    WORD wHour = wTime1/100;
    WORD wMinute = wTime1%100;

    wMinute += wAddMinutes;
    if(wMinute >= 60)
    {
        wHour += (wMinute/60);
        wMinute = wMinute%60;

        if(wHour >= 24)
            wHour = wHour%24;
    }

    return wHour*100 + wMinute;
}

#ifndef DLL_PROJECT
#define CLASSINDLL_CLASS_DECL
#else
#ifdef _CLASSINDLL 
 #define CLASSINDLL_CLASS_DECL __declspec(dllexport)
#else
 #define CLASSINDLL_CLASS_DECL __declspec(dllimport)
#endif
#endif
#endif
