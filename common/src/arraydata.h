#pragma once
#include "QtTypeDef.h"
#pragma pack(1)
template<typename Tp>
class CArrayData
{
public:
    Tp *        m_pData;
    INT32       m_intSize;
    CArrayData()
        :m_intSize(0)
        ,m_pData(NULL)
    {
        
    }

    ~CArrayData()
    {
        Clear();
    }

    void Resize(const int intSize)
    {
        Clear();
        if(intSize)
        {
            m_intSize = intSize;
            m_pData = new Tp[m_intSize];    
            memset(m_pData, 0, sizeof(Tp) * m_intSize);
        }
    }

    CArrayData(const CArrayData & data)
        :m_intSize(0)
        ,m_pData(NULL)
    {
        Copy(data);
    }

    BYTE GetCheckSum()const
    {
        BYTE cCheckSum = 0;
        for(int i = 0; i < m_intSize; i++)
        {
            cCheckSum += m_pData[i];
        }

        return cCheckSum;
    }

    CArrayData & operator =(const CArrayData & data)
    {
        Copy(data);
        return *this;
    }

    bool operator ==(const CArrayData & data)const
    {
        if(m_intSize != data.m_intSize)
        {
            return false;
        }

        for(int i = 0; i < m_intSize; i++)
        {
            if(m_pData[i] != data.m_pData[i])
            {
                return false;
            }
        }

        return true;
    }

    void Clear()
    {
        if(m_pData)
        {
            delete [] m_pData;
            m_pData = NULL;
        }
        
        m_intSize = 0;
    }

    void Copy(const CArrayData & data)
    {
        Resize(data.m_intSize);
        for(int i = 0; i < m_intSize; i++)
        {
            m_pData[i] = data.m_pData[i];
        }   
    }
} ;
#pragma pack()
