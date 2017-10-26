
#ifndef DATETIME_H
#define DATETIME_H
#include "comdatetime.h"
#include <string>
using namespace std;
namespace BackCom
{

struct SDateTime
{
    int m_intDate; // YYYYMMDD
    int m_intTime; // HHMMSS
    SDateTime(){m_intDate = 18991230; m_intTime = 0;}
    SDateTime(int lDate, int lTime){m_intDate = lDate; m_intTime = lTime;}
    bool operator< (const SDateTime & v) const{return (m_intDate==v.m_intDate)?(m_intTime<v.m_intTime):(m_intDate<v.m_intDate);}
    bool operator> (const SDateTime & v) const{return (m_intDate==v.m_intDate)?(m_intTime>v.m_intTime):(m_intDate>v.m_intDate);}
    bool operator<= (const SDateTime & v) const{return (m_intDate==v.m_intDate)?(m_intTime<=v.m_intTime):(m_intDate<=v.m_intDate);}
    bool operator>= (const SDateTime & v) const{return (m_intDate==v.m_intDate)?(m_intTime>=v.m_intTime):(m_intDate>=v.m_intDate);}
    bool operator==(const SDateTime & v) const{return (m_intDate==v.m_intDate)&&(m_intTime==v.m_intTime);}
    bool operator!=(const SDateTime & v) const{return (m_intDate!=v.m_intDate)||(m_intTime!=v.m_intTime);}
};

class CDateTime
{
public:
    static SDateTime FirstDayOfThisWeek(SDateTime dtToday);
    static SDateTime FirstDayOfThisMonth(SDateTime dtToday);
    static SDateTime LastDayOfThisWeek(SDateTime dtToday);
    static SDateTime LastDayOfThisMonth(SDateTime dtToday);
    static SDateTime FirstDayOfThisSeason(SDateTime dtToday);
    static SDateTime LastDayOfThisSeason(SDateTime dtToday);
    static SDateTime FirstDayOfThisHalfYear(SDateTime dtToday);
    static SDateTime LastDayOfThisHalfYear(SDateTime dtToday);
    static SDateTime FirstDayOfThisYear(SDateTime dtToday);
    static SDateTime LastDayOfThisYear(SDateTime dtToday);
    static bool IsSameMinute(SDateTime dt1, SDateTime dt2);
    static bool IsSame5Minute(SDateTime dt1, SDateTime dt2);
    static bool IsSameDay(SDateTime dt1, SDateTime dt2);
    static bool IsSameWeek(SDateTime dt1, SDateTime dt2);
    static bool IsSameMonth(SDateTime dt1, SDateTime dt2);
    static CComDateTime DateTimeCnvt(SDateTime datetime);
    static SDateTime    DateTimeCnvt(CComDateTime lddatetime);
    static char * DateTimeToStr(char * lpszDateTime, SDateTime datetime)//lpszDateTime format:"YYYY-MM-DD HH24:MI:SS"
    {
        CComDateTime lddatetime = DateTimeCnvt(datetime);
        return CComDateTime::DateTimeToStr(lpszDateTime, lddatetime);
    }

    static char * DateToStr(char * lpszDate, SDateTime datetime)//lpszDate format:"YYYY-MM-DD"
    {
        return CComDateTime::DateToStr(lpszDate, DateTimeCnvt(datetime));
    }

    static char * TimeToStr(char * lpszTime, SDateTime datetime)    //lpszTime format: "hh24:mi:ss"
    {
        return CComDateTime::TimeToStr(lpszTime, DateTimeCnvt(datetime));
    }

    static bool StringToDateTime(const string & strSrc, SDateTime & dtmDesc)
    {
        int yyyy,mm,dd,hh,mi,ss;
        ::sscanf(strSrc.c_str()
            ,"%04d-%02d-%02d %02d:%02d:%02d"
            ,&yyyy,&mm,&dd,&hh,&mi,&ss
            );

        dtmDesc.m_intDate = yyyy*10000+mm*100+dd;
        dtmDesc.m_intTime = hh*10000+mi*100+ss;
        return true;
    }
private:
    enum { SUNDAY = 1, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY};
};


}// namespace BackCom
#endif //#ifndef _DATETIME_H__

