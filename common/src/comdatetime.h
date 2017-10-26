
#ifndef CCOMDATETIME_H
#define CCOMDATETIME_H

#include <time.h>
#include <stdio.h>
#include <string.h>
namespace BackCom
{
#define AFX_OLE_DATETIME_ERROR (-1)
#define AFX_OLE_DATETIME_HALFSECOND (1.0 / (2.0 * (60.0 * 60.0 * 24.0)))

/////////////////////////////////////////////////////////////////////////////
// CComDateTime class
typedef double DATE;
class CComDateTimeSpan;

class CComDateTime
{
// Constructors
public:
    static CComDateTime GetCurrentTime(int timezone = +8); //default timezone is CST (Beijing Zone)

    CComDateTime();
    CComDateTime(const CComDateTime& dateSrc);
    CComDateTime(DATE dtSrc);
    CComDateTime(int nYear, int nMonth, int nDay,
        int nHour, int nMin, int nSec);
protected:
    CComDateTime(time_t timeSrc);  //timeSrc is the total seconds from midnight of UTC 1/1/70

// Attributes
public:
    enum DateTimeStatus
    {
        valid = 0,
        invalid = 1,    // Invalid date (out of range, etc.)
        null = 2        // Literally has no value
    };

    DATE m_dt;
    DateTimeStatus m_status;

    void SetStatus(DateTimeStatus status);
    DateTimeStatus GetStatus() const;

    struct tm GetTm() const;
    int GetYear() const;
    int GetMonth() const;       // month of year (1 = Jan)
    int GetDay() const;         // day of month (1-31)
    int GetHour() const;        // hour in day (0-23)
    int GetMinute() const;      // minute in hour (0-59)
    int GetSecond() const;      // second in minute (0-59)
    int GetDayOfWeek() const;   // 1=Sun, 2=Mon, ..., 7=Sat
    int GetDayOfYear() const;   // days since start of year, Jan 1 = 1

// Operations
public:
    const CComDateTime& operator=(const CComDateTime& dateSrc);
    const CComDateTime& operator=(DATE dtSrc);
public:
    bool operator==(const CComDateTime& date) const;
    bool operator!=(const CComDateTime& date) const;
    bool operator<(const CComDateTime& date) const;
    bool operator>(const CComDateTime& date) const;
    bool operator<=(const CComDateTime& date) const;
    bool operator>=(const CComDateTime& date) const;

    // DateTime math
    CComDateTime operator+(const CComDateTimeSpan& dateSpan) const;
    CComDateTime operator-(const CComDateTimeSpan& dateSpan) const;
    const CComDateTime& operator+=(const CComDateTimeSpan dateSpan);
    const CComDateTime& operator-=(const CComDateTimeSpan dateSpan);

    // DateTimeSpan math
    CComDateTimeSpan operator-(const CComDateTime& date) const;

    operator DATE() const;

    int SetDateTime(int nYear, int nMonth, int nDay,
        int nHour, int nMin, int nSec);
    int SetDate(int nYear, int nMonth, int nDay);
    int SetTime(int nHour, int nMin, int nSec);

    static CComDateTime StrToDateTime(const char * lpszDateTime); //lpszDateTime format:"YYYY-MM-DD HH24:MI:SS"
    static CComDateTime DateStrToDateTime(const char * lpszDateTime); //Date format "YYYY-MM-DD"
    static char * DateTimeToStr(char * lpszDateTime, CComDateTime lddatetime);//lpszDateTime format:"YYYY-MM-DD HH24:MI:SS"
    static char * DateToStr(char * lpszDate, CComDateTime lddatetime); //lpszDate format:"YYYY-MM-DD"
    static char * TimeToStr(char * lpszTime, CComDateTime lddatetime); //lpszTime format: "hh24:mi:ss"

// Implementation
protected:
    const CComDateTime& operator=(const time_t& timeSrc); //timeSrc is the total seconds from midnight of UTC 1/1/70
    void CheckRange();
    friend class CComDateTimeSpan;
};

/////////////////////////////////////////////////////////////////////////////
// CComDateTimeSpan class
class CComDateTimeSpan
{
// Constructors
public:
    CComDateTimeSpan();

    CComDateTimeSpan(double dblSpanSrc);
    CComDateTimeSpan(const CComDateTimeSpan& dateSpanSrc);
    CComDateTimeSpan(long lDays, int nHours, int nMins, int nSecs);

// Attributes
public:
    enum DateTimeSpanStatus
    {
        valid = 0,
        invalid = 1,    // Invalid span (out of range, etc.)
        null = 2        // Literally has no value
    };

    double m_span;
    DateTimeSpanStatus m_status;

    void SetStatus(DateTimeSpanStatus status);
    DateTimeSpanStatus GetStatus() const;

    double GetTotalDays() const;    // span in days (about -3.65e6 to 3.65e6)
    double GetTotalHours() const;   // span in hours (about -8.77e7 to 8.77e6)
    double GetTotalMinutes() const; // span in minutes (about -5.26e9 to 5.26e9)
    double GetTotalSeconds() const; // span in seconds (about -3.16e11 to 3.16e11)

    long GetDays() const;       // component days in span
    long GetHours() const;      // component hours in span (-23 to 23)
    long GetMinutes() const;    // component minutes in span (-59 to 59)
    long GetSeconds() const;    // component seconds in span (-59 to 59)

// Operations
public:
    const CComDateTimeSpan& operator=(double dblSpanSrc);
    const CComDateTimeSpan& operator=(const CComDateTimeSpan& dateSpanSrc);

    bool operator==(const CComDateTimeSpan& dateSpan) const;
    bool operator!=(const CComDateTimeSpan& dateSpan) const;
    bool operator<(const CComDateTimeSpan& dateSpan) const;
    bool operator>(const CComDateTimeSpan& dateSpan) const;
    bool operator<=(const CComDateTimeSpan& dateSpan) const;
    bool operator>=(const CComDateTimeSpan& dateSpan) const;

    // DateTimeSpan math
    CComDateTimeSpan operator+(const CComDateTimeSpan& dateSpan) const;
    CComDateTimeSpan operator-(const CComDateTimeSpan& dateSpan) const;
    const CComDateTimeSpan& operator+=(const CComDateTimeSpan dateSpan);
    const CComDateTimeSpan& operator-=(const CComDateTimeSpan dateSpan);
    CComDateTimeSpan operator-() const;

    operator double() const;

    void SetDateTimeSpan(long lDays, int nHours, int nMins, int nSecs);
    static CComDateTimeSpan StrToTimeSpan(const char * lpszTime); //"HH24:MI:SS"
    static char * TimeSpanToStr(char * lpszTime, CComDateTimeSpan dtSpan); //lpszTime format: "hh24:mi:ss"

// Implementation
public:
    void CheckRange();
    friend class CComDateTime;
};


}// namespace BackCom
#endif
