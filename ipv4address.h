/* 
 * ipv4address.h
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
// File: ipv4address.h
// Created by: Diogo Gomes <dgomes@av.it.pt>
// Created on: Wed Mar 29 15:06:43 2006
//

#ifndef _IPV4ADDRESS_H_
#define _IPV4ADDRESS_H_

extern "C" {
	#include <netdb.h>
	#include <string.h>

	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
}
#include <string>
#include <iostream>
#include <stdexcept>

class ipv4address
{
	public:
		ipv4address();
		ipv4address(std::string addr);
		ipv4address(const struct in_addr addr);
		ipv4address(const ipv4address &);
		ipv4address &operator=(const ipv4address &);
		~ipv4address();
		friend std::ostream &operator<<(std::ostream &stream, ipv4address &ip);
		friend std::ostream &operator<<(std::ostream &stream, const ipv4address &ip);
		const std::string getip();
		const std::string getname();
		ipv4address &operator=(const char *);
		bool operator==(ipv4address &);
		bool operator!=(ipv4address &);
		friend bool operator<(const ipv4address &, const ipv4address &);
		friend bool operator>(const ipv4address &, const ipv4address &);
		void set(const char *);
		operator std::string() {return getip(); };
		const in_addr *get_inet_addr() const {return &address; };
	protected:
		struct in_addr address;
		std::string name;			
	
};

class ipv4address_exception
{
	public:
		ipv4address_exception(int herror,std::string ip) {h_error=herror; ipaddress=ip;};
		~ipv4address_exception() {};
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

#endif	//_IPV4ADDRESS_H_
