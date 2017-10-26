#ifndef INCREPROTOCOL_H
#define INCREPROTOCOL_H
#include "pack.h"

namespace BackCom
{
struct SIncrePack
{
    bool            m_blnIsLittleOrder;           //是否是高端开头
    bool            m_blnIsIncreData;           //是否是增量数据
    bool            m_blnHaveMorePacket;        //是否有续包
    bool            m_blnHaveGap;               //是否有缺口
    int             m_intRecCount;              //记录条数
    int             m_intRecLen;                //每条记录条数
    vector<int>     m_vectIndex;                //索引
    vector<char>    m_vectData;                 //数据内容
};

template<class Tp>
struct SIncreDat
{
    bool            m_blnHaveMorePacket;        //是否有续包
    bool            m_blnHaveGap;               //是否有缺口
    int             m_intRecCount;              //记录条数
    vector<int>     m_vectIndex;                //变化的记录的索引
    const Tp *      m_pOldBuf;                  //上一个镜像，m_pOldBuf为NULL时表示非增量
    const Tp *      m_pNewBuf;                  //最新镜像
};

template<typename Tp>
struct SStructCnvt
{
    Tp operator()(const Tp & src)const
    {
        return src;
    }
};

class CIncreProtocol:public CPack
{
public:
    //vectData是输入输出参数vectData.size()为0表示本地没有前一个镜像，解包函数会将其填充，
    //vectData.size()大于0表示本地有前一个镜像，解包函数会对其进行增量运算。
    //vectData中返回的是实际数据，vectIndex返回的是变化的记录下标
    template<typename Tp>
    CPack::ErrorCode UnpackIncreData(const char * pPackBuf, const int intPackLen, vector<Tp> & vectData, vector<int> & vectIndex, bool & blnIsIncre, bool & blnHaveGap, bool & blnHaveMoreData)
    {
        const char * pTmp = pPackBuf;
        SIncrePack packet;
        Tp * pSnap = NULL;
        int intRecCount = vectData.size();
        if(intRecCount > 0)
        {
            pSnap = &*vectData.begin();
        }

        CPack::ErrorCode code = UnpackIncreData<Tp>(pTmp, intPackLen, pSnap, intRecCount, packet);
        if(code != CPack::Err_NoErr)
        {
            return code;
        }

        if(packet.m_blnIsIncreData == false)
        {
            vectData.resize(packet.m_vectData.size()/sizeof(Tp));
        }

        vectIndex = packet.m_vectIndex;
        intRecCount = vectData.size();
        blnHaveGap = packet.m_blnHaveGap;
        blnHaveMoreData = packet.m_blnHaveMorePacket;
        blnIsIncre = packet.m_blnIsIncreData;
        if(blnIsIncre == true)
        {
            if(intRecCount > 0)
            {
                pSnap = &*vectData.begin();
            }

            //将增量数据与本地镜像合并
            if(MergeSnap(packet, (char * )pSnap, vectData.size() * sizeof(Tp)) == false)
            {
                return Err_OutOfMem;
            }
        }
        else
        {
            memcpy(&*vectData.begin(), &*packet.m_vectData.begin(), packet.m_vectData.size());
        }
        return Err_NoErr;
    }

