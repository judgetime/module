/*
 * 日志记录类
 */
#pragma once
#include <iostream>
#include <string>
#include <mutex>
#include <vector>

using namespace std;
namespace loggerSpace {
#define LOGFILE_PATH "e:/test.log"
	typedef enum {
		INFO,
		WARNING,
		ERROR,
		FATAL
	}LogRank;
	
	class Logger
	{
	public:
		void setFileName(const string& filename) { fileName = filename; }
		void setThresholdRank(const LogRank _rank = INFO) { thresholdRank = _rank; }
		void setOutputFile(const bool flag = false) { outputFile = flag; }
		void logPrint(const char *fmt, ...);
		static Logger* getInstance() { return &instance; }


	private:
		Logger(const string& path = string(LOGFILE_PATH));
		~Logger() {};
		string getCurrentTime();

	private:
		static Logger instance;
		string fileName;
		mutex mtx;
		LogRank currentRank = INFO;
		LogRank thresholdRank = INFO;
		vector<string> logRankString;
		bool outputFile = false;
	};
}

