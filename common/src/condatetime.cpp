#include "comdatetime.h"
#include <math.h>

namespace BackCom
{
/////////////////////////////////////////////////////////////
// CComDateTime constructor
/////////////////////////////////////////////////////////////
/*inline*/ CComDateTime::CComDateTime()
    { m_dt = 0; SetStatus(valid); }
/*inline*/ CComDateTime::CComDateTime(const CComDateTime& dateSrc)
    { m_dt = dateSrc.m_dt; m_status = dateSrc.m_status; }
/*inline*/ CComDateTime::CComDateTime(DATE dtSrc)
    { m_dt = dtSrc; SetStatus(valid); }
/*inline*/ CComDateTime::CComDateTime(time_t timeSrc) //timeSrc is the total seconds from midnight of UTC 1/1/70
    { *this = timeSrc; }
/*inline*/ CComDateTime::CComDateTime(int nYear, int nMonth, int nDay,
    int nHour, int nMin, int nSec)
    { SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSec); }
/*inline*/ const CComDateTime& CComDateTime::operator=(const CComDateTime& dateSrc)
    { m_dt = dateSrc.m_dt; m_status = dateSrc.m_status; return *this; }
/*inline*/ CComDateTime::DateTimeStatus CComDateTime::GetStatus() const
    { return m_status; }
/*inline*/ void CComDateTime::SetStatus(DateTimeStatus status)
    { m_status = status; }
/*inline*/ bool CComDateTime::operator==(const CComDateTime& date) const
    { return (m_status == date.m_status && m_dt == date.m_dt); }
/*inline*/ bool CComDateTime::operator!=(const CComDateTime& date) const
    { return (m_status != date.m_status || m_dt != date.m_dt); }
/*inline*/ const CComDateTime& CComDateTime::operator+=(
    const CComDateTimeSpan dateSpan)
    { *this = *this + dateSpan; return *this; }
/*inline*/ const CComDateTime& CComDateTime::operator-=(
    const CComDateTimeSpan dateSpan)
    { *this = *this - dateSpan; return *this; }
/*inline*/ CComDateTime::operator DATE() const
    { return m_dt; }
/*inline*/ int CComDateTime::SetDate(int nYear, int nMonth, int nDay)
    { return SetDateTime(nYear, nMonth, nDay, 0, 0, 0); }
/*inline*/ int CComDateTime::SetTime(int nHour, int nMin, int nSec)
    // Set date to zero date - 12/30/1899
    { return SetDateTime(1899, 12, 30, nHour, nMin, nSec); }
CComDateTime CComDateTime::StrToDateTime(const char * lpszDateTime) //lpszDateTime format:"YYYY-MM-DD HH24:MI:SS"
{
    CComDateTime lddatetime;
    int nYear, nMon, nDay, nHour, nMin, nSec;
    if (sscanf(lpszDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
                        &nYear, &nMon, &nDay, &nHour, &nMin, &nSec) == 6)
        lddatetime.SetDateTime(nYear, nMon, nDay, nHour, nMin, nSec);
    else
        lddatetime.SetStatus(CComDateTime::invalid);
    return lddatetime;
}

CComDateTime CComDateTime::DateStrToDateTime(const char * lpszDateTime) //Date format "YYYY-MM-DD"
{
    CComDateTime lddatetime;
    int nYear, nMon, nDay;
    if (sscanf(lpszDateTime, "%04d-%02d-%02d",
                        &nYear, &nMon, &nDay) == 3)
        lddatetime.SetDate(nYear, nMon, nDay);
    else
        lddatetime.SetStatus(CComDateTime::invalid);
    return lddatetime;
}


char * CComDateTime::DateTimeToStr(char * lpszDateTime, CComDateTime lddatetime)//lpszDateTime format:"YYYY-MM-DD HH24:MI:SS"
{
    if (lddatetime.GetStatus() == CComDateTime::valid)
    {
        sprintf(lpszDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            lddatetime.GetYear(),
            lddatetime.GetMonth(),
            lddatetime.GetDay(),
            lddatetime.GetHour(),
            lddatetime.GetMinute(),
            lddatetime.GetSecond());
        return lpszDateTime;
    }
    else
    {
        lpszDateTime[0] = '\0';
        return lpszDateTime;
    }
}

