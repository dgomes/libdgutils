#include <iostream>
using namespace std;
#include "Configuration.h"

void print(const string &s) {
	cerr << s << endl;
}

int main(void) {
	configuration c("test.cfg");
	cout << c["test"].value("default") << endl;
	cout << c["test2"]["last"].value("default") << endl;
	cout << c["test2"]["last"]["1"].value("default") << endl;
	cout << c["test2"]["last"]["2"].value("default") << endl;
	cout << " ------- " << endl;
	c["test2"]["last"].foreach(print);
	return EXIT_SUCCESS;

}
