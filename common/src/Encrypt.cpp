
#include "Encrypt.h"
#include <string.h>

namespace BackCom
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEncrypt::CEncrypt(const char * lpszKey)
{
  if (!lpszKey)
    {
        strcpy(m_szKey, "DefaultEncryptKey");
    }
    else
    {
    strncpy(m_szKey, lpszKey, sizeof(m_szKey));
    m_szKey[sizeof(m_szKey)-1] = '\0';
    }
}

CEncrypt::~CEncrypt()
{

}

void CEncrypt::EncryptData(void * pData, int nLen)
{
    long lKey = GetKey(m_szKey);
    unsigned char baKey[] = { 0 , 0 , 0 , 0 };
    baKey[0] = (unsigned char)((lKey>>24) & 255);
    baKey[1] = (unsigned char)((lKey>>16) & 255);
    baKey[2] = (unsigned char)((lKey>> 8) & 255);
    baKey[3] = (unsigned char)((lKey>> 0) & 255);
    int i;
    for (i = 0 ; i < nLen ; i ++ )
    {
        ((unsigned char *)pData)[i] ^= baKey[i % 4];
    }
}

long CEncrypt::GetKey(const char * lpszKey)
{
    long lKey = 0;
    if(!lpszKey)
    {
        return 0;
    }
    else if (strlen(lpszKey) == 0)
    {
        return 0;
    }
    int nLen = strlen(lpszKey);
    
    unsigned char baKey[] = { 0 , 0 , 0 , 0 };
    for ( int i = 0 ; i < nLen ; i ++ )
    {
        baKey[ i % 4 ] ^= (unsigned char)lpszKey[i] ^ (nLen - i - 1);
    }
    lKey += ((long)baKey[0]) << 24 ;
    lKey += ((long)baKey[1]) << 16 ;
    lKey += ((long)baKey[2]) <<  8 ;
    lKey += ((long)baKey[3]) ;
    return lKey;
}

}// namespace BackCom
