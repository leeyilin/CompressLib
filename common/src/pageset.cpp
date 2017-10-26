
#include "pageset.h"

using namespace BackCom;
int CPageSet::s_intUnDeletePage = 0;
CLock CPageSet::s_lock;

CBlock::CBlock(const bool blnStatic, const int intPageSize, const int intPageCount)
:m_blnStatic(blnStatic)
,m_intPageSize(intPageSize)
,m_intPageCount(intPageCount)
{
    m_pBuf = new char[m_intPageSize * m_intPageCount];  
    for(int i = 0; i < m_intPageCount; i++)
    {
        m_setFreePage.insert(m_pBuf + i * m_intPageSize);
    }
}
CBlock::~CBlock()
{
    CAutoLock lock(m_lock);
    m_setFreePage.clear();
    m_setUsePage.clear();
    if(m_pBuf)
    {
        delete [] m_pBuf;
    }
}
bool CBlock::IsFree()
{
    CAutoLock lock(m_lock);
    return m_setUsePage.size() == 0;
}
bool CBlock::GetPage(vector<char*> & vectPages,const int intCount)
{
    CAutoLock lock(m_lock);
    if(intCount > m_setFreePage.size())
    {
        return false;
    }
    else
    {
        while(vectPages.size() < intCount)
        {
            char * pBuf = *m_setFreePage.begin();
            vectPages.push_back(pBuf);
            m_setFreePage.erase(m_setFreePage.begin());
            m_setUsePage.insert(pBuf);
        }
        return true;
    }
}

bool CBlock::FreePages(char* pPage)
{
    CAutoLock lock(m_lock);
    if(m_setUsePage.find(pPage) == m_setUsePage.end())
    {
        return false;
    }
    else
    {
        m_setUsePage.erase(pPage);
        m_setFreePage.insert(pPage);
        return true;
    }
}

char * CBlock::GetBuf()
{
    CAutoLock lock(m_lock);
    return m_pBuf;
}

CPageSet::_Page::~_Page()
{
   m_pool.FreePages(m_vectPages);
}

bool CPageSet::_Page::AppendData(const char * pchBuf
              ,const int intSize
              )
{
   const int _const_page_size = m_pool.GetPageSize();
   int intNeedPageNum = (m_intContentLength+intSize+_const_page_size-1)/_const_page_size;
   if( intNeedPageNum > (int)m_vectPages.size() )
   {
       vector<char*>   vectNewPages;
       m_pool.AllocPages(vectNewPages,intNeedPageNum - m_vectPages.size());
       if( vectNewPages.empty() )
       {
            if( CLog::s_pLog != NULL )
            {
                CLog::s_pLog->Log(CLog::Error,"CPageSet::_Page", __func__, "AllocPages failed{needPages=%d}", intNeedPageNum - m_vectPages.size());
            }
            return false;
       }
       m_vectPages.insert(m_vectPages.end(),vectNewPages.begin(),vectNewPages.end());
   }
   //
   char **pPagePtrs = &*m_vectPages.begin()+m_intContentLength/_const_page_size;
   int posSrc = 0;
   if( (m_intContentLength%_const_page_size) != 0 )
   {
       int intNeedCpoy = _const_page_size-(m_intContentLength%_const_page_size);
       if( intNeedCpoy > intSize )
       {
           intNeedCpoy = intSize;
       }
       ::memcpy(*pPagePtrs+(m_intContentLength%_const_page_size),pchBuf+posSrc,intNeedCpoy);
       pPagePtrs++;
       posSrc += intNeedCpoy;
   }
   for(;posSrc < intSize ; )
   {
       int intNeedCpoy = intSize - posSrc;
       if( intNeedCpoy > _const_page_size )
       {
           intNeedCpoy = _const_page_size;
       }
       ::memcpy(*pPagePtrs,pchBuf+posSrc,intNeedCpoy);
       pPagePtrs++;
       posSrc += intNeedCpoy;
   }
   m_intContentLength += intSize;
   return true;
}

bool CPageSet::_Page::ReadData(char * pchBuf
            ,int & intSize
            )
{
   if( intSize < m_intContentLength )
   {
       return false;
   }
   //
   const int intPageSize = m_pool.GetPageSize();
   char * pDest = pchBuf;
   char **pPagePtrs = &*m_vectPages.begin();
   int i = 0 ;
   for(; i < (m_intContentLength/intPageSize) ; ++i , pDest += intPageSize )
   {
       memcpy(pDest,pPagePtrs[i],intPageSize);
   }
   if( (m_intContentLength%intPageSize) != 0 )
   {
       memcpy(pDest,pPagePtrs[i],m_intContentLength%intPageSize);
   }
   intSize = m_intContentLength;
   return true;
}

