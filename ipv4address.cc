//
// File: ipv4address.h
// Created by: Diogo Gomes <dgomes@av.it.pt>
// Created on: Wed Mar 29 15:06:43 2006
//

#include "ipv4address.h"

#include <iostream>
using std::cerr;
using std::endl;

void ipv4address::set(const char *addr) {
	/*
	struct hostent hostbuf, *hp;
	char *tmphstbuf;
	int res;
	int herr;
	int hstbuflen =1024;
	
	tmphstbuf = new char(hstbuflen);
	cerr << "*" << endl;
  	while ((res = gethostbyname2_r (addr.c_str(), AF_INET, &hostbuf, tmphstbuf, hstbuflen, &hp, &herr)) == ERANGE)
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
    	throw ipv4address_exception(herr,addr);
  	
	address=*(struct in6_addr *)hp->h_addr_list[0];
	*/
	struct hostent *hp;	
		
	if((hp=gethostbyname2(addr,AF_INET))==NULL)
		throw ipv4address_exception(h_errno,addr);
	
	address=*(struct in_addr *)hp->h_addr_list[0];
	name = hp->h_name;
}

ipv4address &ipv4address::operator=(const char *addr) {
	set(addr);
	return *this;
}

bool ipv4address::operator==(ipv4address &o) {
	if(getip()==o.getip()) return true;
	return false;
}

bool ipv4address::operator!=(ipv4address &o) {
	if(getip()!=o.getip()) return true;
	return false;
}

bool operator<(const ipv4address &i, const ipv4address &o) {
        if( ntohl(i.address.s_addr) < ntohl(o.address.s_addr))
                return true;
      /*  if( ntohl(i.address.s_addr32) > ntohl(o.address.s_addr))
                return false;*/
        return false;
}

bool operator>(const ipv4address &i, const ipv4address &o) {
        if( ntohl(i.address.s_addr) > ntohl(o.address.s_addr))
                return true;
        /*if( ntohl(i.address.s_addr) < ntohl(o.address.s_addr))
                return false; */
        return false;
}

ipv4address::ipv4address(std::string addr)
{
	set(addr.c_str());
	name="";
}

ipv4address::ipv4address(const struct in_addr addr)
{
	memcpy(&address,&addr,sizeof(in_addr));
	name="";
}

ipv4address::ipv4address() 
{
	struct in6_addr anyaddr = IN6ADDR_ANY_INIT;
	memcpy(&address,&anyaddr,sizeof(in6_addr));
	name="any";
}

ipv4address::ipv4address(const ipv4address &ip)
{
	memcpy(&address,&ip.address,sizeof(in6_addr));
	name = ip.name;
}

ipv4address &ipv4address::operator=(const ipv4address &ip)
{
	memcpy(&address,&ip.address,sizeof(in6_addr));
	name = ip.name;
	return *this;
}

ipv4address::~ipv4address()
{
}

const std::string ipv4address::getip() {
	char ip6[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, (void *) &address,ip6,INET_ADDRSTRLEN);
	return ip6;
}

const std::string ipv4address::getname() {
	char ip4[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, (void *) &address,ip4,INET_ADDRSTRLEN);
	if(name=="") {
		struct hostent *hp;	
		if((hp=gethostbyname2(ip4,AF_INET))==NULL)
			throw ipv4address_exception(h_errno,ip4);
		name = hp->h_name;
	}
	return name;
}

std::ostream &operator<<(std::ostream &o, ipv4address &ip) {
	char ip4[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, (void *) &ip.address,ip4,INET_ADDRSTRLEN);
	if(ip.name=="") {
		struct hostent *hp;	
		if((hp=gethostbyname2(ip4,AF_INET))==NULL)
			throw ipv4address_exception(h_errno,ip4);
		ip.name = hp->h_name;
	}
	
	o << ip.name << "(" << ip << ")";
	return o;
}

std::ostream &operator<<(std::ostream &o, const ipv4address &ip) {
	char ip4[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, (void *) &ip.address,ip4,INET_ADDRSTRLEN);
	
	o << ip4;
	return o;
}

