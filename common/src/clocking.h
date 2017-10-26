#ifndef CLOCKING_H
#define CLOCKING_H
#include <sys/time.h>
#include <time.h>
namespace BackCom
{
class CClocking
{

private:
    timeval m_beg;
    timeval m_end;
public:
    void Start()
    {
        gettimeofday(&m_beg,NULL);
    }

    void Stop()
    {
        gettimeofday(&m_end,NULL);
    }   

    int GetTotalMicroSec()
    {
        return (m_end.tv_sec - m_beg.tv_sec)*1000000 + (m_end.tv_usec - m_beg.tv_usec);
    }
};
};
#endif