    template<typename Tp/*原始类型*/, typename Ttran/*协议类型*/, typename Tcv/*类型转换*/>
    CPack::ErrorCode PackIncreData(const SIncreDat<Tp> & increData, vector<char> & vectPacket, const Tcv & cnvt)
    {
        SIncrePack pack;
        pack.m_blnIsLittleOrder = IsLittleEndian();
        pack.m_blnIsIncreData = (increData.m_pOldBuf != NULL);
        pack.m_blnHaveGap = increData.m_blnHaveGap;
        pack.m_blnHaveMorePacket = increData.m_blnHaveMorePacket;
        pack.m_intRecLen = sizeof(Ttran);
        if(pack.m_blnIsIncreData == false)
        {
            pack.m_intRecCount = increData.m_intRecCount;
            pack.m_vectIndex = increData.m_vectIndex;
            pack.m_vectData.resize(increData.m_intRecCount * sizeof(Ttran));
            char * pTmp = &*pack.m_vectData.begin();
            for(int i = 0; i < increData.m_intRecCount; i++)
            {
                Ttran tmp = cnvt(*(increData.m_pNewBuf + i));
                memcpy(pTmp, &tmp, sizeof(Ttran));
                pTmp += sizeof(Ttran);
            }
        }
        else
        {
            pack.m_vectData.resize(pack.m_intRecLen * increData.m_vectIndex.size());
            char * pR = &*pack.m_vectData.begin();
            for(int i = 0; i < increData.m_vectIndex.size(); i++)
            {
                int intIndex = increData.m_vectIndex[i];
                const Tp * pS = increData.m_pNewBuf + intIndex;
                const Tp * pD = increData.m_pOldBuf + intIndex;
                //确认是否存在变化
                if(memcmp(pS, pD, pack.m_intRecLen) == 0)
                {
                    continue;
                }

                pack.m_vectIndex.push_back(intIndex);
                //进行异或运算
                DiffOrData((char*)&cnvt(*pS), (char*)&cnvt(*pD), pack.m_intRecLen, pR);
                pR += pack.m_intRecLen;
            }

            pack.m_intRecCount = pack.m_vectIndex.size();
            pack.m_vectData.resize(pack.m_intRecLen * pack.m_vectIndex.size());
        }

        return PackIncreData(pack, vectPacket);
    }
protected:
    template<typename Tp>
    CPack::ErrorCode UnpackIncreData(const char * pPackBuf, const int intPackLen, const Tp * pSnap, const int intRecCount, SIncrePack & packet)
    {
        int intStructTypeLen = 0; /* 记录结构体类型长度 */
        intStructTypeLen = sizeof(Tp);
        UnpackIncreData(pPackBuf,intPackLen,packet);
        int intPackRecCount = packet.m_intRecCount;
        if(IsSameEndianToLocal(packet.m_blnIsLittleOrder) == false) /* 字节顺序转换 */
        {
            Tp *pData = (Tp *)&*packet.m_vectData.begin();
            for(int i=0; i < intPackRecCount; i++)
            {
                pData[i].TransByteOrder();
            }
        }

        if(packet.m_blnIsIncreData == true) /* 增量操作 */
        {
            if(pSnap == NULL)
            {
                return Err_OutOfMem;
            }
            int i = 0;
            for( i = 0; i < (int)(packet.m_vectIndex.size()); i++)
            {
                if(packet.m_vectIndex[i] + 1> intRecCount)
                {
                    return Err_OutOfMem;
                }
            }

            for( i = 0; i < (int)(packet.m_vectIndex.size()); i++)
            {
                const char * pS = (const char *)(pSnap + packet.m_vectIndex[i]);
                const char * pD = &*packet.m_vectData.begin() + i * intStructTypeLen;
                char * pR = &*packet.m_vectData.begin() + i * intStructTypeLen;
                //进行异或运算
                DiffOrData(pS, pD, intStructTypeLen, pR);
            }
        }
        else
        {
            packet.m_vectIndex.reserve(intPackRecCount);
            for(int i = 0; i < intPackRecCount; i++)
            {
                packet.m_vectIndex.push_back(i);
            }
        }
        return Err_NoErr;
    }

    CPack::ErrorCode UnpackIncreData(const char * pPackBuf, const int intPackLen, SIncrePack & packet)
    {
        const char *pTempBuff = pPackBuf;
        unsigned int uitFlag = 0;
        pTempBuff += Input(pTempBuff,uitFlag);
        pTempBuff += Input(pTempBuff,packet.m_intRecCount);  /* 解包记录数 */
        pTempBuff += Input(pTempBuff,packet.m_intRecLen); /* 解包每条记录长度 */

        if(uitFlag & 1)
        {
           packet.m_blnIsLittleOrder = false;
        }
        else
        {
            packet.m_blnIsLittleOrder = true;
        }

        if(uitFlag & 2)
        {
            packet.m_blnIsIncreData = true;
        }
        else
        {
            packet.m_blnIsIncreData = false;
        }

        if(uitFlag & 4)
        {
            packet.m_blnHaveMorePacket = true;
        }
        else
        {
            packet.m_blnHaveMorePacket = false;
        }

        if(uitFlag & 8)
        {
            packet.m_blnHaveGap = true;
        }
        else
        {
            packet.m_blnHaveGap = false;
        }

        if( packet.m_blnIsIncreData == true ) /* 判断是否为增量 */
        {
            for(int i=0;i<packet.m_intRecCount;i++) /* 获取所有索引值 */
            {
                int intValue;
                pTempBuff += Input(pTempBuff,intValue);
                packet.m_vectIndex.push_back(intValue);
            }
        }

        packet.m_vectData.insert(packet.m_vectData.end(), pTempBuff, pPackBuf + intPackLen);
        //Reversal(&*packet.m_vectData.begin(), packet.m_intRecCount, packet.m_intRecLen);
        return Err_NoErr;
    }