char * CComDateTime::DateToStr(char * lpszDate, CComDateTime lddatetime) //lpszDate format:"YYYY-MM-DD"
{
    if (lddatetime.GetStatus() == CComDateTime::valid)
    {
        sprintf(lpszDate, "%04d-%02d-%02d", lddatetime.GetYear(),
                lddatetime.GetMonth(), lddatetime.GetDay());
        return lpszDate;
    }
    else
    {
        lpszDate[0] = '\0';
        return lpszDate;
    }
}

char * CComDateTime::TimeToStr(char * lpszTime, CComDateTime lddatetime) //lpszTime format: "hh24:mi:ss"
{
    if (lddatetime.GetStatus() == CComDateTime::valid)
    {
        sprintf(lpszTime, "%02d:%02d:%02d",
            lddatetime.GetHour(),
        lddatetime.GetMinute(),
        lddatetime.GetSecond());
        return lpszTime;
    }
    else
    {
        lpszTime[0] = '\0';
        return lpszTime;
    }
}


CComDateTimeSpan CComDateTimeSpan::StrToTimeSpan(const char * lpszTime) //"HH24:MI:SS"
{
    CComDateTimeSpan dtSpan;
    int nHour, nMin, nSec;
    if (sscanf(lpszTime, "%02d:%02d:%02d", &nHour, &nMin, &nSec) == 3)
        dtSpan.SetDateTimeSpan(0, nHour, nMin, nSec);
    else
        dtSpan.SetStatus(CComDateTimeSpan::invalid);
    return dtSpan;
}

char * CComDateTimeSpan::TimeSpanToStr(char * lpszTime, CComDateTimeSpan dtSpan) //lpszTime format: "hh24:mi:ss"
{
    if (dtSpan.GetStatus() == CComDateTimeSpan::valid)
    {
        sprintf(lpszTime, "%02d:%02d:%02d",
         (int)dtSpan.GetHours(), (int)dtSpan.GetMinutes(), (int)dtSpan.GetSeconds());
        return lpszTime;
    }
    else
    {
        lpszTime[0] = '\0';
        return lpszTime;
    }
}


// CComDateTimeSpan
/*inline*/ CComDateTimeSpan::CComDateTimeSpan()
    { m_span = 0; SetStatus(valid); }
/*inline*/ CComDateTimeSpan::CComDateTimeSpan(double dblSpanSrc)
    { m_span = dblSpanSrc; SetStatus(valid); }
/*inline*/ CComDateTimeSpan::CComDateTimeSpan(
    const CComDateTimeSpan& dateSpanSrc)
    { m_span = dateSpanSrc.m_span; m_status = dateSpanSrc.m_status; }
CComDateTimeSpan::CComDateTimeSpan(
    long lDays, int nHours, int nMins, int nSecs)
    { SetDateTimeSpan(lDays, nHours, nMins, nSecs); }
/*inline*/ CComDateTimeSpan::DateTimeSpanStatus CComDateTimeSpan::GetStatus() const
    { return m_status; }
/*inline*/ void CComDateTimeSpan::SetStatus(DateTimeSpanStatus status)
    { m_status = status; }
/*inline*/ double CComDateTimeSpan::GetTotalDays() const
    {return m_span; }
/*inline*/ double CComDateTimeSpan::GetTotalHours() const
    {long lReturns = (long)(m_span * 24 + AFX_OLE_DATETIME_HALFSECOND);
        return lReturns;
    }
/*inline*/ double CComDateTimeSpan::GetTotalMinutes() const
    {long lReturns = (long)(m_span * 24 * 60 + AFX_OLE_DATETIME_HALFSECOND);
        return lReturns;
    }
/*inline*/ double CComDateTimeSpan::GetTotalSeconds() const
    { long lReturns = (long)(m_span * 24 * 60 * 60 + AFX_OLE_DATETIME_HALFSECOND);
        return lReturns;
    }

/*inline*/ long CComDateTimeSpan::GetDays() const
    { return (long)m_span; }
