#include <iostream>

#include "SocketServer.h"

using namespace std;

int main(int argc, char *argv[]) {
try{	
	SocketServer s(Socket::INET6,Socket::DGRAM, 0);
	s.Bind("::1","1234");
	s.Listen();

	Socket *c = s.Accept();

	string msg;
	do {
	cout << "type msg: ";
	getline(cin,msg); 
	c->Send(msg);
	} while(msg!="quit");
	c->Close();
	return 0; 
} catch (Socket_Exception e) {
	cerr << "teste: " << e.what() << endl;
} catch (...) {
	cerr << "unknown error" << endl;
}

}
