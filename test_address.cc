#include <iostream>

#include "lladdress.h"

using namespace std;

int main(int argc, char *argv[]) {
	try{	
	lladdress a("00:ff:a1:b2:c3:d4");
	lladdress b("00:ff:ab:b2:c3:d5");
	lladdress c("00:ff:a1:b2:c3:d5");
	lladdress aa("00:ff:a1:b2:c3:d4");
	
	cerr << a << endl;
	cerr << b << endl;
	cerr << c << endl;
	cerr << a << " == " << aa << " = " << (a == aa) << endl;
	cerr << a << " < " << b << " = " << (a < b) << endl;
	cerr << c << " > " << b << " = " << (c > b) << endl;
	
	} catch (...) {
		cerr << "unknown error" << endl;
	}
	return 0; 
}