/*inline*/ bool CComDateTimeSpan::operator==(
    const CComDateTimeSpan& dateSpan) const
    { return (m_status == dateSpan.m_status &&
        m_span == dateSpan.m_span); }
/*inline*/ bool CComDateTimeSpan::operator!=(
    const CComDateTimeSpan& dateSpan) const
    { return (m_status != dateSpan.m_status ||
        m_span != dateSpan.m_span); }
/*inline*/ bool CComDateTimeSpan::operator<(
    const CComDateTimeSpan& dateSpan) const
    { return m_span < dateSpan.m_span; }
/*inline*/ bool CComDateTimeSpan::operator>(
    const CComDateTimeSpan& dateSpan) const
    { return m_span > dateSpan.m_span; }
/*inline*/ bool CComDateTimeSpan::operator<=(
    const CComDateTimeSpan& dateSpan) const
    { return m_span <= dateSpan.m_span; }
/*inline*/ bool CComDateTimeSpan::operator>=(
    const CComDateTimeSpan& dateSpan) const
    { return m_span >= dateSpan.m_span; }
/*inline*/ const CComDateTimeSpan& CComDateTimeSpan::operator+=(
    const CComDateTimeSpan dateSpan)
    { *this = *this + dateSpan; return *this; }
/*inline*/ const CComDateTimeSpan& CComDateTimeSpan::operator-=(
    const CComDateTimeSpan dateSpan)
    { *this = *this - dateSpan; return *this; }
/*inline*/ CComDateTimeSpan CComDateTimeSpan::operator-() const
    { return -this->m_span; }
/*inline*/ CComDateTimeSpan::operator double() const
    { return m_span; }

// Verifies will fail if the needed buffer size is too large
#define MAX_TIME_BUFFER_SIZE    128         // matches that in timecore.cpp
#define MIN_DATE                (-657434L)  // about year 100
#define MAX_DATE                2958465L    // about year 9999

// Half a second, expressed in days
#define HALF_SECOND  (1.0/172800.0)

// One-based array of days in year at month start
int _afxMonthDays[13] =
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

/////////////////////////////////////////////////////////////////////////////
// CComDateTime class HELPERS - implementation

bool _AfxOleDateFromTm(unsigned int wYear, unsigned int wMonth, unsigned int wDay,
    unsigned int wHour, unsigned int wMinute, unsigned int wSecond, DATE& dtDest)
{
    // Validate year and month (ignore day of week and milliseconds)
    if (wYear > 9999 || wMonth < 1 || wMonth > 12)
        return false;

    //  Check for leap year and set the number of days in the month
    bool bLeapYear = ((wYear & 3) == 0) &&
        ((wYear % 100) != 0 || (wYear % 400) == 0);

    unsigned int nDaysInMonth =
        _afxMonthDays[wMonth] - _afxMonthDays[wMonth-1] +
        ((bLeapYear && wDay == 29 && wMonth == 2) ? 1 : 0);

    // Finish validating the date
    if (wDay < 1 || wDay > nDaysInMonth ||
        wHour > 23 || wMinute > 59 ||
        wSecond > 59)
    {
        return false;
    }

    // Cache the date in days and time in fractional days
    long nDate;
    double dblTime;

    //It is a valid date; make Jan 1, 1AD be 1
    nDate = wYear*365L + wYear/4 - wYear/100 + wYear/400 +
        _afxMonthDays[wMonth-1] + wDay;

    //  If leap year and it's before March, subtract 1:
    if (wMonth <= 2 && bLeapYear)
        --nDate;

    //  Offset so that 12/30/1899 is 0
    nDate -= 693959L;

    dblTime = (((long)wHour * 3600L) +  // hrs in seconds
        ((long)wMinute * 60L) +  // mins in seconds
        ((long)wSecond)) / 86400.;

    dtDest = (double) nDate + ((nDate >= 0) ? dblTime : -dblTime);

    return true;
}

