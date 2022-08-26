#include <iostream>
#include <stdio.h>
#include <time.h>
#include "logger.h"

using std::string;
using std::cout;
using std::endl;

// Timer 获取格式化的时间
string Timer::getFormatTime() {
    time_t temp;
    time (&temp);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&temp));
    return tmp;
}

// 初始化类中的文件描述符
Logger::Logger() {
    fd = fopen("a.log", "w");
}

Logger::~Logger() {
    fclose(fd);
}

void Logger::writeLog(string &log) {
    string time = Timer::getFormatTime();
    fwrite(time.c_str(), sizeof(time), 1, fd);
    fwrite(log.c_str(), sizeof(log), 1, fd);
}