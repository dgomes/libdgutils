//
// File: ipv6address.h
// Created by: Diogo Gomes <etdgomes@ua.pt>
// Created on: Tue May  6 15:06:43 2003
//

#include "ipv6address.h"

#include <iostream>
using std::cerr;
using std::endl;

void ipv6address::set(const char *addr) {
	/*
	struct hostent hostbuf, *hp;
	char *tmphstbuf;
	int res;
	int herr;
	int hstbuflen =1024;
	
	tmphstbuf = new char(hstbuflen);
	cerr << "*" << endl;
  	while ((res = gethostbyname2_r (addr.c_str(), AF_INET6, &hostbuf, tmphstbuf, hstbuflen, &hp, &herr)) == ERANGE)
    {
		// Enlarge the buffer. 
		cerr << "." << endl;
		hstbuflen *= 2;
		delete tmphstbuf;
		tmphstbuf = new char(hstbuflen);
	}
	cerr << "#" << endl;
  	//  Check for errors.  
  	if (res || hp == NULL)
    	throw ipv6address_exception(herr,addr);
  	
	address=*(struct in6_addr *)hp->h_addr_list[0];
	*/
	struct hostent *hp;	
		
	if((hp=gethostbyname2(addr,AF_INET6))==NULL)
		throw ipv6address_exception(h_errno,addr);
	
	address=*(struct in6_addr *)hp->h_addr_list[0];
	name = hp->h_name;
}

ipv6address &ipv6address::operator=(const char *addr) {
	set(addr);
	return *this;
}

bool ipv6address::operator==(const ipv6address &o) const{
	if(IN6_ARE_ADDR_EQUAL(&address,&o.address)) return true;
	return false;
}

bool ipv6address::operator!=(const ipv6address &o) const{
	if(!IN6_ARE_ADDR_EQUAL(&address,&o.address)) return true;
	return false;
}

bool operator<(const ipv6address &i, const ipv6address &o) {
	for(int ind = 0 ; ind < 16 ; ind++) {
		if( ntohs(i.address.s6_addr[ind]) < ntohs(o.address.s6_addr[ind]))
			return true;
		if( ntohs(i.address.s6_addr[ind]) > ntohs(o.address.s6_addr[ind]))
			return false;
	}
		 
        return false;
}

bool operator>(const ipv6address &i, const ipv6address &o) {
	for(int ind = 0 ; ind < 16 ; ind++) {
		if( ntohs(i.address.s6_addr[ind]) > ntohs(o.address.s6_addr[ind]))
			return true;
		if( ntohs(i.address.s6_addr[ind]) < ntohs(o.address.s6_addr[ind]))
			return false;
	}
	
        return false;
}

ipv6address::ipv6address(const std::string addr)
{
	if(addr.find("/")==addr.npos) {
		set(addr.c_str());
		network_mask = 128;
	} else {
		set(addr.substr(0,addr.find("/")).c_str());
		network_mask = atoi(addr.substr(addr.find("/")+1).c_str()); 
	}
	name="";
	
}

ipv6address::ipv6address(const struct in6_addr addr, unsigned int net_mask)
{
	memcpy(&address,&addr,sizeof(in6_addr));
	name="";
	network_mask=net_mask;
}

ipv6address::ipv6address() 
{
	struct in6_addr anyaddr = IN6ADDR_ANY_INIT;
	memcpy(&address,&anyaddr,sizeof(in6_addr));
	name="any";
	network_mask=128;
}

ipv6address::ipv6address(const ipv6address &ip)
{
	memcpy(&address,&ip.address,sizeof(in6_addr));
	name = ip.name;
	network_mask = ip.network_mask;
}

ipv6address &ipv6address::operator=(const ipv6address &ip)
{
	memcpy(&address,&ip.address,sizeof(in6_addr));
	name = ip.name;
	network_mask = ip.network_mask;
	return *this;
}

ipv6address::~ipv6address()
{
}

const std::string ipv6address::getIP() const {
	char ip6[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, (void *) &address,ip6,INET6_ADDRSTRLEN);
	return ip6;
}

const std::string ipv6address::getName() {
	char ip6[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, (void *) &address,ip6,INET6_ADDRSTRLEN);
	if(name=="") {
		struct hostent *hp;	
		if((hp=gethostbyname2(ip6,AF_INET6))==NULL)
			throw ipv6address_exception(h_errno,ip6);
		name = hp->h_name;
	}
	return name;
}

ipv6address ipv6address::getSolicitedNodeAddress() const {
	ipv6address sol_node_addr("FF02:0:0:0:0:1:FF00:0000");
	sol_node_addr.address.s6_addr[13] = address.s6_addr[13];
	sol_node_addr.address.s6_addr[14] = address.s6_addr[14];
	sol_node_addr.address.s6_addr[15] = address.s6_addr[15];
	return sol_node_addr;
}

ipv6address ipv6address::getNetwork(unsigned prefix_len) const {
	ipv6address network("::");
	unsigned pref_len = prefix_len/8;
	unsigned pref_len_bits = prefix_len%8;
	
	for(unsigned i = 0; i<pref_len; i++)
		network.address.s6_addr[i] = address.s6_addr[i];
	
	if(pref_len<16)
	network.address.s6_addr[pref_len] = address.s6_addr[pref_len] & pref_len_bits;
	
	return network;
}

ipv6address::ipv6address(ipv6address network, lladdress mac) {
	//network part always 64 bits long
	for(unsigned i = 0; i<8; i++)
		address.s6_addr[i] = network.address.s6_addr[i];
	//EUI-64 address
	address.s6_addr[8] = ((char *)mac.getLLAddr())[0] ^ 2; // 2 (0b10) is universal/local bit + individual/group bit
	address.s6_addr[9] = ((char *)mac.getLLAddr())[1];
	address.s6_addr[10] = ((char *)mac.getLLAddr())[2];
	address.s6_addr[11] = 0xff;
	address.s6_addr[12] = 0xfe;
	address.s6_addr[13] = ((char *)mac.getLLAddr())[3];
	address.s6_addr[14] = ((char *)mac.getLLAddr())[4];
	address.s6_addr[15] = ((char *)mac.getLLAddr())[5];
	network_mask = 64;
	name = "";
	
}

lladdress ipv6address::getNeighborDiscoveryL2Address() const {
	std::stringstream s;
	s << "33:33:FF:";
	s << std::hex 
	<< (int) (address.s6_addr[13]) << ":" 
	<< (int) (address.s6_addr[14]) << ":" 
	<< (int) (address.s6_addr[15]);
	return lladdress(s.str().c_str());
}

std::ostream &operator<<(std::ostream &o, ipv6address &ip) {
	char ip6[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, (void *) &ip.address,ip6,INET6_ADDRSTRLEN);
	if(ip.name=="") {
		struct hostent *hp;	
		if((hp=gethostbyname2(ip6,AF_INET6))==NULL)
			throw ipv6address_exception(h_errno,ip6);
		ip.name = hp->h_name;
	}
	if(ip.network_mask!=128)	
		o << ip.name << "(" << ip6 << "/" << ip.network_mask <<")";
	else
		o << ip.name << "(" << ip6 << ")";
	return o;
}

std::ostream &operator<<(std::ostream &o, const ipv6address &ip) {
	char ip6[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, (void *) &ip.address,ip6,INET6_ADDRSTRLEN);
	
	if(ip.network_mask!=128)	
	o << ip6 << "/" << ip.network_mask ;
	else
	o << ip6;
	return o;
}

