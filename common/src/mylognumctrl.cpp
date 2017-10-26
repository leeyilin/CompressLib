#include "mylognumctrl.h"
using namespace BackCom;

#include "comfun.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <set>
using namespace std;

bool is_dir(const char *path) 
{
    struct stat statbuf;
    if (lstat(path, &statbuf) == 0)//lstat?????????????????????????stat????
    {
        return S_ISDIR(statbuf.st_mode) != 0; //S_ISDIR???ж??????????????
    }
    return false;
}

//?ж????????????
bool is_file(const char *path) 
{
    struct stat statbuf;
    if (lstat(path, &statbuf) == 0)
        return S_ISREG(statbuf.st_mode) != 0; //?ж???????????????
    return false;
}

bool is_special_dir(const char *path) 
{
    return strcmp(path, ".") == 0 || strcmp(path, "..") == 0;
}

void get_file_path(const char *path, const char *file_name, char *file_path) 
{
    strcpy(file_path, path);
    if (file_path[strlen(path) - 1] != '/')
        strcat(file_path, "/");
    strcat(file_path, file_name);
}

void delete_file(const char *path) 
{
    DIR *dir;
    dirent *dir_info;
    char file_path[PATH_MAX];
    if (is_file(path)) 
    {
        remove(path);
        return;
    }
    if (is_dir(path)) 
    {
        if ((dir = opendir(path)) == NULL)
            return;
        while ((dir_info = readdir(dir)) != NULL) 
        {
            get_file_path(path, dir_info->d_name, file_path);
            if (is_special_dir(dir_info->d_name))
                continue;
            delete_file(file_path);
            rmdir(file_path);
        }

        closedir(dir);
        rmdir(path);
    }
}

void GetAllFile(const char *path, set<string> & setDir) 
{
    DIR *dir;
    dirent *dir_info;
    char file_path[PATH_MAX];
    if (is_dir(path)) 
    {
        if ((dir = opendir(path)) == NULL)
            return;
        while ((dir_info = readdir(dir)) != NULL) 
        {
            get_file_path(path, dir_info->d_name, file_path);
            if (is_special_dir(dir_info->d_name))
                continue;
            if (is_file(file_path)) 
            {
                setDir.insert(file_path);
            }
        }

        closedir(dir);
    }
}

CMyLogNumCtrl::CMyLogNumCtrl()
{
    m_intMaxDayLog = 5;
}

CMyLogNumCtrl::~CMyLogNumCtrl()
{
    
}

bool CMyLogNumCtrl::Init(const CConfig * pConfig, const std::string& logPath)
{
    mLogPath = logPath;
    if(pConfig->GetValue("log", "maxday", m_intMaxDayLog, 5)== false)
    {
        return false;
    }
    
    return true;
}

void CMyLogNumCtrl::Execute(void *arg) 
{
    int intCount = 0;
    while (!GetStopFlag()) 
    {
        usleep(1 * 1000 * 1000);
        intCount++;
        if( (intCount % (3600 * 12) ) == 0 )
        {
             Log(CLog::Status, "CMyLogNumCtrl", __FUNCTION__, "Scan log begin");
            set<string> setFile;
            GetAllFile(mLogPath.c_str(), setFile);
            if (m_intMaxDayLog < setFile.size()) 
            {
                int intDelCount = setFile.size() - m_intMaxDayLog;
                for (set<string>::iterator it = setFile.begin(); it != setFile.end(); it++) 
                {
                    delete_file(it->c_str());
                    intDelCount--;
                    if (intDelCount <= 0) 
                    {
                        Log(CLog::Status, "CMyLogNumCtrl", __FUNCTION__, "Delete log end");
                        break;
                    }
                }
                Log(CLog::Status, "CMyLogNumCtrl", __FUNCTION__, "Scan log end");
            
            }
        }
  }
}

bool CMyLogNumCtrl::StartThread() 
{
    
    CThread::Start(NULL, true);
    Log(CLog::Status, "CMyLogNumCtrl", __FUNCTION__, "Delete log file pthread start!");
    return true;
}

bool CMyLogNumCtrl::StopThread() 
{
    CThread::Stop();
    void * pRet = NULL;
    CThread::Join(&pRet);
    Log(CLog::Status, "CMyLogNumCtrl", __FUNCTION__, "Delete log file pthread end!");
    return true;
}
