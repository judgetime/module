#pragma once
#include <map>
#include <string>
using namespace std;

namespace settingsSpace{
#define FILE_PATH "e:/test.ini"
	class Setting
	{
	public:
		Setting(const string& path = string(FILE_PATH));
		~Setting();
		string value(const string& _key, const string& defaultValue = string(""));
		bool setValue(const string& _key, const string& _val);

	private:
		void readSettings();
		void writeSettings();
		void trim(string &s);

	private:
		map<string, map<string, string>> settings;
		string fileName;
	};
}

