#include <iostream>
#include <cstring>
#include <cstdlib>
#include <dirent.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <signal.h>
#include "appmgr.h"

static AppMgr *AppMgrInstance = nullptr;

void register_signalHandle(AppMgr *app);
void signalHandler(int signal);

AppMgr::AppMgr()
{
    running = true;
}

AppMgr::~AppMgr()
{
}

/**
 * List of services
 */

static std::vector<process_info> proc_info = {
    {"Service C: ",
     "/home/vboxuser/vscode/learncpp/svmgr/c/c",
     "/home/vboxuser/vscode/learncpp/svmgr/c/c &",
     "pkill -9 -f /home/vboxuser/vscode/learncpp/svmgr/c/c",
     -1,
     false},

    {"Service Testlinhtinh: ",
     "/home/vboxuser/vscode/learncpp/test_linhtinh/testlinhtinh",
     "/home/vboxuser/vscode/learncpp/test_linhtinh/testlinhtinh &",
     "pkill -9 -f /home/vboxuser/vscode/learncpp/test_linhtinh/testlinhtinh",
     -1,
     false},

    {"Service Test1: ",
     "/home/vboxuser/vscode/learncpp/test1",
     "/home/vboxuser/vscode/learncpp/test1 &",
     "pkill -9 -f /home/vboxuser/vscode/learncpp/test1",
     -1,
     false},

    {"Service asadf: ",
     "/home/vboxuser/vscode/learncpp/asadf",
     "/home/vboxuser/vscode/learncpp/asadf &",
     "pkill -9 -f /home/vboxuser/vscode/learncpp/asadf",
     -1,
     false}};

pid_t AppMgr::findProc(const char *procpath)
{
    DIR *dir = opendir("/proc");
    if (dir == nullptr)
    {
        return -1;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != nullptr)
    {
        char *endptr = nullptr;
        long pid = strtol(entry->d_name, &endptr, 10);

        if (*endptr != '\0')
        {
            continue;
        }

        char cmdLinePath[256];
        snprintf(cmdLinePath, sizeof(cmdLinePath), "/proc/%ld/cmdline", pid);
        FILE *file = fopen(cmdLinePath, "r");

        if (file == nullptr)
        {
            continue;
        }

        char cmdLine[256];
        if (fgets(cmdLine, sizeof(cmdLine), file) != nullptr)
        {
            if (strcmp(cmdLine, procpath) == 0)
            {
                fclose(file);
                closedir(dir);
                return static_cast<pid_t>(pid);
            }
        }
        fclose(file);
    }
    closedir(dir);
    return -1;
}

bool AppMgr::isRunning(process_info &proc)
{
    pid_t pid = findProc(proc.path);

    if (pid == -1)
    {
        proc.running = false;
        proc.pid = -1;

        return false;
    }
    else
    {
        proc.running = true;
        proc.pid = pid;

        return true;
    }
}

void AppMgr::startProc(process_info &proc)
{
    std::cout << "Starting " << proc.name << "..." << std::endl;
    int result = system(proc.startCmd);
    if (result != 0)
    {
        std::cout << "Fail to start " << proc.name << std::endl;
        return;
    }

    pid_t pid = findProc(proc.path);
    if (pid == -1)
    {
        std::cout << "Process started but PID not found" << std::endl;
        return;
    }
    else
    {
        proc.pid = pid;
        proc.running = true;
        std::cout << "Started " << proc.name << " PID = " << pid << std::endl;
        return;
    }
}

void AppMgr::Stop()
{
    running = false;
    for (int i = 0; i < proc_info.size(); i++)
    {
        if (!isRunning(proc_info[i]))
        {
            continue;
        }
        system(proc_info[i].killCmd);
        proc_info[i].running = false;
        proc_info[i].pid = -1;
    }
}

void AppMgr::Init()
{
    register_signalHandle(this);

    for (auto &proc : proc_info)
    {
        pid_t pid = findProc(proc.path);
        if (pid == -1)
        {
            std::cout << "[AppMgr] " << proc.name << " is not running/" << std::endl;
            startProc(proc);
        }
        else
        {
            proc.pid = pid;
            proc.running = true;

            std::cout << "[AppMgr] " << proc.name << " already running. PID = " << pid << std::endl;
        }
    }
}

void AppMgr::Mornitor()
{
    while (running)
    {
        std::cout << std::endl;
        for (int i = 0; i < proc_info.size(); i++)
        {
            if (!isRunning(proc_info[i]))
            {
                std::cout << "[AppMgr] " << proc_info[i].name << " died" << std::endl;

                startProc(proc_info[i]);
            }
            else
            {
                std::cout << "[AppMgr] " << proc_info[i].name << " PID = " << proc_info[i].pid << " is running" << std::endl;
            }
        }
        sleep(5);
    }
}

void signalHandler(int signal)
{
    if (AppMgrInstance != nullptr)
    {
        AppMgrInstance->Stop();
    }
    return;
}

void register_signalHandle(AppMgr *app)
{
    AppMgrInstance = app;
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
}
