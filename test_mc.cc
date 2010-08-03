#include <iostream>
#include <string.h>

#include "NetDevice.h"
#include "SocketMulticast.h"

using namespace std;

int main(int argc, char *argv[]) {
try{	

	if(argc<4) { 
		cout << "Usage: " << argv[0] << " s|c [source/]group port [interface]" <<endl;
		cout << "Example: " << argv[0] << " s ff1e::1 4040" <<endl;
		exit(1);
	}
	
	SocketMulticast m;
	char src[1024];
	char *grp;
	strcpy(src,strtok(argv[2],"/"));
	if((grp = strtok(NULL,"/"))) {
		cout << "Group: " << grp << "\tSource: " << src << endl;	
	} else {	
		grp = src;	
		cout << "Group: " << grp << endl;	
	}

	m.createSocket(grp,argv[3]);

	if(argc>4) {
		NetDevice n(argv[4]);
		m.bindInterface(n);
	}
	string msg;
	if(strcmp(argv[1],"s")==0) {
		do {
			cout << "type msg: ";
			getline(cin,msg); 
			m.Send(msg);
		} while(msg != "kick");
	} else {
		cerr << "receiving" << endl;

		if(strcmp(src,grp)) {
			cout << "SSM Mode" << endl;
			m.joinSourceGroup(src,grp,argv[3]);
		} else {	
			cout << "ASM Mode" << endl;
			m.joinGroup(grp,argv[3]); 
		}


		do {
			m.Receive(msg,1500);
			cerr << ": " << msg << endl;
		} while(msg!="kick");
	}
} catch (Socket_Exception e) {
	cerr << "teste: " << e.what() << endl;
} catch (...) {
	cerr << "unknown error" << endl;
}
	return 0; 

}
