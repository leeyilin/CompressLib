/* 
 * File:   CompactMemory.cpp
 * Author: liyilin
 * 
 * Created on April 17, 2017, 5:12 PM
 */
#include "compact_memory.h"

int CompactMemory::encodeMultipleUnit(CBitStream& stream, const unsigned int* ptr_data, unsigned short member_num, 
    CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptr_data_base) {
    if (!ptr_data || member_num == 0)
        return -1;
    
    if (ptr_data_base) {
        for (unsigned short i = 0; i < member_num; ++i) {
            stream.EncodeData(ptr_data[i], pBitCodes, nNumOfCodes, &ptr_data_base[i]);
        }
    } else {
        for (unsigned short i = 0; i < member_num; ++i) {
            stream.EncodeData(ptr_data[i], pBitCodes, nNumOfCodes);
        }
    }
    
    return 0;
}

int CompactMemory::encodeMultipleUnit(COutputBitStream& stream, const unsigned int* ptr_data, unsigned short member_num, 
    CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptr_data_base) {
    if (!ptr_data || member_num == 0)
        return -1;
    
    if (ptr_data_base) {
        for (unsigned short i = 0; i < member_num; ++i) {
            stream.EncodeData(ptr_data[i], pBitCodes, nNumOfCodes, &ptr_data_base[i]);
        }
    } else {
        for (unsigned short i = 0; i < member_num; ++i) {
            stream.EncodeData(ptr_data[i], pBitCodes, nNumOfCodes);
        }
    }
    
    return 0;
}

int CompactMemory::decodeMultipleUnit(CBitStream& stream, unsigned int* ptr_data, unsigned short member_num, 
                   CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptr_data_base) {
    if (member_num == 0)
        return -1;
    
    if (ptr_data_base) {
        for (unsigned short i = 0; i < member_num; ++i) {
            stream.DecodeData(ptr_data[i], pBitCodes, nNumOfCodes, &ptr_data_base[i]);
        }
    } else {
        for (unsigned short i = 0; i < member_num; ++i) {
            stream.DecodeData(ptr_data[i], pBitCodes, nNumOfCodes);
        }
    }
    
    return 0;
}

int CompactMemory::encodeMultipleUnit(CBitStream& stream, const CBigInt* ptr_data, unsigned short member_num, 
    CBitCode* pBitCodes, int nNumOfCodes, const CBigInt* ptr_data_base) {
    if (!ptr_data || member_num == 0)
        return -1;
    
    if (ptr_data_base) {
        for (unsigned short i = 0; i < member_num; ++i) {
            stream.EncodeBigInt(ptr_data[i], pBitCodes, nNumOfCodes, &ptr_data_base[i]);
        }
    } else {
        for (unsigned short i = 0; i < member_num; ++i) {
            stream.EncodeBigInt(ptr_data[i], pBitCodes, nNumOfCodes);
        }
    }
    
    return 0;
}

int CompactMemory::encodeMultipleUnit(COutputBitStream& stream, const CBigInt* ptr_data, unsigned short member_num, 
    CBitCode* pBitCodes, int nNumOfCodes, const CBigInt* ptr_data_base) {
    if (!ptr_data || member_num == 0)
        return -1;
    
    if (ptr_data_base) {
        for (unsigned short i = 0; i < member_num; ++i) {
            stream.EncodeBigInt(ptr_data[i], pBitCodes, nNumOfCodes, &ptr_data_base[i]);
        }
    } else {
        for (unsigned short i = 0; i < member_num; ++i) {
            stream.EncodeBigInt(ptr_data[i], pBitCodes, nNumOfCodes);
        }
    }
    
    return 0;
}

int CompactMemory::decodeMultipleUnit(CBitStream& stream, CBigInt* ptr_data, unsigned short member_num, CBitCode* pBitCodes, 
    int nNumOfCodes, const CBigInt* ptr_data_base) {
    if (member_num == 0)
        return -1;
    
    if (ptr_data_base) {
        for (unsigned short i = 0; i < member_num; ++i) {
            stream.DecodeBigInt(ptr_data[i], pBitCodes, nNumOfCodes, &ptr_data_base[i]);
        }
    } else {
        for (unsigned short i = 0; i < member_num; ++i) {
            stream.DecodeBigInt(ptr_data[i], pBitCodes, nNumOfCodes);
        }
    }
    
    return 0;
}