bool _AfxTmFromOleDate(DATE dtSrc, struct tm& tmDest)
{
    // The legal range does not actually span year 0 to 9999.
    if (dtSrc > MAX_DATE || dtSrc < MIN_DATE) // about year 100 to about 9999
        return false;

    long nDays;             // Number of days since Dec. 30, 1899
    long nDaysAbsolute;     // Number of days since 1/1/0
    long nSecsInDay;        // Time in seconds since midnight
    long nMinutesInDay;     // Minutes in day

    long n400Years;         // Number of 400 year increments since 1/1/0
    long n400Century;       // Century within 400 year block (0,1,2 or 3)
    long n4Years;           // Number of 4 year increments since 1/1/0
    long n4Day;             // Day within 4 year block
                            //  (0 is 1/1/yr1, 1460 is 12/31/yr4)
    long n4Yr;              // Year within 4 year block (0,1,2 or 3)
    bool bLeap4 = true;     // true if 4 year block includes leap year

    double dblDate = dtSrc; // tempory serial date

    // If a valid date, then this conversion should not overflow
    nDays = (long)dblDate;

    // Round to the second
    dblDate += ((dtSrc > 0.0) ? HALF_SECOND : -HALF_SECOND);

    nDaysAbsolute = (long)dblDate + 693959L; // Add days from 1/1/0 to 12/30/1899

    dblDate = fabs(dblDate);
    nSecsInDay = (long)((dblDate - floor(dblDate)) * 86400.);

    // Calculate the day of week (sun=1, mon=2...)
    //   -1 because 1/1/0 is Sat.  +1 because we want 1-based
    tmDest.tm_wday = (int)((nDaysAbsolute - 1) % 7L) + 1;

    // Leap years every 4 yrs except centuries not multiples of 400.
    n400Years = (long)(nDaysAbsolute / 146097L);

    // Set nDaysAbsolute to day within 400-year block
    nDaysAbsolute %= 146097L;

    // -1 because first century has extra day
    n400Century = (long)((nDaysAbsolute - 1) / 36524L);

    // Non-leap century
    if (n400Century != 0)
    {
        // Set nDaysAbsolute to day within century
        nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;

        // +1 because 1st 4 year increment has 1460 days
        n4Years = (long)((nDaysAbsolute + 1) / 1461L);

        if (n4Years != 0)
            n4Day = (long)((nDaysAbsolute + 1) % 1461L);
        else
        {
            bLeap4 = false;
            n4Day = (long)nDaysAbsolute;
        }
    }
    else
    {
        // Leap century - not special case!
        n4Years = (long)(nDaysAbsolute / 1461L);
        n4Day = (long)(nDaysAbsolute % 1461L);
    }

    if (bLeap4)
    {
        // -1 because first year has 366 days
        n4Yr = (n4Day - 1) / 365;

        if (n4Yr != 0)
            n4Day = (n4Day - 1) % 365;
    }
    else
    {
        n4Yr = n4Day / 365;
        n4Day %= 365;
    }

    // n4Day is now 0-based day of year. Save 1-based day of year, year number
    tmDest.tm_yday = (int)n4Day + 1;
    tmDest.tm_year = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;

    // Handle leap year: before, on, and after Feb. 29.
    if (n4Yr == 0 && bLeap4)
    {
        // Leap Year
        if (n4Day == 59)
        {
            /* Feb. 29 */
            tmDest.tm_mon = 2;
            tmDest.tm_mday = 29;
            goto DoTime;
        }

        // Pretend it's not a leap year for month/day comp.
        if (n4Day >= 60)
            --n4Day;
    }

    // Make n4DaY a 1-based day of non-leap year and compute
    //  month/day for everything but Feb. 29.
    ++n4Day;

    // Month number always >= n/32, so save some loop time */
    for (tmDest.tm_mon = (n4Day >> 5) + 1;
        n4Day > _afxMonthDays[tmDest.tm_mon]; tmDest.tm_mon++);

    tmDest.tm_mday = (int)(n4Day - _afxMonthDays[tmDest.tm_mon-1]);

DoTime:
    if (nSecsInDay == 0)
        tmDest.tm_hour = tmDest.tm_min = tmDest.tm_sec = 0;
    else
    {
        tmDest.tm_sec = (int)nSecsInDay % 60L;
        nMinutesInDay = nSecsInDay / 60L;
        tmDest.tm_min = (int)nMinutesInDay % 60;
        tmDest.tm_hour = (int)nMinutesInDay / 60;
    }

    return true;
}

