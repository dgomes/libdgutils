/*
 * SocketMulticast.h
 *
 * Copyright (C) 2004 Universidade Aveiro - Instituto de Telecomunicacoes Polo Aveiro 
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
 * Authors:	Diogo Gomes, <dgomes@av.it.pt>
 */

#ifndef _SOCKETCLIENT_H_
#define _SOCKETCLIENT_H_

#include "Socket.h"
#include "NetDevice.h"
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>

class SocketMulticast : public Socket
{
	public:
		SocketMulticast();
		 ~SocketMulticast();
		bool createSocket(const char *group, const char *port, int family=AF_UNSPEC, int socktype=SOCK_DGRAM); 
		bool joinSourceGroup(const char *source,const char *group, const char *service, int family=AF_UNSPEC, int socktype=SOCK_DGRAM);
		bool joinGroup(const char *group, const char *service, int family=AF_UNSPEC, int socktype=SOCK_DGRAM);
		bool leaveGroup(const char *hostname, const char *service);
		bool bindInterface(const NetDevice &ifindex);
		bool hopLimit(int hops);
		bool loopbackDelivery(bool flag=true);
		int isMulticast(struct sockaddr_storage *addr);
		int Send(std::string buffer);	
		int Receive(std::string &buffer, unsigned size);	
		int Send(const void *buffer, unsigned size);	
		int Receive(void *buffer, unsigned size);	
	protected:
		int ifindex;
		struct sockaddr_storage addr;
	private:
};


#endif	//_SOCKETCLIENT_H_