unsigned int CompactMemory::timeToSeconds(const unsigned int myTime) {
    if (myTime <= 235959) {
        unsigned int myHour = myTime / 10000;
        unsigned int myMinute = (myTime % 10000) / 100;
        unsigned int mySecond = myTime % 100;
        return myHour * 3600 + myMinute * 60 + mySecond;
    } else {
        return 0;
    }
}

unsigned int CompactMemory::secondsToTime(const unsigned int mySeconds) {
    if (mySeconds < 24 * 3600) {
        unsigned int myHour = mySeconds / 3600;
        unsigned int myMinute = (mySeconds % 3600) / 60;
        unsigned int mySecond = mySeconds % 60;
        return myHour * 10000 +  myMinute * 100 + mySecond;
    } else {
        return 0;
    }
}

unsigned int CompactMemory::shortenDate(const unsigned int myDate) {
    // We temporarily only care about the date in the 21st century.
    return myDate % 1000000;
}

unsigned int CompactMemory::restoreDate(const unsigned int myDate) {
    return 20000000 + myDate;
}

bool CompactMemory::encodeTime(COutputBitStream& stream, const unsigned int newTime, CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptrOldTime) {
    stream.WriteBool(ptrOldTime ? true : false);
    if (ptrOldTime) {
        unsigned int newTimeInSeconds = timeToSeconds(newTime);
        unsigned int oldTimeInSeconds = timeToSeconds(*ptrOldTime);
        stream.EncodeData(newTimeInSeconds, pBitCodes, nNumOfCodes, &oldTimeInSeconds);
    } else {
        unsigned int newTimeInSeconds = timeToSeconds(newTime);
        stream.EncodeData(newTimeInSeconds, pBitCodes, nNumOfCodes);
    }
    return true;
}

bool CompactMemory::decodeTime(CBitStream& stream, unsigned int& newTime, CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptrOldTime) {
    bool hasBaseData = stream.ReadDWORD(1);
    unsigned int newTimeInSeconds = 0;    
    if (hasBaseData) {
        unsigned int oldTimeInSeconds = timeToSeconds(*ptrOldTime);
        stream.DecodeData(newTimeInSeconds, pBitCodes, nNumOfCodes, &oldTimeInSeconds);
    } else {
        stream.DecodeData(newTimeInSeconds, pBitCodes, nNumOfCodes);
    }
    newTime = secondsToTime(newTimeInSeconds);
    return true;
}

bool CompactMemory::encodeDate(COutputBitStream& stream, const unsigned int newDate, CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptrOldDate) {
    stream.WriteBool(ptrOldDate ? true : false);
    if (ptrOldDate) {
        unsigned int newShortenDate = shortenDate(newDate);
        unsigned int oldShortenDate = shortenDate(*ptrOldDate);
        stream.EncodeData(newShortenDate, pBitCodes, nNumOfCodes, &oldShortenDate);
    } else {
        unsigned int newShortenDate = shortenDate(newDate);
        stream.EncodeData(newShortenDate, pBitCodes, nNumOfCodes);
    }
    return true;
}

bool CompactMemory::decodeDate(CBitStream& stream, unsigned int& newDate, CBitCode* pBitCodes, int nNumOfCodes, const unsigned int* ptrOldDate) {
    bool hasBaseData = stream.ReadDWORD(1);
    unsigned int newShortenDate = 0;    
    if (hasBaseData) {
        unsigned int oldShortenDate = shortenDate(*ptrOldDate);
        stream.DecodeData(newShortenDate, pBitCodes, nNumOfCodes, &oldShortenDate);
    } else {
        stream.DecodeData(newShortenDate, pBitCodes, nNumOfCodes);
    }
    newDate = restoreDate(newShortenDate);
    return true;
}

