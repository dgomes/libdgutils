/*
 * SocketMulticast.cc
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

#include "SocketMulticast.h"

#include <arpa/inet.h>

SocketMulticast::SocketMulticast() : /*Socket(INET6,DGRAM,0),*/ifindex(0) {};

int SocketMulticast::Send(std::string buf) 
{
	return SendTo(buf,&addr);	
}

int SocketMulticast::Receive(std::string &buf, unsigned size) 
{
	struct sockaddr source_addr;
	return ReceiveFrom(buf,size,&source_addr);	
}
int SocketMulticast::Send(const void *buf, unsigned size) 
{
	return SendTo(buf,size,&addr);	
}

int SocketMulticast::Receive(void *buf, unsigned size) 
{
	struct sockaddr source_addr;
	return ReceiveFrom(buf,size,&source_addr);	
}


bool SocketMulticast::createSocket(const char *group, const char *port, int family, int socktype) {
    struct addrinfo hints, *res, *ai;
    int error, retval;
	int yes = 1;	

    retval = -1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = family;
    hints.ai_socktype = socktype;

    error = getaddrinfo(group, port, &hints, &res);

    if (error != 0) {
        fprintf(stderr, "getaddrinfo error:: [%s]\n", gai_strerror(error));
        return false;
    }

	if(sockfd>=0) close(sockfd);

    sockfd=-1;
    for (ai=res;ai; ai=ai->ai_next) {
        if ((sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0) 
			continue;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
			perror("setsockopt(SO_REUSEADDR) error:: ");
		if (bind(sockfd, ai->ai_addr, ai->ai_addrlen) == 0) { // we got our sock 
			break;		
		}
	    close(sockfd);
    }
    freeaddrinfo(res);

	memcpy(&addr, ai->ai_addr, ai->ai_addrlen);

	return true;
}

// ASM Group will have source=NULL as default value
bool SocketMulticast::joinGroup(const char *group, const char *port, int family, int socktype) {

    switch (addr.ss_family) {
        case AF_INET: {
            struct ip_mreq      mreq;

            mreq.imr_multiaddr.s_addr=
                ((struct sockaddr_in *)&addr)->sin_addr.s_addr;
            mreq.imr_interface.s_addr= INADDR_ANY;

            if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const void *)&mreq, sizeof(mreq))<0)
                perror("joinGroup:: IP_ADD_MEMBERSHIP:: ");
			return true;

        } break;

        case AF_INET6: {

       	   struct ipv6_mreq    mreq6;
           memcpy(&mreq6.ipv6mr_multiaddr,
                 &(((struct sockaddr_in6 *)&addr)->sin6_addr),
                 sizeof(struct in6_addr));
           mreq6.ipv6mr_interface= 0; // any interface 

           if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq6, sizeof(mreq6))<0)
              perror("joinGroup:: IPV6_JOIN_GROUP:: ");

			return true;

        } break;

        default:
			break;
    }

    return false;
}

#ifdef LINUX
// shamelessly copied from http://www.ietf.org/mail-archive/web/pim/current/msg00783.html
int mysetsourcefilter(int s, uint32_t interface,
		    struct sockaddr *group, socklen_t grouplen,
		    /* RFC 3678 says numsrc is uint_t! */
		    uint32_t fmode, uint32_t numsrc,
		    struct sockaddr_storage *slist) {
    struct group_req greq;
    struct group_source_req gsreq;
    int slevel, sopt;
    unsigned i;

    slevel = group->sa_family == AF_INET ? IPPROTO_IP : IPPROTO_IPV6;
    
    if (!numsrc || fmode == MCAST_EXCLUDE) {
	/* should we allow numsrc == 0? */
	/* do we need to join group before blocking sources? */
	greq.gr_interface = interface;
	memcpy(&greq.gr_group, group, grouplen);
	if (setsockopt(s, slevel, MCAST_JOIN_GROUP,
		       (char *)&greq, sizeof(greq)) < 0)
	    return -1;

	if (!numsrc)
	    return 0;
    }
    
    /* do we really need to set interface when blocking? Can you join group
       on multiple interfaces and block different sources? */
    gsreq.gsr_interface = interface;
    memcpy(&gsreq.gsr_group, group, grouplen);
    sopt = fmode == MCAST_EXCLUDE ? MCAST_BLOCK_SOURCE : MCAST_JOIN_SOURCE_GROUP;

    for (i = 0; i < numsrc; i++) {
	gsreq.gsr_source = slist[i];
	if (setsockopt(s, slevel, sopt, (char *)&gsreq, sizeof(gsreq)) < 0) {
		/* We should clean up. When blocking, enough to leave group, or
		   unblock sources also? */
	    return -1;
	}
    }
    
    return 0;
}

