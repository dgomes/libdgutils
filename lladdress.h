/* 
 * lladdress.h
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
// File: lladdress.h
// Created by: Diogo Gomes <dgomes@av.it.pt>
// Created on: Tue Jan 11 19:06:43 2006
//

#ifndef _LLADDRESS_H_
#define _LLADDRESS_H_

extern "C" {
	#include <sys/types.h>
	#include <net/ethernet.h>
	#include <string.h>
}
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>

class lladdress
{
	public:
		lladdress();
		lladdress(std::string addr);
		lladdress(const u_int8_t addr[ETHER_ADDR_LEN]);
		lladdress(const lladdress &);
		lladdress &operator=(const lladdress &);
		~lladdress();
		friend std::ostream &operator<<(std::ostream &stream, const lladdress &lla);
		friend std::ostream &operator<<(std::ostream &stream, lladdress &lla);
		lladdress &operator=(const char *);
		bool operator==(lladdress &);
		bool operator!=(lladdress &);
		friend bool operator<(const lladdress &, const lladdress &);
		friend bool operator>(const lladdress &, const lladdress &);
		void set(const char *);
		operator std::string() {return "TO BE IMPLEMENTED (lladdress)"; };
		const void *getLLAddr() const {return &address; };
	protected:
		u_int8_t address[ETHER_ADDR_LEN];
	
};

class lladdress_exception
{
	public:
		lladdress_exception(int herror,std::string ip) {h_error=herror; lladdress=ip;};
		~lladdress_exception() {};
		std::string what() {
			switch(h_error) {
				default:
					return lladdress+": Unknown error";
			}
		}
	private:
		int h_error;
		std::string lladdress;
};

#endif	//_LLADDRESS_H_
