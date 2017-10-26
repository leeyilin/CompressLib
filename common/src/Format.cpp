#include "Format.h"

using namespace BackCom;

const char Format::digits[] = "9876543210123456789";
const char* Format::zero = Format::digits + 9;

time_t Format::FormatTime(const char* timeStr, const char* format) {
    if(!timeStr) return 0;
    struct tm tm; 
    strptime(timeStr, format, &tm);

    return mktime(&tm);
}   

time_t Format::FormatTime(UINT32 Date, UINT32 Time) {
    if(Date == 0) return 0;
    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "%d-%d-%d %d:%d:%d", Date / 10000, (Date % 10000) / 100, Date % 100 
            , Time / 10000, (Time % 10000) / 100, Time % 100);

    return FormatTime(buf);
}
