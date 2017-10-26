#ifndef RUNSYSCMD_H 
#define RUNSYSCMD_H

#include <string>
#include <stdio.h>
#include "thread.h"
#include "log.h"

namespace BackCom {
    class RunCmd : public CThread {
        private:
            const std::string mCmd;
        public:
            explicit RunCmd(const std::string& cmd) : CThread(), mCmd(cmd) {
            }
            virtual void Execute(void*) {
                FILE* fp = popen(mCmd.c_str(), "r");
                if(fp == NULL) return;
                std::string result;
                while(!feof(fp)) {
                    char buf[1024] = {0};
                    fgets(buf, sizeof(buf), fp);
                    result += buf;
                }
                if(CLog::s_pLog) {
                    CLog::s_pLog->Log(CLog::Status, __FILE__, __LINE__, "cmd:%s, result:%s", mCmd.c_str(), result.c_str());
                }
                pclose(fp);
            }
    };
}

#endif /*RUNSYSCMD_H*/