void _AfxTmConvertToStandardFormat(struct tm& tmSrc)
{
    // Convert afx internal tm to format expected by runtimes (_tcsftime, etc)
    tmSrc.tm_year -= 1900;  // year is based on 1900
    tmSrc.tm_mon -= 1;      // month of year is 0-based
    tmSrc.tm_wday -= 1;     // day of week is 0-based
    tmSrc.tm_yday -= 1;     // day of year is 0-based
}

double _AfxDoubleFromDate(DATE dt)
{
    // No problem if positive
    if (dt >= 0)
        return dt;

    // If negative, must convert since negative dates not continuous
    // (examples: -1.25 to -.75, -1.50 to -.50, -1.75 to -.25)
    double temp = ceil(dt);
    return temp - (dt - temp);
}

DATE _AfxDateFromDouble(double dbl)
{
    // No problem if positive
    if (dbl >= 0)
        return dbl;

    // If negative, must convert since negative dates not continuous
    // (examples: -.75 to -1.25, -.50 to -1.50, -.25 to -1.75)
    double temp = floor(dbl); // dbl is now whole part
    return temp + (temp - dbl);
}

/////////////////////////////////////////////////////////////////////////////
// CComDateTime class

CComDateTime CComDateTime::GetCurrentTime(int timezone)
{
    return CComDateTime(::time(NULL)) + CComDateTimeSpan(timezone / 24.0);
}

struct tm CComDateTime::GetTm() const
{
    struct tm res;

    if (GetStatus() != valid || !_AfxTmFromOleDate(m_dt, res)) {
        res.tm_year = AFX_OLE_DATETIME_ERROR;
        res.tm_mon = AFX_OLE_DATETIME_ERROR;
        res.tm_mday = AFX_OLE_DATETIME_ERROR;
        res.tm_hour = AFX_OLE_DATETIME_ERROR;
        res.tm_min = AFX_OLE_DATETIME_ERROR;
        res.tm_sec = AFX_OLE_DATETIME_ERROR;
    }

    return res;
}

int CComDateTime::GetYear() const
{
    struct tm tmTemp;

    if (GetStatus() == valid && _AfxTmFromOleDate(m_dt, tmTemp))
        return tmTemp.tm_year;
    else
        return AFX_OLE_DATETIME_ERROR;
}

int CComDateTime::GetMonth() const
{
    struct tm tmTemp;

    if (GetStatus() == valid && _AfxTmFromOleDate(m_dt, tmTemp))
        return tmTemp.tm_mon;
    else
        return AFX_OLE_DATETIME_ERROR;
}

int CComDateTime::GetDay() const
{
    struct tm tmTemp;

    if (GetStatus() == valid && _AfxTmFromOleDate(m_dt, tmTemp))
        return tmTemp.tm_mday;
    else
        return AFX_OLE_DATETIME_ERROR;
}

int CComDateTime::GetHour() const
{
    struct tm tmTemp;

    if (GetStatus() == valid && _AfxTmFromOleDate(m_dt, tmTemp))
        return tmTemp.tm_hour;
    else
        return AFX_OLE_DATETIME_ERROR;
}

int CComDateTime::GetMinute() const
{
    struct tm tmTemp;

    if (GetStatus() == valid && _AfxTmFromOleDate(m_dt, tmTemp))
        return tmTemp.tm_min;
    else
        return AFX_OLE_DATETIME_ERROR;
}

int CComDateTime::GetSecond() const
{
    struct tm tmTemp;

    if (GetStatus() == valid && _AfxTmFromOleDate(m_dt, tmTemp))
        return tmTemp.tm_sec;
    else
        return AFX_OLE_DATETIME_ERROR;
}

int CComDateTime::GetDayOfWeek() const
{
    struct tm tmTemp;

    if (GetStatus() == valid && _AfxTmFromOleDate(m_dt, tmTemp))
        return tmTemp.tm_wday;
    else
        return AFX_OLE_DATETIME_ERROR;
}

