#ifndef COMFUN_H
#define COMFUN_H
#include "log.h"
#include <sys/stat.h>
#include <limits.h>
#include <sys/types.h>
#include <typeinfo>
#include <vector>
#include <map>
#include <string>
#include<sstream>
#include <cstdlib>
#include <algorithm>
#include <cxxabi.h>
using namespace std;
using namespace BackCom;

#if __cplusplus < 201103L
namespace std
{
    inline int stoi(const string &str, std::size_t* pos = 0, int base = 10)
    {
        char *endptr = const_cast<char*>(str.c_str());
        int v = static_cast<int>(strtol(str.c_str(), &endptr, base));
        if (pos) {
            *pos = endptr - str.c_str();
        }
        return v;
    }

    inline long stol(const string &str, std::size_t* pos = 0, int base = 10)
    {
        char *endptr = const_cast<char*>(str.c_str());
        long v = strtol(str.c_str(), &endptr, base);
        if (pos) {
            *pos = endptr - str.c_str();
        }
        return v;
    }

    inline long long stoll(const string &str, std::size_t* pos = 0, int base = 10)
    {
        char *endptr = const_cast<char*>(str.c_str());
        long long v = strtoll(str.c_str(), &endptr, base);
        if (pos) {
            *pos = endptr - str.c_str();
        }
        return v;
    }
}
#endif

inline bool ToVector(vector<string> & vectRet,const char * pchToken,const char * pchDelimit)
{
    //
    vectRet.clear();
    //
    if( pchToken == NULL )
    {
        return true;
    }
    if( ::strlen(pchToken) == 0 )
    {
        return true;
    }
    //
    if( pchDelimit == NULL )
    {
        return false;
    }
    if( ::strlen(pchDelimit) == 0 )
    {
        return false;
    }
    //
    char buf[4096];
    char *pchContent = buf;
    char *pchNew = NULL;
    unsigned int intLength = ::strlen(pchToken);
    if( intLength > (sizeof(buf) -1) )
    {
        pchNew = new char[intLength+1];
        pchContent = pchNew;
    }
    ::strcpy(pchContent,pchToken);
    //
    char * ptr_r = NULL;
    char * pchTemp = ::strtok_r( pchContent, pchDelimit , &ptr_r);
    while(pchTemp)
    {
        vectRet.insert(vectRet.end(),pchTemp);
        pchTemp = ::strtok_r( NULL, pchDelimit , &ptr_r);
    }
    if( pchNew != NULL )
        delete[] pchNew;
    return true;
}

inline void TrimString(char * lpszStr) //assert that str ended with '\0'
{
    int l = strlen(lpszStr);
//detect front spaces
    int i = 0;
    while(lpszStr[i] == ' ' || lpszStr[i] == '\t')
    {
        i++;
    }
    memmove(lpszStr, lpszStr + i, l - i + 1);
    l -= i;
//detect rear spaces
    if (l <= 1)
        return;
    i = l - 1;
    while(lpszStr[i] == ' ' || lpszStr[i] == '\t')
    {
        i--;
    }
    lpszStr[i + 1] = '\0';
}

inline void ConvertToCapital(string & str)
{
    const int const_intLen = str.size() + 1;
    char pchValue[const_intLen];
    strcpy(pchValue, str.c_str());
    for(int i = 0;i < const_intLen; i++)
    {
        if((pchValue[i]>='a') && (pchValue[i]<='z'))
        {
            pchValue[i] = pchValue[i] - 'a' + 'A';
        }
    }

    str = pchValue;
}

inline void Log2(CLog::Level enuLevel, const char * pchClassName, const char * pchFunc, const char *pFormat, ...) {
    if (CLog::s_pLog != NULL) {
        va_list arglist;
        va_start(arglist, pFormat);
        CLog::s_pLog->Log(enuLevel, pchClassName, pchFunc, pFormat, arglist);
        va_end(arglist);
    }
}