    CPack::ErrorCode PackIncreData(const  SIncrePack & packet, vector<char> & vectPacket)
    {
        unsigned int uitFlag = 0;
        int intLen = sizeof(int) * 3 + packet.m_vectData.size();
        if(packet.m_blnIsLittleOrder == false)
        {
            uitFlag |= 1;
        }

        if(packet.m_blnIsIncreData == true)
        {
            uitFlag |= 2;
            intLen += packet.m_vectIndex.size() * sizeof(int);
        }

        vectPacket.resize(intLen);
        char *pTempBuff = &*vectPacket.begin();
        if(packet.m_blnHaveMorePacket == true)
        {
            uitFlag |= 4;
        }

        if(packet.m_blnHaveGap == true)
        {
            uitFlag |= 8;
        }

        pTempBuff += Output(pTempBuff,uitFlag);
        pTempBuff += Output(pTempBuff,packet.m_intRecCount);
        pTempBuff += Output(pTempBuff,packet.m_intRecLen);

        if( packet.m_blnIsIncreData == true ) /* 判断是否为增量 */
        {
            for(int i=0;i<(int)packet.m_vectIndex.size();i++) /* 获取所有索引值 */
            {
                pTempBuff += Output(pTempBuff,packet.m_vectIndex[i]);
            }
        }

        if(packet.m_vectData.size() > 0)
        {
            memcpy(pTempBuff, &*packet.m_vectData.begin(), packet.m_vectData.size());
            //Reversal(pTempBuff, packet.m_intRecLen, packet.m_intRecCount);
        }

        return Err_NoErr;
    }

    template<typename Tp1, typename Tp2>
    void VectorCnvt(const vector<Tp1> & vectSrc, vector<Tp2> & vectDesc)
    {
        vectDesc.clear();
        if(vectSrc.size() > 0)
        {
            const Tp2 * pBeg = (const Tp2 *)&*vectSrc.begin();
            const Tp2 * pEnd = pBeg + vectSrc.size()*sizeof(Tp1)/sizeof(Tp2);
            vectDesc.insert(vectDesc.begin(), pBeg, pEnd);
       }
    }

    //是否是低字节开头
    bool IsLittleEndian()
    {
        unsigned int usData = 0x12345678;
        unsigned char *pucData = (unsigned char*)&usData;
        if(*pucData == 0x78)
            return true;
        else
            return false;
    }

    //判断是否与本地字节次序相同
    bool IsSameEndianToLocal(const bool blnIsLittleOrder)
    {
        return IsLittleEndian() == blnIsLittleOrder;
    }

    //异或运算
    void DiffOrData(const char * pSnap, const char * pDiffOr, const int intLen, char * pResult)
    {
        for(int i = 0; i < intLen; i++)
        {
            pResult[i] = pSnap[i] ^ pDiffOr[i];
        }
    }

    //增量与本地镜像合并
    bool MergeSnap(const SIncrePack & packet, char * pSnap, const int intLen)
    {
        if(packet.m_blnIsIncreData == true)
        {
            const char * pData = &*packet.m_vectData.begin();
            for(int i = 0; i < (int)packet.m_vectIndex.size(); i++)
            {
                int intIndex = packet.m_vectIndex[i];
                const char * pTmpSrc = pData + i * packet.m_intRecLen;
                char * pTmpDest = pSnap + intIndex * packet.m_intRecLen;
                if(intIndex * packet.m_intRecLen > intLen)
                {
                    return false;
                }
                memcpy(pTmpDest, pTmpSrc, packet.m_intRecLen);
            }
        }
        else
        {
            if((int)packet.m_vectData.size() > intLen)
            {
                return false;
            }

            memcpy(pSnap, &*packet.m_vectData.begin(), packet.m_vectData.size());
        }
        return true;
    }

    void Reversal(char * pBuf, const int intWideSize, const int intHighSize)
    {
        vector<char> vectTmp;
        vectTmp.resize(intWideSize*intHighSize);
        char * pTmp = &*vectTmp.begin();
        for(int i = 0; i < intHighSize; i++)
        {
            for(int j = 0; j < intWideSize; j++)
            {
                pTmp[intHighSize * j + i] = pBuf[intWideSize * i + j];
            }
        }

        memcpy(pBuf, pTmp, vectTmp.size());
    }
};
};
#endif

