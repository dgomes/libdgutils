/* 
 * NetDevice.h
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
#ifndef NETDEVICE_H
#define NETDEVICE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h> 
#include <net/if.h>

#include <iostream>
#include <errno.h>
#include <string>
#include "common.h"
#include "lladdress.h"
#include "ipv4address.h"
#include "ipv6address.h"

extern "C" {
#include <netdb.h>
#include <ifaddrs.h>
#include <netinet/in.h>

#ifdef LINUX
#include <netinet/ether.h>

#include <iproute/libnetlink.h>
#include <iproute/utils.h>
#include <libnetlink.h>

#endif
}

class NetDevice_Exception {
	public:
		NetDevice_Exception() { error_str=""; };
		NetDevice_Exception(const std::string &str) { error_str=str; };
		virtual ~NetDevice_Exception() {};
		const std::string what() { return error_str; };
	private:
		std::string error_str;
};

class NetDevice {
	public:
		NetDevice(const std::string &ifname);
		~NetDevice();
		int getIfIndex() const;
		std::string getIfName() const {return ifname; };
		ipv6address getIPv6Address();
		ipv4address getIPv4Address();
		bool isUp();
		void bringUp();
		void setPromisc();
		void setHWAddress(struct sockaddr &);
		bool setIPv6Address(const ipv6address &ip, unsigned netmask=64);
		bool addNeighbour(lladdress &lla, ipv6address &ip);
		struct sockaddr getNeighbourHWAddress(const ipv6address &ip); 
		struct sockaddr getHWAddress();
	private:
		void call_ioctl(struct ifreq &ifr, int request) const;
		std::string ifname;
		int skfd;
};

#endif
