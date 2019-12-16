#include <iostream>
#include "Setting.h"

int main(int argc, char **argv)
{
	settingsSpace::Setting s;
	s.setValue("1/1.1", "a");
	s.setValue("1/1.2", "a");
	s.setValue("2/2.1", "a");
	s.setValue("0.1", "a");

	cout << "1"<<s.value("1/1.1")<<"2" << s.value("1/1.2")<<"3" << s.value("2/2.1")<<"4" << s.value("0.1")<<endl;
	system("pause");
	return 0;
}