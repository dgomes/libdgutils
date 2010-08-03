/*
 * Socket.h
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

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <sys/poll.h>

#ifdef LINUX
	#ifndef __LINUX_IF_PACKET_H
		#include <netpacket/packet.h>
	#endif
#endif

#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#ifdef USE_LIBBOOST
#include <boost/thread/mutex.hpp>
#endif

#include <iostream>

class Socket_Exception
{
	public:
		Socket_Exception(const char *str) {error_msg = str;}
		~Socket_Exception() {};
		const std::string what() {return error_msg; };
	private:
		std::string error_msg;
};

class Socket
{
	public:
	
		enum Domain
		{
			UNIX = PF_UNIX,
			LOCAL = PF_LOCAL,  // Local communication
			INET = PF_INET,		// IPv4 Internet protocols   
			INET6 = PF_INET6,    // IPv6 Internet protocols
#ifdef LINUX
			NETLINK = PF_NETLINK, // Kernel user interface device     
			PACKET = PF_PACKET,   // Low level packet interface       
#endif
			UNSPEC = PF_UNSPEC	// not specified
		};
		
		enum Type
		{
			STREAM = SOCK_STREAM,
			DGRAM = SOCK_DGRAM,
			SEQPACKET = SOCK_SEQPACKET,
			RAW = SOCK_RAW,
			RDM = SOCK_RDM,
#ifdef LINUX
			S_PACKET = SOCK_PACKET
#endif
		};

		Socket() throw (Socket_Exception);
		Socket(int socket) throw (Socket_Exception);
		Socket(Domain d, Type t, int proto, int ifindex=-1) throw (Socket_Exception);
		 ~Socket() throw (Socket_Exception);
		void Bind(unsigned short int proto, int ifindex) throw (Socket_Exception);
		int Receive(std::string &Buffer, unsigned size) throw(Socket_Exception);
		template <class T> int ReceiveFrom(std::string &Buffer, unsigned size, T *addr) throw(Socket_Exception)
		{
			char buffer[size];
			int recv_size = ReceiveFrom(buffer, size, addr);
			Buffer = std::string(buffer,recv_size);
			return recv_size;
		}
		int Receive(void *Buffer, unsigned size) throw(Socket_Exception);
		template <class T> int ReceiveFrom(void *Buffer, unsigned size, T *addr) throw(Socket_Exception)
		{
		        int recv_size = 0;
			socklen_t addrlen=sizeof(T);
		        #ifdef USE_LIBBOOST
			boost::mutex::scoped_lock scoped_lock(mutex);
		        #endif
			if(sockfd!=0)
		        if((recv_size=recvfrom(sockfd,Buffer,size,0, (struct sockaddr *) addr, &addrlen))==-1) throw Socket_Exception(strerror(errno));
			if(addr==NULL) throw Socket_Exception("no source information provided"); 
		        return recv_size;
		}

		int TryReceive(void *Buffer, unsigned size) throw(Socket_Exception);
		int SendTo(const std::string Buffer,struct sockaddr_storage *) throw(Socket_Exception);
		int SendTo(const void *Buffer, unsigned size, struct sockaddr_storage *) throw(Socket_Exception);
		int Send(const std::string Buffer) throw(Socket_Exception);
		struct ether_header CreateL2Header( const void *srcl2addr, const void *dstl2addr, unsigned protocol);
		int SendX(int interface, const void *dstl2addr, const in_addr &src, const in_addr &dst, uint8_t protocol, const void *payload, uint16_t len) {
			return SendX(interface, dstl2addr, src, dst, 1, protocol, payload, len); 
		};
		int SendX(int interface, const void *dstl2addr, const in6_addr &src, const in6_addr &dst, uint8_t protocol, const void *payload, uint16_t len) {
			return SendX(interface, dstl2addr, src, dst, 1, protocol, payload, len); 
		};
		int SendX(int interface, const void *dstl2addr, const in_addr &src, const in_addr &dst, int hlim, uint8_t protocol, const void *payload, uint16_t len); 
		int SendX(int interface, const void *srcl2adrr, const void *dstl2addr, const in_addr &src, const in_addr &dst, int hlim, uint8_t protocol, const void *payload, uint16_t len); 
		int SendX(int interface, const void *dstl2addr, const in6_addr &src, const in6_addr &dst, int hlim, uint8_t protocol, const void *payload, uint16_t len); 
		int SendX(int interface, const void *srcl2addr, const void *dstl2addr, const in6_addr &src, const in6_addr &dst, int hlim, uint8_t protocol, const void *payload, uint16_t len); 

		int Send(const void *Buffer, unsigned size) throw(Socket_Exception);
		bool Poll(int timeout);		//in miliseconds
		bool Close() throw(Socket_Exception);
		int GetSocketFD() { return sockfd; }; 
			
	protected:

		Domain domain;
		Type type;
		int protocol;
		#ifdef USE_LIBBOOST
		boost::mutex mutex,destroy;
		#endif
		int sockfd;

};


#endif	//_SOCKET_H_