bool SocketMulticast::joinSourceGroup(const char *source, const char*group, const char *port, int family, int socktype) {

    switch (addr.ss_family) {
        case AF_INET6:
			{ 
				struct sockaddr_in6 multicast_address;
				int address_length = sizeof(multicast_address);
			    
				int filter_mode = MCAST_INCLUDE;

				int number_sources = 1;
				struct sockaddr_storage source_list[1];

					/*
				memset(source_list, 0, sizeof(source_list));
				((struct sockaddr_in6 *) &source_list[0])->sin6_family = AF_INET6; 
				((struct sockaddr_in6 *) &source_list[0])->sin6_port = 0;
			    inet_pton(AF_INET6, source, &(((struct sockaddr_in6 *) &source_list[0])->sin6_addr));
	*/
				struct addrinfo hints, *res;
				memset(&hints, '\0', sizeof(hints));
			    hints.ai_family = AF_INET6;
				if (getaddrinfo(source, NULL, &hints, &res)) {
					return false;
				}
				memcpy(source_list,res->ai_addr, res->ai_addrlen);
				freeaddrinfo(res);

				if (getaddrinfo(group, NULL, &hints, &res)) {
					return false;
				}
				memcpy(&multicast_address,res->ai_addr, res->ai_addrlen);
				freeaddrinfo(res);
				
				//int rc = mysetsourcefilter(sockfd, ifindex, (struct sockaddr *) &multicast_address, address_length, filter_mode, number_sources, source_list);
				int rc = setsourcefilter(sockfd, ifindex, (struct sockaddr *) &multicast_address, address_length, filter_mode, number_sources, source_list);
				if(rc == 0)
					return true;
				else
					perror("joinSource:: ");
	
			}
			break;
		default:
			perror("not implemented");
	};
	return false;
}
#endif

bool SocketMulticast::hopLimit(int hops) {
    switch (addr.ss_family) {
        case AF_INET: 
			if(setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &hops, sizeof(hops))<0)
				perror("hopLimit:: IP_MULTICAST_HOPS:: ");
			else
				return true;
			break;	
        case AF_INET6: 
			if(setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hops, sizeof(hops))<0)
				perror("hopLimit:: IPV6_MULTICAST_HOPS:: ");
			else
				return true;
			break;
		default:
			perror("hopLimit:: Unknown family!:: ");
	};
	return false;
}

bool SocketMulticast::loopbackDelivery(bool flag) {
	int f=0;
	if(flag) f = 1;

    switch (addr.ss_family) {
        case AF_INET: 
			if(setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &f, sizeof(f))<0)
				perror("loopbackDelivery:: IP_MULTICAST_LOOP:: ");
			else
				return true;
			break;	
        case AF_INET6: 
			if(setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &f, sizeof(f))<0)
				perror("loopbackDelivery:: IPV6_MULTICAST_LOOP:: ");
			else
				return true;
			break;
		default:
			perror("loopbackDelivery:: Unknown family!:: ");
	};
	return false;
	
}

bool SocketMulticast::bindInterface(const NetDevice &dev) {
	ifindex = dev.getIfIndex();
    switch (addr.ss_family) {
        case AF_INET: 
			if(setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &ifindex, sizeof(ifindex))<0)
				perror("bindInterface:: IP_MULTICAST_IF:: ");
			else
				return true;
			break;	
        case AF_INET6: 
			if(setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifindex, sizeof(ifindex))<0)
				perror("bindInterface:: IPV6_MULTICAST_IF:: ");
			else
				return true;
			break;
		default:
			perror("bindInterface:: Unknown family!:: ");
	};
	return false;
}

int SocketMulticast::isMulticast(struct sockaddr_storage *addr)
{
    int retVal;

    retVal=-1;

    switch (addr->ss_family) {
        case AF_INET: {
            struct sockaddr_in *addr4=(struct sockaddr_in *)addr;
            retVal = IN_MULTICAST(ntohl(addr4->sin_addr.s_addr));
        } break;

        case AF_INET6: {
            struct sockaddr_in6 *addr6=(struct sockaddr_in6 *)addr;
            retVal = IN6_IS_ADDR_MULTICAST(&addr6->sin6_addr);
        } break;

        default:
           ;
    }

    return retVal;
} 

SocketMulticast::~SocketMulticast()
{
	sockfd=0;
}
