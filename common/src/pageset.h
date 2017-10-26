

#ifndef CPAGEDMEMPOOL_H
#define CPAGEDMEMPOOL_H

#include <set>
#include <map>
#include <vector>
#include <typeinfo>
using namespace std;
#include <sys/time.h>

#include "lock.h"
#include "log.h"

namespace BackCom
{
class CBlock
{
public:
    bool m_blnStatic;
    int m_intPageCount;
    int m_intPageSize;    
private:
    CLock m_lock;
    set<char*> m_setUsePage;
    set<char*> m_setFreePage;
    char * m_pBuf;
public:
    CBlock(const bool blnStatic = false, const int intPageSize = 4*1024, const int intPageCount = 8*1024);
    ~CBlock();
    bool IsFree();
    bool GetPage(vector<char*> & vectPages,const int intCount);
    bool FreePages(char* pPage);
    char * GetBuf();
};

class CPageSet
{
public:
    class _Page
    {
    public:
        ~_Page();
        bool AppendData(const char * pchBuf,const int intSize);
        bool ReadData(char * pchBuf,int & intSize);
        void ClearData();
        int  GetPageCount()const;
        int  GetPageSize()const ;
        int  GetContentLength()const ;
        const char *const*  GetPageBasePtrs()const ;
    private:
        _Page(CPageSet & pool,const int intMinPageCount);
    private:
        CPageSet & m_pool;
        vector<char*>   m_vectPages;
        int             m_intContentLength;
        int             m_intMinPageCount;
        //
        friend class CPageSet;
    };

public:
    CPageSet(const int intPageSize = 4*1024, const int intBlockPageCount = 8*1024, const int intBlockCount = 8);
    ~CPageSet();
    int  GetPageSize() const;
    _Page * CreatePagedBuf(const int intMinPageCount = 0);
    static int GetUnDeletePageCount()
    {
        CAutoLock lock(s_lock);
        return s_intUnDeletePage;
    }    
private:
    void AllocPages(vector<char*> & vectPages,const int intCount);
    void FreePages(const vector<char*> & vectPages);
    bool Log(CLog::Level enuLevel, const char * pFunc, const char *pFormat,...);

    static CLock    s_lock;
    static int      s_intUnDeletePage;    
    int             m_intPageSize;
    int             m_intBlockCount;
    int             m_intBlockPageCount;
    CLock           m_lock;
    typedef map<char *, CBlock*>::iterator map_iter;
    map<char *, CBlock*>    m_mapBlocks;
};
}// namespace BackCom

#endif
