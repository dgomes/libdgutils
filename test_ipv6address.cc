#include <iostream>

#include "ipv6address.h"

using namespace std;

int main(int argc, char *argv[]) {
	try{	
	ipv6address a("2001:690:2380:778f:201:2ff:fe67:3cb9");
	ipv6address ab("2001:690:2380:778f:201:2ff:fe67:3cb8/48");
	ipv6address aa("2001:690:2380:778f::1");
	ipv6address aaa("2001:690:2380:777a::/64");
	lladdress mac("00:11:25:2f:5f:c6");
	ipv6address eui_aaa(aaa,mac);

	cerr << aaa << " + " << mac << endl;
		
	cerr << "EUI64 " << eui_aaa << endl << endl;

	cerr << a << endl;
	cerr << ab << endl;
	cerr << "Network part: " << ab.getNetwork(64) << endl;
	cerr << aa << endl;
	cerr << a << " == " << aa << " = " << (a == aa) << endl;
	cerr << a << " == " << a << " = " << (a == a) << endl;
	cerr << a << " < " << ab << " = " << (a < ab) << endl;
	cerr << a << " > " << ab << " = " << (a > ab) << endl;

	cerr << a.getSolicitedNodeAddress() << endl;
	cerr << a.getNeighborDiscoveryL2Address() << endl;	
	} catch (...) {
		cerr << "unknown error" << endl;
	}
	return 0; 
}
