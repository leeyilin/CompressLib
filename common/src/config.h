#ifndef CCONFIG_H
#define CCONFIG_H

#include <string>
#include <map>
using namespace std;

namespace BackCom
{

class CConfig
{
public:
    CConfig();
    CConfig(const string & file_name);
    virtual ~CConfig();
    //
    bool Init(const string & file_name);
    bool GetValue(const string & group_name,const string & item_name,string & value, const string & strDefault="")const;
    bool GetValue(const string & group_name,const string & item_name, int & value, int nDefault=0)const;
    bool GetValue(const string & group_name,const string & item_name, double & value, double dbDefault=0.0)const;
    bool GetValue(const string & group_name,const string & item_name, float & value, float fDefault=0.0)const;
    bool GetValue(const string & group_name, map<string, string> & mapGroupVal)const;
    bool SetValue(const string & group_name,const string & item_name,const string & value);
    bool Remove(const string & group_name);
    bool Remove(const string & group_name,const string & item_name);
    bool Exist(const string & group_name,const string & item_name)const;
    bool PrintAll()
    {
        string strOutput;
        PrintAllToString(strOutput);
        printf("%s",strOutput.c_str());
        return true;
    }
    bool Refresh();
    bool Save(const string & strMultiLineHeaders);
    bool Clear();

    // 返回一个组中所有设置项的map对象
    bool GetGroup(map<string,string> & mapGroup
                 ,const string & group_name
                 )const
    {
        STR2MAP::const_iterator map_it = m_mapGroups.find(group_name);
        if( map_it != m_mapGroups.end() )
        {
            mapGroup = (*map_it).second;
        }
        return true;
    };
    bool PrintAllToString(string & strOutput);

private:
    static char* str_trim(char *ptr);
private:
    string m_strFileName;
    typedef map<string,string> STR2STR;
    typedef map<string,STR2STR> STR2MAP;
    STR2MAP m_mapGroups;
    //
};

}// namespace BackCom
#endif // #ifndef __CONFIG_H
