#include <iostream>

#include "SocketClient.h"

using namespace std;

int main(int argc, char *argv[]) {
try{	
	SocketClient c(Socket::INET6,Socket::DGRAM, 0);
	c.Connect("2001:690:2380:777e:20e:9bff:fe7c:1e4b","8080");

	string msg;
	do {
	cout << "type msg: ";
	getline(cin,msg); 
	c.Send(msg);
	} while(msg!="quit");
	c.Close();
	return 0; 
} catch (Socket_Exception e) {
	cerr << "teste: " << e.what() << endl;
} catch (...) {
	cerr << "unknown error" << endl;
}

}
