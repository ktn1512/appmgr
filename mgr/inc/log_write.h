#ifndef LOG_WRITE_H
#define LOG_WRITE_H

#include <iostream>
#include <fstream>

class AppMgr;

class LogWrite
{
public:
    void logW(std::string &message, const AppMgr &appmgr);
};

#endif
