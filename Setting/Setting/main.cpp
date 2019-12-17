#include <iostream>
#include "Settings.h"
#include "Logger.h"

using namespace settingsSpace;

int main(int argc, char **argv)
{
	loggerSpace::Logger *l = loggerSpace::Logger::getInstance();
	l->setOutputFile(true);
	string s("--");
	l->logPrint("abc %s %s \n", "bcd", s.c_str());
	
	system("pause");
	return 0;
}