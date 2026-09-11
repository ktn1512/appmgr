#ifndef APPMGR_H
#define APPMGR_H

#include <sys/types.h>
#include <vector>
#include "proc_info.h"

class AppMgr
{
private:
    /**
     * running: AppMgr running or not
     * findProc: find pid form running process path
     * isRunning: check process running or not
     * startProc: check process starting or not
     */
    bool running;
    pid_t findProc(const char *procPath);
    bool isRunning(process_info &proc);
    void startProc(process_info &proc);

public:
    /**
     *
     */

    AppMgr();
    ~AppMgr();
    void Init();
    void Mornitor();
    void Stop();
};

#endif
