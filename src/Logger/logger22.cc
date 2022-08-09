#include "logger22.h"

LOG*             LOG::Log         = NULL;
string           LOG::logBuffer   = "";
HANDLE           LOG::mFileHandle = INVALID_HANDLE_VALUE;
mutex            LOG::log_mutex;
 
LOG::LOG()
{
	// 初始化
	init(LOG_LEVEL_NONE, LOG_TARGET_FILE);
} 
 
void LOG::init(LOGLEVEL loglevel, LOGTARGET logtarget)
{
	setLogLevel(loglevel);
	setLogTarget(logtarget);
	createFile();
}
 
void LOG::uninit()
{
	if (INVALID_HANDLE_VALUE != mFileHandle)
	{
		CloseHandle(mFileHandle);
	}
}
 
LOG* LOG::getInstance()
{
	if (NULL == Log)
	{
		log_mutex.lock();
		if (NULL == Log)
		{
			Log = new LOG();
		}
		log_mutex.unlock();
	}
	return Log;
}
 
LOGLEVEL LOG::getLogLevel()
{
	return this->logLevel;
}
 
void LOG::setLogLevel(LOGLEVEL iLogLevel)
{
	this->logLevel = iLogLevel;
}
 
LOGTARGET LOG::getLogTarget()
{
	return this->logTarget;
}
 
void LOG::setLogTarget(LOGTARGET iLogTarget)
{
	this->logTarget = iLogTarget;
}

int LOG::createFile()
{
	TCHAR fileDirectory[256];
	GetCurrentDirectory(256, fileDirectory);
 
	// 创建log文件的路径
	TCHAR logFileDirectory[256];
	_stprintf_s(logFileDirectory, _T("%s\\Test\\"), fileDirectory);// 使用_stprintf_s需要包含头文件<TCHAR.H>
 
	// 文件夹不存在则创建文件夹
	if (_taccess(logFileDirectory, 0) == -1)
	{
		_tmkdir(logFileDirectory);
	}
 
	TCHAR cTmpPath[MAX_PATH] = { 0 };
	TCHAR* lpPos = NULL;
	TCHAR cTmp = _T('\0');
 
	WCHAR pszLogFileName[256];
	// wcscat:连接字符串
	wcscat(logFileDirectory, _T("test.log"));
	_stprintf_s(pszLogFileName, _T("%s"), logFileDirectory);
	mFileHandle = CreateFile(
		pszLogFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == mFileHandle)
	{
		return -1;
	}
	return 0;
}

int LOG::writeLog(
	LOGLEVEL loglevel,         // Log级别
	unsigned char* fileName,   // 函数所在文件名
	unsigned char* function,   // 函数名
	int lineNumber,            // 行号
	char* format,              // 格式化
	...)
{
	int ret = 0;
 
	// 获取日期和时间
	char timeBuffer[100];
	ret = getSystemTime(timeBuffer);
	logBuffer += string(timeBuffer);
 
	// LOG级别
	char* logLevel;
	if (loglevel == LOG_LEVEL_DEBUG){
		logLevel = "DEBUG";
	}
	else if (loglevel == LOG_LEVEL_INFO){
		logLevel = "INFO";
	}
	else if (loglevel == LOG_LEVEL_WARNING){
		logLevel = "WARNING";
	}
	else if (loglevel == LOG_LEVEL_ERROR){
		logLevel = "ERROR";
	}
 
	// [进程号][线程号][Log级别][文件名][函数名:行号]
	char locInfo[100];
	char* format2 = "[PID:%4d][TID:%4d][%s][%-s][%s:%4d]";
	ret = printfToBuffer(locInfo, 100, format2,
		GetCurrentProcessId(),
		GetCurrentThreadId(),
		logLevel,
		fileName,
		function,
		lineNumber);
	logBuffer += string(locInfo);	
 
	// 日志正文
	char logInfo2[256];
	va_list ap;
	va_start(ap, format);
	ret = vsnprintf(logInfo2, 256, format, ap);
	va_end(ap);
 
	logBuffer += string(logInfo2);
	logBuffer += string("\n");
 
	outputToTarget();
 
	return 0;
}

void LOG::outputToTarget()
{
	if (LOG::getInstance()->getLogTarget() & LOG_TARGET_FILE)
	{
		SetFilePointer(mFileHandle, 0, NULL, FILE_END);
		DWORD dwBytesWritten = 0;
		WriteFile(mFileHandle, logBuffer.c_str(), logBuffer.length(), &dwBytesWritten, NULL);
		FlushFileBuffers(mFileHandle);
	}
	if (LOG::getInstance()->getLogTarget() & LOG_TARGET_CONSOLE)
	{
		printf("%s", logBuffer.c_str());
	}
 
	// 清除buffer
	logBuffer.clear();
}

int main() {
    LOG logger;
    return 0;
}
