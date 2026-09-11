#ifndef PROC_INFO_H
#define PROC_INFO_H

#include <sys/types.h>
/**definite struct process_info
 * name: process name
 * path: process execute path
 * startCmd: execute shell command process path
 * pid: process id
 * running: process running or not
 **/
struct process_info
{
    const char *name;
    const char *path;
    const char *startCmd;
    const char *killCmd;
    
    pid_t pid;

    bool running;
};

#endif
