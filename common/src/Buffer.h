// Buffer.h: interface for the CPackBuffer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BITBUFFER_H
#define BITBUFFER_H
#include "QtTypeDef.h"

#define     DATA_LACK       -1
#define     DATA_ERROR      -2

class CPackBuffer
{
// Attributes
private:
    PBYTE       m_pBase;                        // the pointer pointing to the buffer
    UINT        m_nDataSize;                    // the current data size in the buffer
    UINT        m_nSize;                        // the current buffer size

    UINT        m_nInitSize;

    UINT        m_nMoreBytes;

    bool        m_bSustainSize;

    static UINT32   m_dwPageSize;

public:
    bool        m_bNoAlloc;     //a flag to indicate whether to allocate memory
    bool        m_bSingleRead;  // Read Only Mode
    UINT        m_nReadPos;     //the current position of the cursor

// Methods
protected:
    UINT DeAllocateBuffer(UINT nRequestedSize);
    UINT GetMemSize();
public:
    CPackBuffer();
    virtual ~CPackBuffer();

    void ClearBuffer();
    void Initialize(UINT nInitsize, bool bSustain, UINT nMoreBytes = 0);

    UINT Delete(UINT nSize);
    UINT Add(UINT nSize);
    UINT Read(void* pData, UINT nSize);
    UINT Write(const void* pData, UINT nSize);
    UINT Insert(const void* pData, UINT nSize);
    void Overwrite(const void* pData, UINT nSize, UINT offset);
    UINT DeleteEnd(UINT nSize);

    UINT SkipData(int nSize);

    void Copy(CPackBuffer& buffer);

    PBYTE GetBuffer(UINT nPos=0);
    UINT GetBufferLen();

    const CPackBuffer& operator+(CPackBuffer& buff);
    UINT ReAllocateBuffer(UINT nRequestedSize);

    void SetBuffer(const char* data, UINT nbytes) { return SetBuffer((PBYTE)data, nbytes); }
    void SetBuffer(PBYTE, UINT);
};

#endif
