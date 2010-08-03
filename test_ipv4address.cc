#include <iostream>

#include "ipv4address.h"

using namespace std;

int main(int argc, char *argv[]) {
	try{	
	ipv4address a("127.0.0.1");
	ipv4address aa("127.0.0.2");
	
	cerr << a << endl;
	cerr << a << " == " << aa << " = " << (a == aa) << endl;
	cerr << a << " < " << aa << " = " << (a < aa) << endl;
	
	} catch (...) {
		cerr << "unknown error" << endl;
	}
	return 0; 
}
