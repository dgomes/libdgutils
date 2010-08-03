#include <iostream>

#include "NetDevice.h"
#include "lladdress.h"
#include "ipv6address.h"

using namespace std;

int main(int argc, char *argv[]) {
	try{	
		NetDevice nd("eth0");
/*		cerr << nd.getIfIndex() << endl;
		cerr << nd.isUp() << endl;
		cerr << nd.setIPv6Address(ipv6address("::2"));
		cerr << "Address: "<< nd.getIPv6Address() << endl;	
	*/
	struct sockaddr neigh_addr;
	neigh_addr = nd.getNeighbourHWAddress(ipv6address("fe80::2a0:c9ff:fe1c:c9d4"));
    lladdress neigh_mac((unsigned char *)neigh_addr.sa_data);
	cerr << neigh_mac << endl;
	} catch (NetDevice_Exception e) {
		cerr << e.what() << endl;
	}
	return 0; 
}
