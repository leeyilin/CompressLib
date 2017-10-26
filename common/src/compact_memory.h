/* 
 * File:   compactMemory.h
 * Author: liyilin
 *
 * Created on April 17, 2017, 5:12 PM
 */

#ifndef COMPACTMEMORY_H
#define	COMPACTMEMORY_H

#include "BitStream.h"

class CompactMemory {
public:
    template <typename Member>
    static int encodeMultipleUnit(CBitStream& stream, const Member* ptr_data, unsigned short member_num,
        CBitCode* pBitCodes, int nNumOfCodes, const Member* ptr_data_base = NULL);
    template <typename Member>
    static int encodeMultipleUnit(COutputBitStream& stream, const Member* ptr_data, unsigned short member_num,
        CBitCode* pBitCodes, int nNumOfCodes, const Member* ptr_data_base = NULL);
    
    template <typename Member>
    static int decodeMultipleUnit(CBitStream& stream, Member* ptr_data, unsigned short member_num,
        CBitCode* pBitCodes, int nNumOfCodes, const Member* ptr_data_base = NULL);  
    
    static int encodeMultipleUnit(CBitStream& stream, const unsigned int* ptr_data, unsigned short member_num, 
        CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptr_data_base = NULL);
    static int encodeMultipleUnit(COutputBitStream& stream, const unsigned int* ptr_data, unsigned short member_num, 
        CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptr_data_base = NULL);
    
    static int decodeMultipleUnit(CBitStream& stream, unsigned int* ptr_data, unsigned short member_num, 
        CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptr_data_base = NULL);
    
    static int encodeMultipleUnit(CBitStream& stream, const CBigInt* ptr_data, unsigned short member_num, 
        CBitCode* pBitCodes, int nNumOfCodes, const CBigInt* ptr_data_base = NULL);
    static int encodeMultipleUnit(COutputBitStream& stream, const CBigInt* ptr_data, unsigned short member_num, 
        CBitCode* pBitCodes, int nNumOfCodes, const CBigInt* ptr_data_base = NULL);
    
    static int decodeMultipleUnit(CBitStream& stream, CBigInt* ptr_data, unsigned short member_num, 
        CBitCode* pBitCodes, int nNumOfCodes, const CBigInt* ptr_data_base = NULL);
    
    static bool encodeTime(COutputBitStream& stream, const unsigned int newTime, CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptrOldTime=NULL);
    static bool decodeTime(CBitStream& stream, unsigned int& newTime, CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptrOldTime=NULL);
    static bool encodeDate(COutputBitStream& stream, const unsigned int newDate, CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptrOldDate=NULL);
    static bool decodeDate(CBitStream& stream, unsigned int& newDate, CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptrOldDate=NULL);
protected:
    static unsigned int shortenDate(const unsigned int myDate);
    static unsigned int restoreDate(const unsigned int myDate);
    static unsigned int timeToSeconds(const unsigned int myTime);
    static unsigned int secondsToTime(const unsigned int mySeconds);
};

template <typename Member>
int CompactMemory::encodeMultipleUnit(CBitStream& stream, const Member* ptr_data, unsigned short member_num,
    CBitCode* pBitCodes, int nNumOfCodes, const Member* ptr_data_base) {
    if (!ptr_data || member_num == 0)
        return -1;

    // Convert to unsigned int.
    if (ptr_data_base) {
        for (unsigned short i = 0; i < member_num; ++i) {
            unsigned int tmp = ptr_data[i];
            unsigned int tmp_base = ptr_data_base[i];
            stream.EncodeData(tmp, pBitCodes, nNumOfCodes, &tmp_base);
        }
    } else {
        for (unsigned short i = 0; i < member_num; ++i) {
            unsigned int tmp = ptr_data[i];
            stream.EncodeData(tmp, pBitCodes, nNumOfCodes);
        }
    }

    return 0;
}

template <typename Member>
int CompactMemory::decodeMultipleUnit(CBitStream& stream, Member* ptr_data, unsigned short member_num,
    CBitCode* pBitCodes, int nNumOfCodes, const Member* ptr_data_base) {
    if (member_num == 0)
        return -1;

    if (ptr_data_base) {
        for (unsigned short i = 0; i < member_num; ++i) {
            unsigned int tmp;
            unsigned int tmp_base = ptr_data_base[i];
            stream.DecodeData(tmp, pBitCodes, nNumOfCodes, &tmp_base);
            ptr_data[i] = tmp;
        }
    } else {
        for (unsigned short i = 0; i < member_num; ++i) {
            unsigned int tmp;
            stream.DecodeData(tmp, pBitCodes, nNumOfCodes);
            ptr_data[i] = tmp;
        }
    }

    return 0;
}

template <typename Member>
int CompactMemory::encodeMultipleUnit(COutputBitStream& stream, const Member* ptr_data, unsigned short member_num,
    CBitCode* pBitCodes, int nNumOfCodes, const Member* ptr_data_base) {
    if (!ptr_data || member_num == 0)
        return -1;

    // Convert to unsigned int.
    if (ptr_data_base) {
        for (unsigned short i = 0; i < member_num; ++i) {
            unsigned int tmp = ptr_data[i];
            unsigned int tmp_base = ptr_data_base[i];
            stream.EncodeData(tmp, pBitCodes, nNumOfCodes, &tmp_base);
        }
    } else {
        for (unsigned short i = 0; i < member_num; ++i) {
            unsigned int tmp = ptr_data[i];
            stream.EncodeData(tmp, pBitCodes, nNumOfCodes);
        }
    }

    return 0;    
}

#endif	/* COMPACTMEMORY_H */

