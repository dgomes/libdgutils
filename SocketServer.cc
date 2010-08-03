/*
 * SocketServer.cc
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

#include "SocketServer.h"

SocketServer::SocketServer() : Socket()
{
}

SocketServer::SocketServer(Socket::Domain d, Socket::Type t, unsigned int p) : Socket(d,t,p)
{
}

bool SocketServer::Bind(const char *address)
{
	#ifdef USE_LIBBOOST
	boost::mutex::scoped_lock scoped_lock(mutex);
	#endif
	struct sockaddr_un local;
	unsigned len;
	local.sun_family = domain;
	strcpy(local.sun_path, address);
        unlink(local.sun_path);
        len = strlen(local.sun_path) + sizeof(local.sun_family);
	if (bind(sockfd, (struct sockaddr*) &local,len) == -1)
		return false;
	return true;
}


bool SocketServer::Bind(const char *server, const char *port) 
{
	struct addrinfo hints, *res, *res0;
	int error;
	
	#ifdef USE_LIBBOOST
	boost::mutex::scoped_lock scoped_lock(mutex);
	#endif
	memset(&hints, 0, sizeof(hints));
	
	hints.ai_family = UNSPEC;
	hints.ai_socktype = STREAM;
	hints.ai_flags = AI_PASSIVE;
	close(sockfd);
	sockfd=0;
	if((error = getaddrinfo(server, port, &hints, &res0))!=0) throw Socket_Exception((char *)gai_strerror(error));

	for (res = res0; res; res = res->ai_next) {
		if((sockfd=socket(res->ai_family, res->ai_socktype,res->ai_protocol))<0) {
			continue;
		}
		int on = 1;
                setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

		if (bind(sockfd, res->ai_addr, res->ai_addrlen) >= 0) {
			freeaddrinfo(res0);
	        return true;   
		}
	}
    freeaddrinfo(res0);
    throw Socket_Exception(strerror(errno));
    return false;
}

bool SocketServer::Listen(unsigned maxcon) 
{
	#ifdef USE_LIBBOOST
	boost::mutex::scoped_lock scoped_lock(mutex);
	#endif
	if (listen(sockfd, maxcon) < 0) {
		throw Socket_Exception(strerror(errno));        
	}	
	return true;   
}

Socket *SocketServer::Accept() {
   socklen_t sin_size = sizeof(struct sockaddr);
   sockaddr RemoteAddress;
   int newfd;

   ioctl(sockfd,FIONBIO,1);

   if((newfd = accept(sockfd, (struct sockaddr *)&RemoteAddress, &sin_size)) == -1) {
      throw Socket_Exception(strerror(errno));        
   }
   return new Socket(newfd);

}

SocketServer::~SocketServer()
{
	// TODO: put destructor code here
}
