/*
 * Socket.cc 
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

#include <iostream>
using namespace std;

#include "Socket.h"

Socket::Socket() throw (Socket_Exception)
{
	struct protoent *proto = getprotobyname("ip");
	protocol = proto->p_proto;
	domain=INET6;
	type=STREAM;
	sockfd=0;
	
#ifdef USE_LIBBOOST
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if((sockfd=socket(domain,type,protocol))==-1) throw Socket_Exception(strerror(errno));
}

Socket::Socket(int socket) throw (Socket_Exception)
{
	struct protoent *proto = getprotobyname("ip");
	protocol = proto->p_proto;
	domain=INET6;
	type=STREAM;
	sockfd=socket;
}

Socket::Socket(Domain d, Type t, int proto, int ifindex) throw (Socket_Exception)
{
	domain=d;
	type=t;
	protocol = htons(proto);
#ifdef USE_LIBBOOST	
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if ((sockfd=socket(domain,type,protocol)) == -1) throw Socket_Exception(strerror(errno));
	
}

#ifdef LINUX
void Socket::Bind(unsigned short protocol, int ifindex) throw (Socket_Exception) {
	if(ifindex!=-1 && domain==PACKET) {	
		cerr << "Socket::Socket binding to interface " << ifindex << endl;	
		struct sockaddr_ll s;
		memset(&s,0,sizeof(sockaddr_ll));
		s.sll_family = AF_PACKET;
		s.sll_protocol = htons(protocol);
		s.sll_ifindex = ifindex;
		if(bind(sockfd,(struct sockaddr *) &s, sizeof(sockaddr_ll)) < 0) throw Socket_Exception(strerror(errno));
	}
}
#endif

int Socket::Receive(std::string &Buffer, unsigned size)  throw(Socket_Exception)
{
	char buffer[size];
	int recv_size = 0;
	
#ifdef USE_LIBBOOST
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if(sockfd!=0)
	if((recv_size=recv(sockfd,buffer,size,0))==-1) throw Socket_Exception(strerror(errno));
	Buffer = std::string(buffer,recv_size);
	
	return recv_size;
}

int Socket::Receive(void *Buffer, unsigned size)  throw(Socket_Exception)
{
	int c_recv = 0;
#ifdef USE_LIBBOOST
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if(sockfd!=0)
	if((c_recv=recv(sockfd,Buffer,size,0))==-1) throw Socket_Exception(strerror(errno));
	return c_recv;
}

int Socket::TryReceive(void *Buffer, unsigned size)  throw(Socket_Exception)
{
	int c_recv = 0;
#ifdef USE_LIBBOOST	
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if(sockfd!=0)
	if((c_recv=recv(sockfd,Buffer,size,MSG_PEEK))==-1) throw Socket_Exception(strerror(errno));
	if(c_recv==0) throw Socket_Exception("There is no data in the receive queue");
	return c_recv;
}

int Socket::SendTo(const std::string Buffer, struct sockaddr_storage *addr) throw(Socket_Exception)
{
	int c_sent = 0;
	unsigned addrlen;
	addrlen=sizeof(*addr); 
	
#ifdef USE_LIBBOOST
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if(sockfd!=0)
	if((c_sent=sendto(sockfd,Buffer.c_str(),Buffer.size(),0,(struct sockaddr *)addr, addrlen))==-1) throw Socket_Exception(strerror(errno));
	return c_sent;
}

int Socket::SendTo(const void *Buffer, unsigned size, struct sockaddr_storage *addr) throw(Socket_Exception)
{
	int c_sent = 0;
	unsigned addrlen;
	addrlen=sizeof(*addr); 
	
#ifdef USE_LIBBOOST
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if(sockfd!=0)
	if((c_sent=sendto(sockfd,Buffer,size,0,(struct sockaddr *)addr, addrlen))==-1) throw Socket_Exception(strerror(errno));
	return c_sent;
}

int Socket::Send(const std::string Buffer)  throw(Socket_Exception)
{
	int c_sent = 0;
#ifdef USE_LIBBOOST	
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if(sockfd!=0)
	if((c_sent=send(sockfd,Buffer.c_str(),Buffer.size(),0))==-1) throw Socket_Exception(strerror(errno));
	return c_sent;
}

int Socket::Send(const void *Buffer, unsigned size)  throw(Socket_Exception)
{
	int c_sent = 0;
#ifdef USE_LIBBOOST	
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if(sockfd!=0)
	if((c_sent=send(sockfd,Buffer,size,0))==-1) throw Socket_Exception(strerror(errno));
	return c_sent;
}

unsigned short in_chksum (u_short *addr, int len)
{
   register int nleft = len;
   register int sum = 0;
   u_short answer = 0;

   while (nleft > 1) {
      sum += *addr++;
      nleft -= 2;
   }

   if (nleft == 1) {
      *(u_char *)(&answer) = *(u_char *)addr;
      sum += answer;
   }

   sum = (sum >> 16) + (sum + 0xffff);
   sum += (sum >> 16);
   answer = ~sum;
   return(answer);
}

#ifdef LINUX
struct ether_header Socket::CreateL2Header(const void *srcl2addr, const void *dstl2addr, unsigned int protocol) {
	struct ether_header header;
	memcpy(header.ether_dhost, dstl2addr,ETH_ALEN);
	memcpy(header.ether_shost, srcl2addr,ETH_ALEN);
	header.ether_type = htons(protocol);
	return header;
}
#endif

#ifdef LINUX
int Socket::SendX(int interface, const void *srcl2addr, const void *dstl2addr, const in_addr &src, const in_addr &dst, int hlim, uint8_t protocol, const void *payload, uint16_t len)
{
	if(domain!=PACKET || type==RAW) return 0;
	char buffer[sizeof(iphdr)+len];
        
	sockaddr_ll addr;
        memset(&addr, 0, sizeof(addr));
        addr.sll_family = AF_PACKET;
        addr.sll_protocol = htons(ETH_P_IP);
        addr.sll_ifindex = interface;
        addr.sll_halen = 6;

        memcpy(addr.sll_addr, dstl2addr, 6);
	
	
	struct ether_header l2hdr = CreateL2Header(srcl2addr,dstl2addr,ETH_P_IPV6);
	memcpy(buffer,&l2hdr,sizeof(struct ether_header));

        iphdr *hdr = (iphdr *)(buffer+sizeof(struct ether_header));

   	hdr->tot_len = htons(sizeof(struct iphdr) + len);
	hdr->ihl = 5;
	hdr->version = 4;
	hdr->ttl = 255;
	hdr->tos = 0;
	hdr->frag_off = 0;
	hdr->protocol = protocol;

	in_addr_t inet_addr(const char *cp);
        char *inet_ntoa(struct in_addr in);

	in_addr s = src;
	in_addr d = dst;
	hdr->saddr = inet_netof(src);
	hdr->daddr = inet_netof(dst);
	hdr->check = in_chksum((u_short *)hdr, sizeof(struct iphdr));

        memcpy(buffer + sizeof(iphdr) + sizeof(struct ether_header), payload, len);

	int c_sent = 0;
#ifdef USE_LIBBOOST	
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if(sockfd!=0)
	if((c_sent=sendto(sockfd,buffer, len + sizeof(iphdr) + sizeof(struct ether_header), 0, (const sockaddr *)&addr, sizeof(addr)))==-1) throw Socket_Exception(strerror(errno));
	return c_sent;
}
#endif

#ifdef LINUX
int Socket::SendX(int interface, const void *dstl2addr, const in_addr &src, const in_addr &dst, int hlim, uint8_t protocol, const void *payload, uint16_t len)
{
	if(domain!=PACKET || type==RAW) return 0;
	char buffer[sizeof(iphdr)+len];
        
	sockaddr_ll addr;
        memset(&addr, 0, sizeof(addr));
        addr.sll_family = AF_PACKET;
        addr.sll_protocol = htons(ETH_P_IP);
        addr.sll_ifindex = interface;
        addr.sll_halen = 6;

        memcpy(addr.sll_addr, dstl2addr, 6);

        iphdr *hdr = (iphdr *)buffer;

   	hdr->tot_len = htons(sizeof(struct iphdr) + len);
	hdr->ihl = 5;
	hdr->version = 4;
	hdr->ttl = 255;
	hdr->tos = 0;
	hdr->frag_off = 0;
	hdr->protocol = protocol;

	in_addr_t inet_addr(const char *cp);
        char *inet_ntoa(struct in_addr in);

	in_addr s = src;
	in_addr d = dst;
	hdr->saddr = inet_netof(src);
	hdr->daddr = inet_netof(dst);
	hdr->check = in_chksum((u_short *)hdr, sizeof(struct iphdr));

        memcpy(buffer + sizeof(iphdr), payload, len);

	int c_sent = 0;
#ifdef USE_LIBBOOST	
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if(sockfd!=0)
	if((c_sent=sendto(sockfd,buffer, len + sizeof(iphdr), 0, (const sockaddr *)&addr, sizeof(addr)))==-1) throw Socket_Exception(strerror(errno));
	return c_sent;
}
#endif

#ifdef LINUX
int Socket::SendX(int interface, const void *srcl2addr, const void *dstl2addr, const in6_addr &src, const in6_addr &dst, int hlim, uint8_t protocol, const void *payload, uint16_t len) {
	
	if(domain!=PACKET) return 0;
	char buffer[sizeof(struct ether_header) + sizeof(ip6_hdr)+len];
        sockaddr_ll addr;
        memset(&addr, 0, sizeof(addr));
        addr.sll_family = AF_PACKET;
        addr.sll_protocol = htons(ETH_P_IPV6);
        addr.sll_ifindex = interface;
        addr.sll_halen = 6;

        memcpy(addr.sll_addr, dstl2addr, 6);

	struct ether_header l2hdr = CreateL2Header(srcl2addr,dstl2addr,ETH_P_IPV6);
	memcpy(buffer,&l2hdr,sizeof(struct ether_header));

        ip6_hdr *hdr = (ip6_hdr *) (buffer+sizeof(struct ether_header));

        hdr->ip6_flow = 0;
        hdr->ip6_vfc = 0x60;
        hdr->ip6_plen = htons(len);
        hdr->ip6_nxt = protocol;
        hdr->ip6_hlim = hlim;

        hdr->ip6_src = src;
        hdr->ip6_dst = dst;

	memcpy(buffer + sizeof(struct ether_header), hdr, sizeof(ip6_hdr));

        memcpy(buffer + sizeof(struct ether_header) + sizeof(ip6_hdr), payload, len);

	int c_sent = 0;
#ifdef	USE_LIBBOOST	
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if(sockfd!=0)
	if((c_sent=sendto(sockfd,buffer, len + sizeof(ip6_hdr) + sizeof(struct ether_header), 0, (const sockaddr *)&addr, sizeof(addr)))==-1) throw Socket_Exception(strerror(errno));
	return c_sent;

}
#endif

#ifdef LINUX
int Socket::SendX(int interface, const void *dstl2addr, const in6_addr &src, const in6_addr &dst, int hlim, uint8_t protocol, const void *payload, uint16_t len) 
{

	if(domain!=PACKET || type==RAW) return 0;
	char buffer[sizeof(ip6_hdr)+len];
        sockaddr_ll addr;
        memset(&addr, 0, sizeof(addr));
        addr.sll_family = AF_PACKET;
        addr.sll_protocol = htons(ETH_P_IPV6);
        addr.sll_ifindex = interface;
        addr.sll_halen = 6;

        memcpy(addr.sll_addr, dstl2addr, 6);

        ip6_hdr *hdr = (ip6_hdr *)buffer;

        hdr->ip6_flow = 0;
        hdr->ip6_vfc = 0x60;
        hdr->ip6_plen = htons(len);
        hdr->ip6_nxt = protocol;
        hdr->ip6_hlim = hlim;

        hdr->ip6_src = src;
        hdr->ip6_dst = dst;

        memcpy(buffer + sizeof(ip6_hdr), payload, len);

	int c_sent = 0;
#ifdef USE_LIBBOOST	
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if(sockfd!=0)
	if((c_sent=sendto(sockfd,buffer, len + sizeof(ip6_hdr), 0, (const sockaddr *)&addr, sizeof(addr)))==-1) throw Socket_Exception(strerror(errno));
	return c_sent;
}
#endif

bool Socket::Poll(int timeout)
{	
#ifdef USE_LIBBOOST
	boost::mutex::scoped_lock scoped_lock(destroy);
#endif
	struct pollfd ufds;
	ufds.fd = sockfd;
	
	ufds.events = POLLIN | POLLPRI;
	ufds.revents = 0;
	int res;
	if((res = poll(&ufds,1, timeout)) == -1) throw Socket_Exception(strerror(errno));	
	
	#ifdef DEBUG
	std::cerr << "Socket::Poll(" << timeout << ") = " << res << " : events: " << ufds.events << " , revents: " << ufds.revents << std::endl;
	#endif
	
	if((ufds.revents & POLLERR) == POLLERR) throw Socket_Exception("Error Condition");
	if((ufds.revents & POLLHUP) == POLLHUP) throw Socket_Exception("Hung Up");
	if((ufds.revents & POLLNVAL) == POLLNVAL) {
		sockfd = 0;
		throw Socket_Exception("Invalid request: fd not open");
	}
	
	if(res == 0) return false;	
	return true;
}

Socket::~Socket() throw(Socket_Exception)
{
#ifdef USE_LIBBOOST
	boost::mutex::scoped_lock scoped_lock(destroy);
#endif
	if(sockfd>0)
		Close();
}

bool Socket::Close() throw(Socket_Exception) {
#ifdef USE_LIBBOOST
	boost::mutex::scoped_lock scoped_lock(mutex);
#endif
	if(sockfd==-1) throw Socket_Exception("sockfd=-1");
	if(shutdown(sockfd,SHUT_RDWR)==-1) {
		throw Socket_Exception(strerror(errno));
	}
	if(close(sockfd)==0) {
		sockfd=0;
		return true;
	}
	return false;
	throw Socket_Exception(strerror(errno));
}
