#ifndef FORMAT_H 
#define FORMAT_H

#include <time.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include "QtTypeDef.h"

namespace BackCom {
class Format {
public:
    static const char digits[];
    static const char* zero;

    static time_t FormatTime(const char* timeStr, const char* format = "%Y-%m-%d %H:%M:%S");
    static time_t FormatTime(UINT32 Date, UINT32 Time);

    template<typename Type>
    static size_t ConvertInteger(char buf[], Type Value) {
        Type i = Value;
        char* p = buf;

        do {
            int lsd = static_cast<int>(i % 10);
            i /= 10; 
            *p++ = zero[lsd];
        } while(i != 0); 

        if(Value < 0) {
            *p++ = '-';
        }   
        *p = '\0';
        std::reverse(buf, p);

        return p - buf;
    }

    template<typename Type>
    static std::string FormatInteger(Type Value) {
        char Buf[256] = {0};
        ConvertInteger(Buf, Value);

        return Buf;
    }
};

}

#endif /*FORMAT_H*/
