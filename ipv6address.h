/* 
 * ipv6address.h
 *
 * Copyright (C) 2004 Universidade Aveiro - Instituto de Telecomunicacoes Polo A
veiro
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:     Diogo Gomes, <dgomes@av.it.pt>
 */
//
// File: ipv6address.h
// Created by: Diogo Gomes <etdgomes@ua.pt>
// Created on: Tue May  6 15:06:43 2003
//

#ifndef _IPV6ADDRESS_H_
#define _IPV6ADDRESS_H_

extern "C" {
	#include <netdb.h>
	#include <string.h>	
	#include <stdlib.h>

	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
}
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "lladdress.h"

class ipv6address
{
	public:
		ipv6address();
		ipv6address(const std::string addr);
		ipv6address(const struct in6_addr addr, unsigned network_mask=128);
		ipv6address(const ipv6address &);
		ipv6address(ipv6address network, lladdress mac);
		ipv6address &operator=(const ipv6address &);
		~ipv6address();
		void defineNetmask(unsigned netmask) {network_mask = netmask;};	
		
		const std::string getIP() const;
		const std::string getName();
		const unsigned int getNetworkMask() const {return network_mask;};
		operator std::string() {return getIP(); };
		const in6_addr *get_in6_addr() const {return &address; };

		ipv6address &operator=(const char *);
		bool operator==(const ipv6address &) const;
		bool operator!=(const ipv6address &) const;
		friend bool operator<(const ipv6address &, const ipv6address &);
		friend bool operator>(const ipv6address &, const ipv6address &);

		ipv6address getSolicitedNodeAddress() const;
		ipv6address getNetwork(unsigned prefix_len) const;
		lladdress getNeighborDiscoveryL2Address() const;
		
		friend std::ostream &operator<<(std::ostream &stream, ipv6address &ip);
		friend std::ostream &operator<<(std::ostream &stream, const ipv6address &ip);
	protected:
		unsigned int network_mask;
		struct in6_addr address;
		std::string name;			
		void set(const char *);
	
};

class ipv6address_exception
{
	public:
		ipv6address_exception(int herror,std::string ip) {h_error=herror; ipaddress=ip;};
		~ipv6address_exception() {};
		std::string what() {
			switch(h_error) {
				case HOST_NOT_FOUND: 
					return ipaddress+": The specified host is unknown.";
				case NO_ADDRESS:
					return ipaddress+": The requested name is valid but does not have an IP address.";
				case NO_RECOVERY:
					return ipaddress+": A non-recoverable name server error occurred.";
				case TRY_AGAIN: 
					return ipaddress+": A temporary error occurred on an authoritative name server.  Try again later.";
				default:
					return ipaddress+": Unknown error";
			}
		}
	private:
		int h_error;
		std::string ipaddress;
};

#endif	//_IPV6ADDRESS_H_
