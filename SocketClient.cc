/*
 * SocketClient.cc
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

#include "SocketClient.h"

SocketClient::SocketClient() : Socket()
{
}

SocketClient::SocketClient(Socket::Domain d, Socket::Type t, unsigned int p) : Socket(d,t,p)
{
}

bool SocketClient::Connect(const char *server, char *port) 
{
	#ifdef USE_LIBBOOST
	boost::mutex::scoped_lock scoped_lock(mutex);
	#endif
	struct addrinfo hints, *res, *res0;
	int error;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = UNSPEC;
	hints.ai_socktype = type;
	if((error = getaddrinfo(server, port, &hints, &res0))!=0) throw Socket_Exception((char *)gai_strerror(error));

	close(sockfd);
	for (res = res0; res; res = res->ai_next) {
		if((sockfd=socket(res->ai_family, res->ai_socktype,res->ai_protocol))<0) {
			continue;
		}
	        if (connect(sockfd, res->ai_addr, res->ai_addrlen) >= 0) {
			freeaddrinfo(res0);
	        return true;
		}
	}
	throw Socket_Exception(strerror(errno));
	freeaddrinfo(res0);
        return false;
}

bool SocketClient::Connect(const char *address)
{
	#ifdef USE_LIBBOOST
        boost::mutex::scoped_lock scoped_lock(mutex);
        #endif
	struct sockaddr_un local;
        unsigned len;
        local.sun_family = domain;
        strcpy(local.sun_path, address);
        len = strlen(local.sun_path) + sizeof(local.sun_family);
        if (connect(sockfd, (struct sockaddr*) &local,len) == -1)
                return false;
	return true;
}

SocketClient::~SocketClient()
{
}
