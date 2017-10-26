#ifndef CSTATICSTRING_H
#define CSTATICSTRING_H

#include <string.h>

namespace BackCom
{
// CStaticString
template <int __size=16>
struct CStaticString
{
    CStaticString()                {   memset(m_pch, 0, __size);       };
    CStaticString(const char *ptr) {   *this = ptr;        };
    CStaticString(const char *ptr, int len) {   *this = ptr; if (len < __size) m_pch[len] = 0;}
    //
    CStaticString & operator=(const CStaticString & v)
    {
        if( this == &v )
        {// same object,nothing need do
            return *this;
        }
        memcpy(m_pch,v.m_pch,__size);
        return *this;
    }
    CStaticString & operator=(const char * ptr)
    {
        if( m_pch == ptr )
        {// same object,nothing need do
            return *this;
        }
        if( ptr == 0 )
        {
            m_pch[0] = 0;
            return *this;
        }
        int size = strlen(ptr) ;
        if((__size-1) < size)
            size = __size-1;
        memcpy(m_pch,ptr,size);
        m_pch[size] = 0;
        return *this;
    };
    //
    const int size() const                      {   return strlen(m_pch);               };
    const char* c_str() const                   {   return m_pch;                       };
    //
    operator const char*() const                {   return m_pch;                       };
    bool operator<(const CStaticString & v) const {   return strcmp(m_pch,v.m_pch) < 0;   };
    bool operator<(const char * ptr) const      {   return strcmp(m_pch,ptr) < 0;       };
    bool operator>(const CStaticString & v) const {   return strcmp(m_pch,v.m_pch) > 0;   };
    bool operator>(const char * ptr) const      {   return strcmp(m_pch,ptr) > 0;       };
    bool operator==(const CStaticString & v) const{   return strcmp(m_pch,v.m_pch) == 0;  };
    bool operator==(const char * ptr) const     {   return strcmp(m_pch,ptr) == 0;      };
    bool operator!=(const CStaticString & v) const{   return strcmp(m_pch,v.m_pch) != 0;  };
    bool operator!=(const char * ptr) const     {   return strcmp(m_pch,ptr) != 0;      };
    CStaticString & operator+=(const char * ptr) const     {  strncat(m_pch, ptr, __size - strlen(m_pch) - 1); return *this;};
    //
    char    m_pch[__size];
};

}// namespace BackCom
#endif
