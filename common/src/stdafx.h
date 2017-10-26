#ifndef STDAFX_H
#define STDAFX_H
#include "string.h"
inline void ZeroMemory(void * pBuf, const int intSize)
{
    memset(pBuf, 0, intSize);
}

inline void CopyMemory(void * pSrc, const void * pDest, const int intSize)
{
    memcpy(pSrc, pDest, intSize);
}

inline void MoveMemory(void * pSrc, const void * pDest, const int intSize)
{
    memmove(pSrc, pDest, intSize);
}
#define CMutex CLock
#define CTime CComDateTime
#define CTimeSpan CComDateTimeSpan
#endif
