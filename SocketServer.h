/*
 * SocketServer.h
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

#ifndef _SOCKETSERVER_H_
#define _SOCKETSERVER_H_

#include "Socket.h"
#include <sys/types.h>
#include <sys/un.h>
#include <sys/ioctl.h>

class SocketServer : public Socket
{
	public:
		SocketServer();
		SocketServer(Socket::Domain , Socket::Type, unsigned int );
		 ~SocketServer();
		bool Bind(const char *server, const char *port);
		bool Bind(const char *address);
		bool Listen(unsigned maxcon=SOMAXCONN);
		Socket *Accept();
		
	protected:
};


#endif	//_SOCKETSERVER_H_
