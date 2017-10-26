
#include "comdatetime.h"
#include "DateTime.h"

namespace BackCom
{

bool CDateTime::IsSameMinute(SDateTime dt1, SDateTime dt2)
{
    return (dt1.m_intDate == dt2.m_intDate &&
        dt1.m_intTime/100 == dt2.m_intTime/100);
}

bool CDateTime::IsSame5Minute(SDateTime dt1, SDateTime dt2)
{
    return (dt1.m_intDate == dt2.m_intDate &&
        dt1.m_intTime/10000 == dt2.m_intTime/10000 &&
        dt1.m_intTime/100%100/5 == dt2.m_intTime/100%100/5);
}

bool CDateTime::IsSameDay(SDateTime dt1, SDateTime dt2)
{
    return (dt1.m_intDate == dt2.m_intDate);
}

bool CDateTime::IsSameWeek(SDateTime dt1, SDateTime dt2)
{
    return IsSameDay(FirstDayOfThisWeek(dt1),
        FirstDayOfThisWeek(dt2));
}

bool CDateTime::IsSameMonth(SDateTime dt1, SDateTime dt2)
{
    return (dt1.m_intDate/100 == dt2.m_intDate/100);
}

SDateTime CDateTime::FirstDayOfThisWeek(SDateTime dtToday)
{
    //��dtToday���ĵ��(ʱ�0:0:0)
    CComDateTime dt = DateTimeCnvt(dtToday);
    CComDateTimeSpan dtspan;
    int nDayOfWeek;
    int nYear, nMon, nDay;
    nDayOfWeek = DateTimeCnvt(dtToday).GetDayOfWeek();
    dtspan.SetDateTimeSpan(nDayOfWeek - SUNDAY, 0, 0, 0);
    dt -= dtspan;
    nYear = dt.GetYear();
    nMon = dt.GetMonth();
    nDay = dt.GetDay();
    dt.SetDate(nYear, nMon, nDay);
    return DateTimeCnvt(dt);
}

SDateTime CDateTime::LastDayOfThisWeek(SDateTime dtToday)
{
    CComDateTimeSpan dtSpan(6, 0, 0, 0);
    return (DateTimeCnvt(
        DateTimeCnvt(FirstDayOfThisWeek(dtToday)) + dtSpan));
}

SDateTime CDateTime::FirstDayOfThisMonth(SDateTime dtToday)
{
    CComDateTime dt = DateTimeCnvt(dtToday);
    dt.SetDate(dt.GetYear(), dt.GetMonth(), 1);
    return DateTimeCnvt(dt);
}

SDateTime CDateTime::LastDayOfThisMonth(SDateTime dtToday)
{
    CComDateTime dt = DateTimeCnvt(dtToday);
    int nYear = dt.GetYear();
    int nMon = dt.GetMonth();
    if (nMon == 12)
    {
        nYear += 1;
        nMon = 1;
    }
    else
    {
        nMon++;
    }
    dt.SetDate(nYear, nMon, 1);
    CComDateTimeSpan dtSpan(1, 0, 0, 0);
    return DateTimeCnvt(dt - dtSpan);
}

SDateTime CDateTime::FirstDayOfThisSeason(SDateTime dtToday)
{
    CComDateTime dt = DateTimeCnvt(dtToday);
    int nYear = dt.GetYear();
    int nMon = dt.GetMonth();
    if(nMon >= 1 && nMon < 4)
        dt.SetDate(nYear, 1, 1);
    else if(nMon >=4 && nMon < 7)
        dt.SetDate(nYear, 4, 1);
    else if(nMon >=7 && nMon < 10)
        dt.SetDate(nYear, 7, 1);
    else
        dt.SetDate(nYear, 10, 1);
    
    return DateTimeCnvt(dt);
}

SDateTime CDateTime::LastDayOfThisSeason(SDateTime dtToday)
{
    CComDateTime dt = DateTimeCnvt(dtToday);
    int nYear = dt.GetYear();
    int nMon = dt.GetMonth();
    if(nMon >= 1 && nMon < 4)
        dt.SetDate(nYear, 3, 31);
    else if(nMon >=4 && nMon < 7)
        dt.SetDate(nYear, 6, 30);
    else if(nMon >=7 && nMon < 10)
        dt.SetDate(nYear, 9, 30);
    else
        dt.SetDate(nYear, 12, 31);
    
    return DateTimeCnvt(dt);
}

SDateTime CDateTime::FirstDayOfThisHalfYear(SDateTime dtToday)
{
    CComDateTime dt = DateTimeCnvt(dtToday);
    int nYear = dt.GetYear();
    int nMon = dt.GetMonth();
    if(nMon >= 1 && nMon < 7)
        dt.SetDate(nYear, 1, 1);
    else
        dt.SetDate(nYear, 7, 1);
    
    return DateTimeCnvt(dt);
}


SDateTime CDateTime::LastDayOfThisHalfYear(SDateTime dtToday)
{
    CComDateTime dt = DateTimeCnvt(dtToday);
    int nYear = dt.GetYear();
    int nMon = dt.GetMonth();
    if(nMon < 7)
        dt.SetDate(nYear, 6, 30);
    else
        dt.SetDate(nYear, 12, 31);
    return DateTimeCnvt(dt);
}

SDateTime CDateTime::FirstDayOfThisYear(SDateTime dtToday)
{
    CComDateTime dt = DateTimeCnvt(dtToday);
    int nYear = dt.GetYear();
    dt.SetDate(nYear, 1, 1);
    return DateTimeCnvt(dt);
}

SDateTime CDateTime::LastDayOfThisYear(SDateTime dtToday)
{
    CComDateTime dt = DateTimeCnvt(dtToday);
    int nYear = dt.GetYear();
    dt.SetDate(nYear, 12, 31);
    return DateTimeCnvt(dt);
}


CComDateTime CDateTime::DateTimeCnvt(SDateTime datetime)
{
    return CComDateTime(
        datetime.m_intDate/10000,
        datetime.m_intDate%10000/100,
        datetime.m_intDate%10000%100,
        datetime.m_intTime/10000,
        datetime.m_intTime%10000/100,
        datetime.m_intTime%10000%100);
}

SDateTime   CDateTime::DateTimeCnvt(CComDateTime lddatetime)
{
    SDateTime datetime;
    if (lddatetime.GetStatus() == CComDateTime::valid)
    {
        datetime.m_intDate = lddatetime.GetYear()*10000 + lddatetime.GetMonth()*100 + lddatetime.GetDay();
        datetime.m_intTime = lddatetime.GetHour()*10000 + lddatetime.GetMinute()*100 + lddatetime.GetSecond();
    }
    else
    {
        datetime.m_intDate = 0;
        datetime.m_intTime = 0;
    }
    return datetime;
}

}// namespace BackCom
