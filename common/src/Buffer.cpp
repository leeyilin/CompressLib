// Buffer.cpp: implementation of the CPackBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Buffer.h"
#include "string.h"
#include <assert.h>

UINT32  CPackBuffer::m_dwPageSize = 8192;

CPackBuffer::CPackBuffer()                      //normal constructor, set all members to their default values..
{
    m_nSize = 0;

    m_bSustainSize = false;
    m_bSingleRead = false;
    m_bNoAlloc = false;
    m_nReadPos = 0;

    m_pBase =  NULL;
    m_nDataSize = 0;

    m_nInitSize = 0;
    m_nMoreBytes = 0;
}

void CPackBuffer::Initialize(UINT nInitsize, bool bSustain, UINT nMoreBytes)
{
    m_bSustainSize = bSustain;
    m_nMoreBytes = nMoreBytes;

    ReAllocateBuffer(nInitsize);
    m_nInitSize = m_nSize;
}


CPackBuffer::~CPackBuffer()
{
    if (m_bNoAlloc)
        return;

    if(m_pBase)
        delete [] m_pBase;
}


UINT CPackBuffer::GetMemSize()
{
    return m_nSize;
}

UINT CPackBuffer::GetBufferLen()                //return the left data size in the buffer
{
    if(m_pBase == NULL)
        return 0;

    if (m_bSingleRead == true)
        return m_nDataSize-m_nReadPos;
    else
        return m_nDataSize;
}

// Is it necessary to check out nPos is legal, namely it is smaller than the buffer size?
PBYTE CPackBuffer::GetBuffer(UINT nPos)         // return the pointer pointing to the specified position in the buffer
{
    if(m_pBase == NULL)
        return NULL;

    return m_pBase + nPos;
}

//to increase the memory space of the buffer to meet the requested size and return the requested memory space
UINT CPackBuffer::ReAllocateBuffer(UINT nRequestedSize)
{
    if (m_bNoAlloc)                             //the flag m_bNoAlloc is true here, so we do not allocate any memory here
        return 0;

    if(nRequestedSize <= m_nSize)               //Causing the requested size is smaller than m_nSize, which is the current buffer size, it is not neccessary to allocate any memory.
        return 0;
// set m_nSize to a size big enough.
    UINT nNewSize = m_nSize;
    if(nNewSize < m_dwPageSize)
        nNewSize = m_dwPageSize;

    while(nRequestedSize > nNewSize)
        nNewSize *= 2;


    // New Copy Data Over
    PBYTE pNewBuffer = new BYTE[nNewSize];
    if (m_pBase)
    {
        if(m_nDataSize)
            memcpy(pNewBuffer, m_pBase, m_nDataSize);
        delete [] m_pBase;
    }

    // Hand over the pointer
    m_pBase = pNewBuffer;

    m_nSize = nNewSize;

//  TRACE("ReAllocateBuffer %d\n",m_nSize);

    return m_nSize;
}

//this function acts to reduce the memory space of the buffer reasonably and returns the current buffer size
UINT CPackBuffer::DeAllocateBuffer(UINT nRequestedSize)             //this function's name is well-defined..
{
//the following four "if" statement can combine into one big "if" statement
    if (m_bNoAlloc || nRequestedSize == 0)
        return 0;

    if(m_bSustainSize)
        return 0;

    if(m_nSize <= m_nInitSize)
        return 0;

    if(nRequestedSize < m_nDataSize)
        return 0;

    if(nRequestedSize < m_nInitSize)
        nRequestedSize = m_nInitSize;

    UINT nNewSize = m_nSize;
    while(nNewSize >= nRequestedSize * 2)       //this indicates the current buffer size is much bigger than the initial buffer size and the data size,
                                              //namely it has many idle memory space.
        nNewSize /= 2;

    if(nNewSize == m_nSize)
        return 0;


    PBYTE pNewBuffer = new BYTE[nNewSize];

    if(m_pBase)
    {
        if(m_nDataSize)
            memcpy(pNewBuffer, m_pBase, m_nDataSize);

        delete [] m_pBase;
    }

    // Hand over the pointer
    m_pBase = pNewBuffer;

    m_nSize = nNewSize;

//  TRACE("DeAllocateBuffer %d\n",m_nSize);

    return m_nSize;
}
//to expand the size of the buffer, that is , to add nSize bits at the back of the buffer
UINT CPackBuffer::Add(UINT nSize)
{
    if (nSize)
    {
        // What is the meaning of m_nMoreBytes?
        ReAllocateBuffer(nSize + m_nDataSize + m_nMoreBytes);

        // Advance Pointer
        m_nDataSize += nSize;
    }

    return nSize;
}
//to write the data in pData into the buffer
UINT CPackBuffer::Write(const void* pData, UINT nSize)
{
    if(nSize)
    {
        ReAllocateBuffer(nSize + m_nDataSize + m_nMoreBytes);

        memcpy(m_pBase+m_nDataSize, pData, nSize);

        // Advance Pointer
        m_nDataSize += nSize;
    }

    return nSize;
}