int CComDateTime::GetDayOfYear() const
{
    struct tm tmTemp;

    if (GetStatus() == valid && _AfxTmFromOleDate(m_dt, tmTemp))
        return tmTemp.tm_yday;
    else
        return AFX_OLE_DATETIME_ERROR;
}


const CComDateTime& CComDateTime::operator=(DATE dtSrc)
{
    m_dt = dtSrc;
    SetStatus(valid);

    return *this;
}

const CComDateTime& CComDateTime::operator=(const time_t& timeSrc)//timeSrc is the total seconds from midnight of UTC 1/1/70
{
    SetDateTime(1970,1,1,0,0,0);
    *this += CComDateTimeSpan(timeSrc / 86400.00);

    return *this;
}

bool CComDateTime::operator<(const CComDateTime& date) const
{
    // Handle negative dates
    return _AfxDoubleFromDate(m_dt) < _AfxDoubleFromDate(date.m_dt);
}

bool CComDateTime::operator>(const CComDateTime& date) const
{
    // Handle negative dates
    return _AfxDoubleFromDate(m_dt) > _AfxDoubleFromDate(date.m_dt);
}

bool CComDateTime::operator<=(const CComDateTime& date) const
{
    // Handle negative dates
    return _AfxDoubleFromDate(m_dt) <= _AfxDoubleFromDate(date.m_dt);
}

bool CComDateTime::operator>=(const CComDateTime& date) const
{
    // Handle negative dates
    return _AfxDoubleFromDate(m_dt) >= _AfxDoubleFromDate(date.m_dt);
}

CComDateTime CComDateTime::operator+(const CComDateTimeSpan& dateSpan) const
{
    CComDateTime dateResult;    // Initializes m_status to valid

    // If either operand NULL, result NULL
    if (GetStatus() == null || dateSpan.GetStatus() == CComDateTimeSpan::null)
    {
        dateResult.SetStatus(null);
        return dateResult;
    }

    // If either operand invalid, result invalid
    if (GetStatus() == invalid || dateSpan.GetStatus() == CComDateTimeSpan::invalid)
    {
        dateResult.SetStatus(invalid);
        return dateResult;
    }

    // Compute the actual date difference by adding underlying dates
    dateResult = _AfxDateFromDouble(_AfxDoubleFromDate(m_dt) + dateSpan.m_span);

    // Validate within range
    dateResult.CheckRange();

    return dateResult;
}

CComDateTime CComDateTime::operator-(const CComDateTimeSpan& dateSpan) const
{
    CComDateTime dateResult;    // Initializes m_status to valid

    // If either operand NULL, result NULL
    if (GetStatus() == null || dateSpan.GetStatus() == CComDateTimeSpan::null)
    {
        dateResult.SetStatus(null);
        return dateResult;
    }

    // If either operand invalid, result invalid
    if (GetStatus() == invalid || dateSpan.GetStatus() == CComDateTimeSpan::invalid)
    {
        dateResult.SetStatus(invalid);
        return dateResult;
    }

    // Compute the actual date difference by subtracting underlying dates
    dateResult = _AfxDateFromDouble(_AfxDoubleFromDate(m_dt) - dateSpan.m_span);

    // Validate within range
    dateResult.CheckRange();

    return dateResult;
}

CComDateTimeSpan CComDateTime::operator-(const CComDateTime& date) const
{
    CComDateTimeSpan spanResult;

    // If either operand NULL, result NULL
    if (GetStatus() == null || date.GetStatus() == null)
    {
        spanResult.SetStatus(CComDateTimeSpan::null);
        return spanResult;
    }

    // If either operand invalid, result invalid
    if (GetStatus() == invalid || date.GetStatus() == invalid)
    {
        spanResult.SetStatus(CComDateTimeSpan::invalid);
        return spanResult;
    }

    // Return result (span can't be invalid, so don't check range)
    return _AfxDoubleFromDate(m_dt) - _AfxDoubleFromDate(date.m_dt);
}

int CComDateTime::SetDateTime(int nYear, int nMonth, int nDay,
    int nHour, int nMin, int nSec)
{
    return m_status = _AfxOleDateFromTm((unsigned int)nYear, (unsigned int)nMonth,
        (unsigned int)nDay, (unsigned int)nHour, (unsigned int)nMin, (unsigned int)nSec, m_dt) ?
        valid : invalid;
}

