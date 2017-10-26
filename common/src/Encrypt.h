
#ifndef CENCRYPacket_H
#define CENCRYPacket_H

namespace BackCom
{
class CEncrypt
{
public:
    CEncrypt(const char* lpszKey);
    virtual ~CEncrypt();
    void EncryptData(void * pData, int nLen);
    long GetKey(const char* lpszKey);
private:
    char  m_szKey[100];
    long  m_lKey;
};

}// namespace BackCom
#endif //#ifndef _ENCRYPacket_H__


