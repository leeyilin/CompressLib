/* 
 * File:   mylognumctrl.h
 * Author: litaozhi
 *
 * Created on 2015年3月31日, 上午11:03
 */

#ifndef MYLOGNUMCTRL_H
#define	MYLOGNUMCTRL_H

#include <vector>
#include <string>
using namespace std;

#include "thread.h"
#include "lock.h"
#include "log.h"
#include "config.h"

namespace BackCom
{
class CMyLogNumCtrl : public CThread
{
public:
    CMyLogNumCtrl();
    ~CMyLogNumCtrl();
    bool Init(const CConfig * pConfig, const std::string& logPath);
    bool StartThread();
    bool StopThread();
private:
    void Execute(void *arg);
    
private:
        int m_intMaxDayLog;
        std::string mLogPath;

};

}

#endif	/* MYLOGNUMCTRL_H */

