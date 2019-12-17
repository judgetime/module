#include "Logger.h"
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <fstream>

using namespace loggerSpace;
Logger Logger::instance;
enum index { underflow, overflow };

Logger::Logger(const string& path) : fileName(path)
{
	logRankString.emplace_back("Info");
	logRankString.emplace_back("Warning");
	logRankString.emplace_back("Error");
	logRankString.emplace_back("Fatal");
}

void Logger::logPrint(const char* fmt, ...)
{
	try {
		if (currentRank < thresholdRank) return;
		lock_guard<mutex> lck(mtx);
		FILE* fp = fopen(fileName.c_str(), "w+");
		if (nullptr == fp) {
			perror("logfile open failed!");
			exit(1);
		}
		int rank = currentRank;
		string str1 = logRankString[rank];
		string str2 = getCurrentTime();
		fprintf(fp, "%s: %s ", str1.c_str(), str2.c_str());
		va_list ap;
		va_start(ap, fmt);
		if (!outputFile) vfprintf(stdout, fmt, ap);
		else			vfprintf(fp, fmt, ap);
		va_end(ap);
		fclose(fp);
	}
	catch (index e)
	{
		if (e == underflow)
		{
			cout << "index underflow!" << endl;
			exit(-1);
		}
		if (e == overflow)
		{
			cout << "index overflow!" << endl;
			exit(-1);
		}
	}
}

string Logger::getCurrentTime()
{
	time_t tm;
	time(&tm);
	return string(ctime(&tm));
}