static inline
void LogFileFunc(CLog::Level enuLevel, const char *file, const char * pchFunc, const char *pFormat, ...) {
    int status = 0;
    va_list arglist;
    va_start(arglist, pFormat);
    if (CLog::s_pLog != NULL) {
        CLog::s_pLog->Log(enuLevel, file, pchFunc, pFormat, arglist);
    } else {
        CLog::Log2Console(enuLevel, file, pchFunc, pFormat, arglist);
    }
    va_end(arglist);
}

static inline
void Log(CLog::Level enuLevel, const char *file, int line, const char *pFormat, ...) {
    int status = 0;
    va_list arglist;
    va_start(arglist, pFormat);
    if (CLog::s_pLog != NULL) {
        CLog::s_pLog->Log(enuLevel, file, line, pFormat, arglist);
    } else {
        CLog::Log2Console(enuLevel, file, line, pFormat, arglist);
    }
    va_end(arglist);
}

template <typename T>
inline const char* GetTypeName(char *buffer, size_t len) {
    buffer[0] = 0;
    int status = 0;
    const char *raw_name = typeid(T).name();
    abi::__cxa_demangle(raw_name, buffer, &len, &status);
    if (status == 0) {
        return buffer;
    } else {
        return raw_name;
    }
}

template<typename Tp>
void Log(CLog::Level enuLevel, const Tp * pThis, const char * pchFunc, const char *pFormat, ...) {
    char name_buffer[256] = {0};
    const char* name = GetTypeName<Tp>(name_buffer, sizeof(name_buffer));
    va_list arglist;
    va_start(arglist, pFormat);
    if (CLog::s_pLog != NULL) {
        CLog::s_pLog->Log(enuLevel, name, pchFunc, pFormat, arglist);
    } else {
        CLog::Log2Console(enuLevel, name, pchFunc, pFormat, arglist);
    }
    va_end(arglist);
}

inline void DumpBuf(const CLog::Level enuLevel
                ,const char *pchFile
                ,const char *pchFunc
                ,const char *pchTitle
                ,const char *pchBuf
                ,const int intBufSize
                ,const int intDumpSize
                )
{
    if( CLog::s_pLog != NULL )
    {
        CLog::s_pLog->DumpBuf(enuLevel, pchFile, pchFunc, pchTitle, pchBuf, intBufSize, intDumpSize);
    }
}

inline int GetDiffMicroSec(const timeval & sRecBeg, const timeval & sRecEnd)
{
    return (sRecEnd.tv_sec-sRecBeg.tv_sec)*1000000 + (sRecEnd.tv_usec-sRecBeg.tv_usec);
}

template<typename Tp>
void AddToBuf(const Tp & data, vector<char> & vectPack, const bool blnBegin = false)
{
    char * pBuf = (char *)&data;
    if(blnBegin)
    {
        vectPack.insert(vectPack.begin(), pBuf, pBuf + sizeof(Tp));
    }
    else
    {
        vectPack.insert(vectPack.end(), pBuf, pBuf + sizeof(Tp));
    }
}

inline void AddStrToBuf(const string & data, vector<char> & vectPack)
{
    vectPack.insert(vectPack.end(), data.c_str(), data.c_str() + data.size() + 1);
}

inline void AddStrToBuf2(const string & data, vector<char> & vectPack)
{
    unsigned short wLen = data.size();
    AddToBuf(wLen, vectPack);
    vectPack.insert(vectPack.end(), data.c_str(), data.c_str() + data.size());
}

template<typename Tp>
int OutFromBuf(Tp & data, const char * pBuf)
{
    memcpy(&data, pBuf, sizeof(data));
    return sizeof(data);
}

inline int OutStrFromBuf(string & data, const char * pBuf)
{
    data = pBuf;
    return data.size() + 1;
}

