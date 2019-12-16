#include <iostream>
#include <fstream>
#include <cstdint>
#include "Setting.h"

using namespace settingsSpace;

Setting::Setting(const string &path) : fileName(path)
{
}


Setting::~Setting()
{
}

void Setting::readSettings()
{
	ifstream infile(fileName);
	string line, section, key, val;
	size_t pos = 0, oldStart = 0, oldEnd = 0, newStart = 0, newEnd = 0;
	map<string, string> k_v;
	while (getline(infile, line)) {
		if (line.empty()) continue;
		pos = line.find('#');
		line = line.substr(0, pos+1);
		if (line.empty()) continue;
		if ((oldStart = line.find('[')) != string::npos && (oldEnd = line.find(']')) != string::npos) {
			section = line.substr(oldStart, oldEnd - oldStart);
		}
		if (1 == line.size() - oldEnd) continue;
		line = line.substr(oldEnd, line.size() - oldEnd);
		if ((pos = line.find('=')) != string::npos) {
			key = line.substr(0, pos);
			val = line.substr(pos + 1, line.size() - pos - 1);
			k_v[key] = val;
			settings[section] = k_v;
		}
	}
	infile.close();
}

void Setting::writeSettings()
{
	ofstream outfile(fileName, ios::out | ios::trunc);
	for (auto i : settings) {
		outfile << "[" << i.first << "]\n";
		for (auto j : i.second) {
			outfile << j.first << "=" << j.second << "\n";
		}
	}
	outfile.close();
}

void Setting::trim(string &s)
{
	if (!s.empty())
	{
		s.erase(0, s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
	}
}

string Setting::value(const string &_key, const string &defaultValue)
{
	string str = _key;
	string section, key;
	trim(str);
	if (str.empty()) return string("");
	size_t pos = str.find('/');
	if (pos == string::npos) {
		section = "General";
		key = str;
	} else {
		section = str.substr(0, pos);
		key = str.substr(pos + 1, str.size() - pos - 1);
	}

	auto iter = settings.find(section);
	if (iter != settings.end()) {
		auto kv_iter = iter->second.find(key);
		if (kv_iter != iter->second.end()) {
			return kv_iter->second;
		}
	}
	return defaultValue;
}

bool Setting::setValue(const string& _key, const string& _val)
{
	string str = _key;
	string section, key;
	size_t pos;
	trim(str);
	if (str.empty()) return false;
	pos = str.find('/');
	if (pos == string::npos) {
		section = "General";
		key = str;
	}
	else {
		section = str.substr(0, pos);
		key = str.substr(pos + 1, str.size() - pos - 1);
	}

	auto iter = settings.find(section);
	if (iter == settings.end()) {
		map<string, string> k_v;
		k_v.insert(pair<string, string>(key, _val));
		settings.insert(pair<string,map<string,string>>(section,k_v));
	} else {
		iter->second.insert(map<string, string>::value_type(key, _val));		
	}
	writeSettings();
	return true;
}




