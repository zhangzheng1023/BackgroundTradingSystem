#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <mutex>
#include <time.h>
#include <sys/time.h>

using std::string;

enum LOG_LEVEL {
	LOG_LEVEL_FATAL,  // fatal
	LOG_LEVEL_ERROR,  // error
	LOG_LEVEL_WARNING,  // warning
	LOG_LEVEL_INFO,  // info
	LOG_LEVEL_DEBUG  // debug	
};

// 获取线程号
extern pid_t getTid();

class Timer {
public:
	static string getFormatTime();
private:
	
};

class Buffer {
public:

private:

};

class Logger {
public:
	// 单例模式实现
	static Logger* ins() {
		pthread_once(&_once, Logger::init);
	}

	static void init() {
		while (!_ins) {
			_ins = new Logger();
		}
	}

	void writeLog(string &log);
	
	void ERROR();
	void WARNING();
	void INFO();
	void DEBUG();

private:
	Logger();

	Buffer *buffer;

	FILE *fd = nullptr;  
	pid_t pid;
	
    //singleton 单例模式
    static Logger *_ins;
	static pthread_once_t _once;

	static string filePath;
	static LOG_LEVEL LogLevel;

};

// 需要注意的是宏函数存在的情况是直接替换 不是函数 注意区分替代的变量名
#define LOG_INIT(_filePath, _level) \
	do { \
		Logger::filePath = _filePath; \
		Logger::LogLevel = _level; \ 
	}while (0)



#endif