inline int OutStrFromBuf2(string & data, const char * pBuf)
{
    int intSize = 0;
    unsigned short wLen =0;
    intSize += OutFromBuf(wLen, pBuf);
    if(wLen)
    {
        vector<char> tmp;
        tmp.insert(tmp.end(), pBuf + intSize, pBuf + intSize + wLen);
        intSize += wLen;
        tmp.push_back(0);
        data = &*tmp.begin();
    }

    return intSize;
}

inline int CreateDir(const char *sPathName)
{
    char   DirName[256];
    strcpy(DirName,   sPathName);
    int   i,len   =   strlen(DirName);
    if(DirName[len-1] != '/')
        strcat(DirName,   "/");

    len   =   strlen(DirName);
    for( i = 1 ; i < len ; i++ )
    {
        if( DirName[i] == '/' )
        {
            DirName[i] = 0;
            if( access(DirName, 0) !=0 )
            {
                if(mkdir(DirName, 0755)==-1)
                {
                    perror("mkdir   error");
                    return -1;
                }
            }
            DirName[i] = '/';
        }
    }
    return 0;
};

inline int upperStr(char * p)
{
    if(p == NULL)
        return -1;
    for(;*p != 0; p++)
    {
        if(*p >= 'a' && *p <= 'z')
            *p= *p - 32;
    }
    *p = '\0';
    return 0;
}


