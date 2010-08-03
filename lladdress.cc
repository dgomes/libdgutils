//
// File: lladdress.h
// Created by: Diogo Gomes <etdgomes@ua.pt>
// Created on: Tue May  6 15:06:43 2003
//

#include "lladdress.h"

#include <iostream>
using std::cerr;
using std::endl;

void lladdress::set(const char taddr[17]) {
        memset(&address,0,ETHER_ADDR_LEN);
        for(unsigned int j=0,i=0; i<17; i+=3,j++) {
                char temp[3] = { (char) NULL, (char) NULL, (char) NULL };
                memcpy(temp,&taddr[i],2);
                std::stringstream t;
                t << temp;
                unsigned  v;
                t >> std::hex >> v;
                address[j] = v;
/*                #ifdef DEBUG
                if(j!=0) cerr << ":";
                cerr << std::hex << (unsigned) address[j];
                #endif //DEBUG */
        };
}

lladdress &lladdress::operator=(const char *addr) {
	set(addr);
	return *this;
}

bool lladdress::operator==(lladdress &o) {
	for(int i=0; i<ETHER_ADDR_LEN; i++) {
		if(address[i]!=o.address[i]) return false;
	}
	return true;
}

bool lladdress::operator!=(lladdress &o) {
	return !(*this==o);
}

bool operator<(const lladdress &i, const lladdress &o) {
	for(int ii=0; ii< ETHER_ADDR_LEN; ii++) {
		if( (unsigned) i.address[ii]> (unsigned) o.address[ii]) return false;
	}
	return true;
}

bool operator>(const lladdress &i, const lladdress &o) {
	for(int ii=0; ii< ETHER_ADDR_LEN; ii++) {
		if( (unsigned) i.address[ii]< (unsigned) o.address[ii]) return false;
	}
	return true;
}

lladdress::lladdress() 
{
}

lladdress::lladdress(std::string addr)
{
	set(addr.c_str());
}

lladdress::lladdress(const u_int8_t addr[ETHER_ADDR_LEN])
{
	memcpy(address,addr,ETHER_ADDR_LEN);
}

lladdress::lladdress(const lladdress &mac)
{
	memcpy(address,mac.address,ETHER_ADDR_LEN);
}

lladdress &lladdress::operator=(const lladdress &mac)
{
	memcpy(address,mac.address,ETHER_ADDR_LEN);
	return *this;
}

lladdress::~lladdress()
{
}

std::ostream &operator<<(std::ostream &o, lladdress &mac) {
	for(int i=0; i< ETHER_ADDR_LEN; i++) {
		if(i!=0) o << ":";
		o << std::hex << (unsigned) mac.address[i];
	}
	o<< std::dec;
	return o;
}

std::ostream &operator<<(std::ostream &o, const lladdress &mac) {
	for(int i=0; i< ETHER_ADDR_LEN; i++) {
		if(i!=0) o << ":";
		o << std::hex << (unsigned) mac.address[i];
	}
	o<< std::dec;
	return o;
}