void CPageSet::_Page::ClearData()
{
   m_intContentLength = 0;
}

int  CPageSet::_Page::GetPageCount()const{   return m_vectPages.size();      };

int  CPageSet::_Page::GetPageSize()const {   return m_pool.GetPageSize();  };

int  CPageSet::_Page::GetContentLength()const {   return m_intContentLength;  };

const char *const*  CPageSet::_Page::GetPageBasePtrs()const {   return &*m_vectPages.begin();  };

CPageSet::_Page::_Page(CPageSet & pool
           ,const int intMinPageCount
           )
   : m_pool(pool)
    ,m_intContentLength(0)
    ,m_intMinPageCount(intMinPageCount)
{
}

CPageSet::CPageSet(const int intPageSize, const int intBlockPageCount, const int intBlockCount)
   :m_intPageSize(intPageSize)
   ,m_intBlockCount(intBlockCount)
   ,m_intBlockPageCount(intBlockPageCount)
{
    CAutoLock lock(m_lock);
    for(int i = 0; i < m_intBlockCount; i++)
    {
        CBlock * pBlock = NULL;
        pBlock = new CBlock(true, m_intPageSize, m_intBlockPageCount);
        m_mapBlocks[pBlock->GetBuf()] = pBlock;            
    } 
}

CPageSet::~CPageSet() {
    //must ensure dctor was called in one thread
    //it is not thread safe, even if we use lock in dctor
    CAutoLock lock(m_lock);
    for(map_iter it = m_mapBlocks.begin(); it != m_mapBlocks.end(); ++it) {
        delete it->second;
    }
    m_mapBlocks.clear();
}

int  CPageSet::GetPageSize() const{  return m_intPageSize;   };

void CPageSet::AllocPages(vector<char*> & vectPages
                     ,const int intCount
                     )
{
    CAutoLock lock(m_lock);
    for(map_iter it =m_mapBlocks.begin(); it != m_mapBlocks.end(); it++)
    {
        if(it->second->GetPage(vectPages, intCount))
        {
            break;
        }
    }

    if(vectPages.size() == 0)
    {
        CBlock * pBlock = NULL;
        pBlock = new CBlock(false, m_intPageSize, m_intBlockPageCount);
        pBlock->GetPage(vectPages, intCount);
        m_mapBlocks[pBlock->GetBuf()] = pBlock;   
        Log(CLog::Status,__func__,"Create new block{block_count=%d, block_ptr=%p}", m_mapBlocks.size(), pBlock);
    } 

    {
        CAutoLock lock(s_lock);
        s_intUnDeletePage+=vectPages.size();
    }    
}

void CPageSet::FreePages(const vector<char*> & vectPages
                    )
{
    CAutoLock lock(m_lock);
    bool blnFind = false;
    for(int i = 0; i < vectPages.size(); i++)
    {
        map_iter it = m_mapBlocks.upper_bound(vectPages[i]);
        it--;
        if(it != m_mapBlocks.end())
        {
            if(it->second->FreePages(vectPages[i]) == false)
            {
                Log(CLog::Error,__func__,"it->second->FreePages(vectPages[i]) == false{page=%p, block=%p}", vectPages[i], it->second);
            }
            else
            {
                CAutoLock lock(s_lock);
                s_intUnDeletePage--;//=vectPages.size();
            }
        }
        else
        {
            Log(CLog::Error,__func__,"it == m_mapBlocks.end(){page=%p, block_count=%d}", vectPages[i], m_mapBlocks.size());
        }
    }

    if(m_mapBlocks.size() <= m_intBlockCount)
    {
        return;
    }

    for(map_iter it =m_mapBlocks.begin(); it != m_mapBlocks.end(); it++)
    {
        if(m_mapBlocks.size() <= m_intBlockCount)
        {
            break;
        }
        
        if(it->second->m_blnStatic == false && it->second->IsFree())
        {
            CBlock * pBlock = it->second;
            m_mapBlocks.erase(it);
            Log(CLog::Status,__func__,"Free block{block_count=%d, block_ptr=%p}", m_mapBlocks.size(), pBlock);
            delete pBlock;
            break;
        }
    }
}

CPageSet::_Page * CPageSet::CreatePagedBuf(const int intMinPageCount 
                                   )
{
    return new _Page(*this,intMinPageCount);
}

bool CPageSet::Log(CLog::Level enuLevel, const char * pFunc, const char *pFormat, ...) {
    bool bRet(true);
    if (CLog::s_pLog != NULL) {
        va_list arglist;
        va_start(arglist, pFormat);
        bRet = CLog::s_pLog->Log(enuLevel, "CPageSet", pFunc, pFormat, arglist);
        va_end(arglist);
    }    
    return bRet;
}