inline std::vector<std::string> split_new(std::string str,std::string pattern)
{
    std::string::size_type pos;
    std::vector<string> result;
    str+=pattern;
    int size=str.size();

    for(int i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            std::string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}

template <typename T>
inline void shrink_to_fit(std::vector<T>& c)
{
    if (c.size() != c.capacity()) {
        std::vector<T>(c).swap(c);
    }
}

template <typename T, size_t N>
inline size_t array_size(const T (&array)[N])
{
    return N;
}

inline string stringtrim(const string& str, const char tok)
{
	string tmp = str;
	string::size_type pos = tmp.find_last_not_of(tok);
	if(pos != string::npos)
	{
		tmp.erase(pos + 1);
		pos = tmp.find_first_not_of(tok);
		if(pos != string::npos)
			tmp.erase(0, pos);
	}
	else
		tmp.erase(tmp.begin(), tmp.end());
	return tmp;
}

// We assume value and relative is positive, at least relative is.
template <typename T>
inline int
GetRiseAsInt(T value, T relative, int enlarge_times = 10000)
{
    if (!relative || !value) {
        return 0;
    }

    long long sv = value;
    long long sr = relative;
    if (sv >= sr) {
        return ((sv - sr) * enlarge_times + (sr >> 1)) / sr;
    } else {
        return ((sv - sr) * enlarge_times - (sr >> 1)) / sr;
    }
}

namespace multicommon
{
    // The path should exist.
    inline std::string
    GetAbsPath(const std::string &path) {
        char abs_path[PATH_MAX];
        if (realpath(path.c_str(), abs_path)) {
            return abs_path;
        } else {
            return path;
        }
    }

    // The path may not exist
    inline std::string
    GetAbsDir(const std::string &path) {
        struct stat st;
        if (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            return GetAbsPath(path);
        }

        std::string::size_type slash = path.rfind('/');
        if (slash == std::string::npos) {
            return GetAbsDir(".");
        } else {
            return GetAbsDir(path.substr(0, slash));
        }
    }

    inline std::vector<std::string> split_string(const std::string &str, char delim)
    {
        std::vector<string> result;

        string::const_iterator it = str.begin();
        while (it != str.end()) {
            string::const_iterator pos = find(it, str.end(), delim);
            if (pos != it) {
                result.push_back(string(it, pos));
            }

            if (pos == str.end()) {
                break;
            }

            it = ++pos;
        }

        return result;
    }

    template <typename T, typename ConverterFunction>
    inline std::vector<T> split_string(const std::string &str,
                                       char delim,
                                       ConverterFunction func)
    {
        std::vector<T> result;

        string::const_iterator it = str.begin();
        while (it != str.end()) {
            string::const_iterator pos = find(it, str.end(), delim);
            if (pos != it) {
                string s(it, pos);
                result.push_back(func(s));
            }

            if (pos == str.end()) {
                break;
            }

            it = ++pos;
        }

        return result;
    }

    template <typename K, typename V>
    std::vector<K> get_map_keys(const std::map<K, V> &src) {
        std::vector<K> result;
        result.reserve(src.size());

        typedef typename std::map<const K, V>::const_iterator IteratorType;
        for (IteratorType it = src.begin(); it != src.end(); ++it) {
            result.push_back(it->first);
        }

        return result;
    }

    template <typename K, typename V>
    std::vector<V> get_map_values(const std::map<K, V> &src) {
        std::vector<V> result;
        result.reserve(src.size());

        typedef typename std::map<const K, V>::const_iterator IteratorType;
        for (IteratorType it = src.begin(); it != src.end(); ++it) {
            result.push_back(it->second);
        }

        return result;
    }

    template <typename K, typename V>
    void get_map_values(const std::map<K, V> &src, std::vector<V> &dest) {
        typedef typename std::map<const K, V>::const_iterator IteratorType;
        for (IteratorType it = src.begin(); it != src.end(); ++it) {
            dest.push_back(it->second);
        }
    }
}

#define UNUSED_PARAM(x) ((void) (x))

#define LOG_STATUS_FILE_FUNC(__fmt, ...) ::LogFileFunc(CLog::Status, __FILE__, __FUNCTION__, __fmt, ##__VA_ARGS__)
#define LOG_WARN_FILE_FUNC(__fmt, ...) ::LogFileFunc(CLog::Warn, __FILE__, __FUNCTION__, __fmt, ##__VA_ARGS__)
#define LOG_ERROR_FILE_FUNC(__fmt, ...) ::LogFileFunc(CLog::Error, __FILE__, __FUNCTION__, __fmt, ##__VA_ARGS__)
#define LOG_INFO_FILE_FUNC(__fmt, ...) ::LogFileFunc(CLog::Info, __FILE__, __FUNCTION__, __fmt, ##__VA_ARGS__)

#define LOG_STATUS_FILE_LINE(__fmt, ...) ::Log(CLog::Status, __FILE__, __LINE__, __fmt, ##__VA_ARGS__)
#define LOG_WARN_FILE_LINE(__fmt, ...) ::Log(CLog::Warn, __FILE__, __LINE__, __fmt, ##__VA_ARGS__)
#define LOG_ERROR_FILE_LINE(__fmt, ...) ::Log(CLog::Error, __FILE__, __LINE__, __fmt, ##__VA_ARGS__)
#define LOG_INFO_FILE_LINE(__fmt, ...) ::Log(CLog::Info, __FILE__, __LINE__, __fmt, ##__VA_ARGS__)

#define LOG_STATUS_THIS_FUNC(__fmt, ...) ::Log(CLog::Status, this, __FUNCTION__, __fmt, ##__VA_ARGS__)
#define LOG_WARN_THIS_FUNC(__fmt, ...) ::Log(CLog::Warn, this, __FUNCTION__, __fmt, ##__VA_ARGS__)
#define LOG_ERROR_THIS_FUNC(__fmt, ...) ::Log(CLog::Error, this, __FUNCTION__, __fmt, ##__VA_ARGS__)
#define LOG_INFO_THIS_FUNC(__fmt, ...) ::Log(CLog::Info, this, __FUNCTION__, __fmt, ##__VA_ARGS__)

#define DEBUG_INFO_OUTPUT
#define DEBUG_EXCLUDE

template<class out_type, class in_value>
inline out_type ConvertDataType(const in_value & t)
{
    stringstream stream;
    stream<<t;
    out_type result;
    stream>>result;
    return result;
}

#endif
