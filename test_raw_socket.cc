#include <iostream>

#include "Socket.h"
#include "ipv6address.h"

using namespace std;

int main(int argc, char *argv[]) {
try{	
	cerr << "test" << endl;	
	Socket r(Socket::PACKET, Socket::DGRAM, ETH_P_ALL,4);
	struct sockaddr_ll addr;
	addr.sll_addr[0] = (unsigned char) 0xff;
	addr.sll_addr[1] = (unsigned char) 0xff;
	addr.sll_addr[2] = (unsigned char) 0xff;
	addr.sll_addr[3] = (unsigned char) 0xff;
	addr.sll_addr[4] = (unsigned char) 0xff;
	addr.sll_addr[5] = (unsigned char) 0xff;
	addr.sll_addr[6] = (unsigned char) 0x0;
	addr.sll_addr[7] = (unsigned char) 0x0;

	//r.Bind(ETH_P_IPV6,3);

	cerr << "Receive:" << endl;
	do {
		char buffer[1500];
		unsigned t = 0;
		struct sockaddr_ll ll;
		t = r.ReceiveFrom(buffer,1500,(struct sockaddr *) &ll);
		if (ll.sll_pkttype == PACKET_OUTGOING || ll.sll_pkttype == PACKET_MULTICAST) {
			continue;
		} else {
			cerr << (unsigned) ll.sll_pkttype << endl;
		}
		ip6_hdr *hdr = (ip6_hdr *) buffer;
		
        if(hdr->ip6_vfc == 0x60 && hdr->ip6_nxt==0x06) {
	        cerr << "len: " << ntohs(hdr->ip6_plen) << endl;
 	        cerr << "Protocol: " << (unsigned short int) hdr->ip6_nxt << endl; 
        	cerr << "hlim: " << (unsigned short int) hdr->ip6_hlim << endl;

	        ipv6address src(hdr->ip6_src);
	        ipv6address dst(hdr->ip6_dst);
		cerr << src << " -> " << dst << endl;

	};



	} while (1);
	
	cerr << "received..." << endl;
} catch (ipv6address_exception e) {
	cerr << "ipv6address: " << e.what() << endl;
} catch (Socket_Exception e) {
	cerr << "socket: " << e.what() << endl;
} catch (...) {
	cerr << "unknown error" << endl;
}
	return 0; 

}
