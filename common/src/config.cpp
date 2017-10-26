#include "config.h"

#include <stdio.h>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <string.h>
using namespace std;

namespace BackCom
{

CConfig::CConfig()
{
}

CConfig::CConfig(const string & file_name)
{
    m_strFileName = file_name;
}

CConfig::~CConfig()
{
    m_mapGroups.clear();
}

bool CConfig::Clear()
{
    m_mapGroups.clear();
    return true;
}

bool CConfig::Init(const string & file_name)
{
    m_strFileName = file_name;
    return Refresh();
}

bool CConfig::Refresh()
{
    m_mapGroups.clear();
    const int _const_buf_size = 1024;
    // Read from file
    ifstream in1(m_strFileName.c_str());
    if (!in1.is_open())
        return false;
    char buf[_const_buf_size];
    string group_name;
    STR2STR map1;
    while( true )
    {
        char pchTmp[_const_buf_size];
        in1.getline(pchTmp,_const_buf_size-1);
        pchTmp[_const_buf_size-1] = '\0';       // set string terminal char
        if( in1.gcount() > 0 )
        {
            // erase after '#' content
            for(int i = 0 ; i < (int)strlen(pchTmp) ; i++ )
            {
                if( pchTmp[i] == '#' )
                {
                    pchTmp[i] = '\0';
                    break;
                }
            }
            char * pch = str_trim(pchTmp);
            if( strlen(pch) == 0 || pch[0] == ';')
                continue;
            strncpy(buf,pch,_const_buf_size);
            buf[_const_buf_size-1] = '\0';
            //
            if( buf[0] == '[' )
            {
                if( group_name.size() > 0 )
                {
                    STR2STR::iterator i;
                    for( i = map1.begin(); i != map1.end() ; i++ )
                        SetValue(group_name,(*i).first,(*i).second);
                }
                buf[strlen(buf) - 1] = 0;       // erase ']'
                group_name = str_trim(buf + 1);
                map1.clear();
            }
            else if( group_name.size() > 0 )
            {
                char *ptr = buf;
                while ( *ptr != '=' )
                    ptr++;
                *ptr++ = 0;
                //
                map1[str_trim(buf)] = str_trim(ptr);
            }
        }
        if( in1.eof() != 0 )
        {
            if( group_name.size() > 0 ) //last group
            {
                STR2STR::iterator i;
                for( i = map1.begin(); i != map1.end() ; i++ )
                    SetValue(group_name,(*i).first,(*i).second);
            }
            break;
        }
    }
    in1.close();
    //
    return true;


}

bool CConfig::Exist(const string & group_name,const string & item_name) const
{
    //锟斤拷值锟斤拷锟??7
    STR2MAP::const_iterator map_it = m_mapGroups.find(group_name);
    if (map_it != m_mapGroups.end()) {
        const STR2STR & map1 = (*map_it).second;
        if (map1.find(item_name) != map1.end())
            return true;
    }
    return false;
}

bool CConfig::GetValue(const string & group_name,const string & item_name,string & value, const string & strDefault) const
{
    //取锟街??7

    value = strDefault;
    STR2MAP::const_iterator map_it = m_mapGroups.find(group_name);
    if( map_it != m_mapGroups.end() )
    {
        const STR2STR & map1 = (*map_it).second;
        STR2STR::const_iterator it = map1.find(item_name);
        if( it != map1.end() )
        {
            value = (*it).second;
        }
    }
    return true;
}

bool CConfig::GetValue(const string & group_name,const string & item_name, int & value, int nDefault) const
{
    string strValue;
    GetValue(group_name, item_name, strValue, "");
    if (!strValue.empty())
    {
        value = ::atoi(strValue.c_str());
    }
    else
    {
        value = nDefault;
    }
    return true;
}

bool CConfig::GetValue(const string & group_name,const string & item_name, double & value, double dbDefault) const
{
    string strValue;
    GetValue(group_name, item_name, strValue, "");
    if (!strValue.empty())
    {
        value = ::atof(strValue.c_str());
    }
    else
    {
        value = dbDefault;
    }
    return true;
}

bool CConfig::GetValue(const string & group_name,const string & item_name, float & value, float fDefault) const
{
    string strValue;
    GetValue(group_name, item_name, strValue, "");
    if (!strValue.empty())
    {
        value = (float)::atof(strValue.c_str());
    }
    else
    {
        value = fDefault;
    }
    return true;
}

bool CConfig::GetValue(const string & group_name, map < string, string > & mapGroupVal)const
{
    STR2MAP::const_iterator map_it = m_mapGroups.find(group_name);
    mapGroupVal.clear();
    if( map_it != m_mapGroups.end() )
    {
        mapGroupVal = (*map_it).second;
    }
    return true;
}

bool CConfig::SetValue(const string & group_name,const string & item_name,const string & value)
{
    STR2STR & map1 = m_mapGroups[group_name];
    map1[item_name] = value;
    return true;
}

bool CConfig::Remove(const string & group_name)
{
    m_mapGroups.erase(group_name);
    return true;
}

bool CConfig::Remove(const string & group_name,const string & item_name)
{
    STR2MAP::iterator it = m_mapGroups.find(group_name);
    if (it != m_mapGroups.end())
    {
        (*it).second.erase(item_name);
    }
    return true;
}


bool CConfig::PrintAllToString(string & strOutput)
{
    STR2MAP::iterator it1;
    for( it1 = m_mapGroups.begin(); it1 != m_mapGroups.end(); it1++ )
    {
        string group_name =  (*it1).first;
        STR2STR & map1 = m_mapGroups[group_name];
        strOutput += "[";
        strOutput += group_name;
        strOutput += "]\n";
        //printf("[%s]\n",group_name.c_str());
        //
        STR2STR::iterator it2;
        for( it2 = map1.begin(); it2 != map1.end(); it2++ )
        {
            strOutput += "\t'";
            strOutput += (*it2).first;
            strOutput += "'='";
            strOutput += (*it2).second;
            strOutput += "'\n";
            //printf("\t'%s'='%s'\n",(*it2).first.c_str(),(*it2).second.c_str());
        }
    }
    return true;
}

bool CConfig::Save(const string & strMultiLineHeaders)
{
    vector<char> vec;
    vector<char>::iterator vecIter;//pug add
    int nLen = (int)strMultiLineHeaders.size();
    for (int i = 0; i < nLen; i++)
    {
        if (i == 0)
            vec.insert(vec.end(), '#');
        vec.push_back(strMultiLineHeaders[i]);
        if (i < nLen - 1 && strMultiLineHeaders[i] == '\n')
            vec.insert(vec.end(), '#');
        else if(i == nLen - 1)
            vec.insert(vec.end(), '\n');
    }

    STR2MAP::iterator it1;
    for( it1 = m_mapGroups.begin(); it1 != m_mapGroups.end(); it1++ )
    {
        string group_name =  (*it1).first;
        STR2STR & map1 = m_mapGroups[group_name];

        vec.insert(vec.end(), '[');
        vec.insert(vec.end(), group_name.begin(),group_name.end());
        vec.insert(vec.end(), ']');
        vec.insert(vec.end(), '\n');

        STR2STR::iterator it2;
        for( it2 = map1.begin(); it2 != map1.end(); it2++ )
        {
            vec.insert(vec.end(), (*it2).first.begin(), (*it2).first.end());
            vec.insert(vec.end(), ' ');
            vec.insert(vec.end(), '=');
            vec.insert(vec.end(), ' ');
            vec.insert(vec.end(), (*it2).second.begin(), (*it2).second.end());
            vec.insert(vec.end(), '\n');
        }
    }

    if (vec.size() == 0)
        return true;

    int fd;
    vecIter = vec.begin();
    if ((fd = ::open(m_strFileName.c_str(), O_RDWR|O_CREAT, 0644)) >= 0)
    {
        ::lseek(fd, 0, SEEK_SET);
        ::write(fd, (void*)(&*vecIter), vec.size());
        ::ftruncate(fd, vec.size());
        ::close(fd);
        return true;
    }
    else
        return false;
}

char* CConfig::str_trim(char *ptr)
{
    while( *ptr == ' ' || *ptr== '\t' || *ptr=='\r' ||*ptr=='\n')
        ptr++;
    int n = strlen(ptr);
    while( n > 0 )
    {
        if( *(ptr + n - 1) != ' ' && *(ptr + n - 1) != '\t' && *(ptr + n - 1) != '\r'&& *(ptr + n - 1) != '\n')
            break;

        n--;
    }
    *(ptr + n) = 0;
    return ptr;
}

}// namespace BackCom