//insert the data in pData in the front of the buffer
UINT CPackBuffer::Insert(const void* pData, UINT nSize)
{
    ReAllocateBuffer(nSize + m_nDataSize + m_nMoreBytes);

    memmove(m_pBase+nSize, m_pBase, m_nDataSize);
    memcpy(m_pBase, pData, nSize);

    // Advance Pointer
    m_nDataSize += nSize;

    return nSize;
}

void CPackBuffer::Overwrite(const void* pData, UINT nSize, UINT offset)
{
    assert(nSize + offset <= m_nDataSize);

    memcpy(m_pBase + offset, pData, nSize);

    return;
}

//to read data in Read Only Mode or at the beginning of the buffer
UINT CPackBuffer::Read(void* pData, UINT nSize)
{
    // all that we have

    if (nSize)
    {
        if (m_bSingleRead)              //in Read Only Mode
        {
            if (nSize+m_nReadPos > m_nDataSize)             //to show that the call Read() is illegal because there is no enough data at the current cursor to read.
            {
                throw DATA_LACK;
                return 0;
            }

            memcpy(pData, m_pBase+m_nReadPos, nSize);
            m_nReadPos += nSize;
        }
        else                            //read data at the beginning of the buffer
        {
            if (nSize > m_nDataSize)                //Obviously this is illegal.
            {
                throw DATA_LACK;
                return 0;
            }

            m_nDataSize -= nSize;

            memcpy(pData, m_pBase, nSize);
            if (m_nDataSize > 0)
                memmove(m_pBase, m_pBase+nSize, m_nDataSize);
        }
    }

    DeAllocateBuffer(m_nDataSize + m_nMoreBytes);

    return nSize;
}

//to advance the cursor in Read Only Mode, namely skipping some data.
UINT CPackBuffer::SkipData(int nSize)
{

    if(m_bSingleRead)
    {
        if (nSize+m_nReadPos > m_nDataSize)
        {
            throw DATA_LACK;
            return 0;
        }

        m_nReadPos += nSize;

        return nSize;
    }

    return 0;
}

//to clean up the buffer and retrieve the memory space
void CPackBuffer::ClearBuffer()
{
    // Force the buffer to be empty
    m_nDataSize = 0;
    m_nReadPos = 0;

    DeAllocateBuffer(0);
}

void CPackBuffer::Copy(CPackBuffer& buffer)
{
    UINT nReSize = buffer.GetMemSize();

    if(nReSize != m_nSize)
    {
        if (m_pBase)
            delete [] m_pBase;
        m_pBase = new BYTE[nReSize];

        m_nSize = nReSize;
    }
    m_nDataSize = buffer.GetBufferLen();

    if(m_nDataSize > 0)
        memcpy(m_pBase, buffer.GetBuffer(), m_nDataSize);
}


// to delete nSize bytes at the beginning of the buffer
UINT CPackBuffer::Delete(UINT nSize)
{
    if(nSize == 0)
        return nSize;

    if (nSize > m_nDataSize)            //to make nSize reasonable
        nSize = m_nDataSize;

    m_nDataSize -= nSize;

    if(m_nDataSize > 0)
        memmove(m_pBase, m_pBase+nSize, m_nDataSize);

    DeAllocateBuffer(m_nDataSize);

    return nSize;
}

//to add another buffer at tne back of the buffer
const CPackBuffer& CPackBuffer::operator+(CPackBuffer& buff)
{
    this->Write(buff.GetBuffer(), buff.GetBufferLen());

    return* this;
}


//to delete nSize bytes at the back of the buffer
UINT CPackBuffer::DeleteEnd(UINT nSize)
{
    if(nSize > m_nDataSize)                 //to make nSize reasonable
        nSize = m_nDataSize;

    if(nSize)
    {
        m_nDataSize -= nSize;
        DeAllocateBuffer(m_nDataSize);
    }

    return nSize;
}

//to set the buffer to the specified buffer
void CPackBuffer::SetBuffer(PBYTE pData, UINT uSize)
{
    m_bNoAlloc = true;
    m_pBase = pData;
    m_nDataSize = m_nSize = uSize;
}
