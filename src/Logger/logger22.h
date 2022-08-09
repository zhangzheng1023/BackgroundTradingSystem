#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <mutex>

using std::string;
using std::mutex;
using std::HANDLE;

enum LOGLEVEL
{
	LOG_LEVEL_NONE,
	LOG_LEVEL_ERROR,     // error
	LOG_LEVEL_WARNING,   // warning
	LOG_LEVEL_DEBUG,     // debug
	LOG_LEVEL_INFO,      // info	
};

enum LOGTARGET
{
	LOG_TARGET_NONE      = 0x00,
	LOG_TARGET_CONSOLE   = 0x01,
	LOG_TARGET_FILE      = 0x10
};

class LOG
{
public:
 
	// 初始化
	void init(LOGLEVEL loglevel, LOGTARGET logtarget);
 
	// 
	void uninit();
 
	// file
	int createFile();
 
	static LOG* getInstance();
 
	// Log级别
	LOGLEVEL getLogLevel();
	void setLogLevel(LOGLEVEL loglevel);
 
	// Log输出位置
	LOGTARGET getLogTarget();
	void setLogTarget(LOGTARGET logtarget);
 
	// 打log
	static int writeLog(
		LOGLEVEL loglevel,         // Log级别
		unsigned char* fileName,   // 函数所在文件名
		unsigned char* function,   // 函数名
		int lineNumber,            // 行号
		char* format,              // 格式化
		...);                      // 变量
 
	// 输出log
	static void outputToTarget();
 
private:
	LOG();
	~LOG();
	static LOG* Log;
 
	// 互斥锁
	static mutex log_mutex;
 
	// 存储log的buffer
	static string logBuffer;
 
	// Log级别
	LOGLEVEL logLevel;
 
	// Log输出位置
	LOGTARGET logTarget;
 
	// Handle
	static HANDLE mFileHandle;
};



#endif