void CComDateTime::CheckRange()
{
    if (m_dt > MAX_DATE || m_dt < MIN_DATE) // about year 100 to about 9999
        SetStatus(invalid);
}

/////////////////////////////////////////////////////////////////////////////
// CComDateTimeSpan class helpers

#define MAX_DAYS_IN_SPAN    3615897L

/////////////////////////////////////////////////////////////////////////////
// CComDateTimeSpan class
long CComDateTimeSpan::GetHours() const
{
    double dblTemp;

    // Truncate days and scale up
    dblTemp = modf(m_span, &dblTemp);

    long lReturns = (long)((dblTemp + AFX_OLE_DATETIME_HALFSECOND) * 24);
    if (lReturns >= 24)
        lReturns -= 24;

    return lReturns;
}

long CComDateTimeSpan::GetMinutes() const
{
    double dblTemp;

    // Truncate hours and scale up
    dblTemp = modf(m_span * 24, &dblTemp);

    long lReturns = (long) ((dblTemp + AFX_OLE_DATETIME_HALFSECOND) * 60);
    if (lReturns >= 60)
        lReturns -= 60;

    return lReturns;
}

long CComDateTimeSpan::GetSeconds() const
{
    double dblTemp;

    // Truncate minutes and scale up
    dblTemp = modf(m_span * 24 * 60, &dblTemp);

    long lReturns = (long) ((dblTemp + AFX_OLE_DATETIME_HALFSECOND) * 60);
    if (lReturns >= 60)
        lReturns -= 60;

    return lReturns;
}

const CComDateTimeSpan& CComDateTimeSpan::operator=(double dblSpanSrc)
{
    m_span = dblSpanSrc;
    SetStatus(valid);
    return *this;
}

const CComDateTimeSpan& CComDateTimeSpan::operator=(const CComDateTimeSpan& dateSpanSrc)
{
    m_span = dateSpanSrc.m_span;
    m_status = dateSpanSrc.m_status;
    return *this;
}

CComDateTimeSpan CComDateTimeSpan::operator+(const CComDateTimeSpan& dateSpan) const
{
    CComDateTimeSpan dateSpanTemp;

    // If either operand Null, result Null
    if (GetStatus() == null || dateSpan.GetStatus() == null)
    {
        dateSpanTemp.SetStatus(null);
        return dateSpanTemp;
    }

    // If either operand Invalid, result Invalid
    if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
    {
        dateSpanTemp.SetStatus(invalid);
        return dateSpanTemp;
    }

    // Add spans and validate within legal range
    dateSpanTemp.m_span = m_span + dateSpan.m_span;
    dateSpanTemp.CheckRange();

    return dateSpanTemp;
}

CComDateTimeSpan CComDateTimeSpan::operator-(const CComDateTimeSpan& dateSpan) const
{
    CComDateTimeSpan dateSpanTemp;

    // If either operand Null, result Null

    if (GetStatus() == null || dateSpan.GetStatus() == null)
    {
        dateSpanTemp.SetStatus(null);
        return dateSpanTemp;
    }

    // If either operand Invalid, result Invalid
    if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
    {
        dateSpanTemp.SetStatus(invalid);
        return dateSpanTemp;
    }

    // Subtract spans and validate within legal range
    dateSpanTemp.m_span = m_span - dateSpan.m_span;
    dateSpanTemp.CheckRange();

    return dateSpanTemp;
}

void CComDateTimeSpan::SetDateTimeSpan(
    long lDays, int nHours, int nMins, int nSecs)
{
    // Set date span by breaking into fractional days (all input ranges valid)
    m_span = lDays + ((double)nHours)/24 + ((double)nMins)/(24*60) +
        ((double)nSecs)/(24*60*60);

    SetStatus(valid);
}

void CComDateTimeSpan::CheckRange()
{
    if(m_span < -MAX_DAYS_IN_SPAN || m_span > MAX_DAYS_IN_SPAN)
        SetStatus(invalid);
}
}// namespace BackCom
