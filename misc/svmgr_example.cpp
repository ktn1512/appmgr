#include <iostream>
#include <sys/wait.h>
#include <csignal>
#include <unistd.h>
#include <sys/signal.h>
#include <cstring>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <vector>

struct service
{
    std::string path{};
    int cur_pid = -1;
};

std::vector<service> sv_list = {
    {"/home/vboxuser/vscode/learncpp/svmgr/c/c", -1},
    {"/home/vboxuser/vscode/learncpp/test_linhtinh/testlinhtinh", -1},
    {"/home/vboxuser/vscode/learncpp/asadf", -1},
    {"/home/vboxuser/vscode/learncpp/test1", -1}};

void write_log(const service &s, int status)
{
    time_t now = time(NULL);
    char *t = ctime(&now);

    std::ofstream f;
    f.open("service.log", std::ios::app);

    f << t << "Service: " << s.path << " (pid = " << s.cur_pid << " ) ";

    if (WIFEXITED(status))
    {
        f << "exitted by code = " << WEXITSTATUS(status);
        std::cout << "exitted by code = " << WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status))
    {
        if (WTERMSIG(status) == SIGFPE)
        {
            std::cout << " Lỗi chia cho 0. Error code = " << SIGFPE;
            f << " Lỗi chia cho 0. Error code = " << SIGFPE;
        }
        else if (WTERMSIG(status) == SIGSEGV)
        {
            f << " tràn stack rồi. Error code = " << SIGSEGV;
            std::cout << " tràn stack rồi. Error code = " << SIGSEGV;
        }
        else
        {
            f << " Bị kill. Signal = " << WTERMSIG(status);
            std::cout << " Bị kill. Signal = " << WTERMSIG(status);
        }
    }
    f << "\n\n";
}

int main()
{
    for (auto &s : sv_list)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
        }
        else if (pid == 0)
        {
            int check = execl(s.path.c_str(), s.path.c_str(), NULL);
            if (check < 0)
            {
                perror(s.path.c_str());
                return 1;
            }
        }
        else
        {
            s.cur_pid = pid;
        }
    }

    while (1)
    {
        int status;
        int die_pid = waitpid(-1, &status, WNOHANG);
        if (die_pid > 0)
        {
            for (auto &s : sv_list)
            {
                if (s.cur_pid == die_pid)
                {
                    write_log(s, status);

                    bool restart = 0;

                    if (WIFSIGNALED(status))
                    {
                        restart = 1;
                    }
                    else if (WIFEXITED(status))
                    {
                        if (WEXITSTATUS(status) != 0)
                        {
                            restart = 1;
                        }
                    }

                    if (restart)
                    {
                        pid_t rst_pid = fork();

                        if (rst_pid < 0)
                        {
                            perror("fork");
                            _exit(1);
                        }
                        else if (rst_pid == 0)
                        {
                            int check = execl(s.path.c_str(), s.path.c_str(), NULL);
                            if (check < 0)
                            {
                                perror(s.path.c_str());
                                _exit(127);
                            }
                        }
                        else
                        {
                            s.cur_pid = rst_pid;
                        }
                    }
                    else
                    {
                        s.cur_pid = -1;
                    }
                    break;
                }
            }
        }
        sleep(1);
    }
}
