#pragma once
#include <map>
#include <string>
#include <mutex>
using namespace std;

namespace settingsSpace{
#define FILE_PATH "e:/test.ini"
	class Settings
	{
	public:
		static Settings* getIntance() { return &instance; }
		string value(const string& _key, const string& defaultValue = string(""));
		bool setValue(const string& _key, const string& _val);

	private:
		Settings(const string& path = string(FILE_PATH)) : fileName(path) {};
		~Settings() {};
		Settings(const Settings&) = delete;
		Settings& operator=(const Settings&) = delete;
		void readSettings();
		void writeSettings();
		void trim(string &s);

	private:
		map<string, map<string, string>> settings;
		string fileName;
		static Settings instance;
		mutex mtx;

	};
}

