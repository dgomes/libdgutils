#include "NetDevice.h"

NetDevice::NetDevice(const std::string &ifn) { 
	if (( skfd = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ) {
		throw NetDevice_Exception("socket error");
	}
	ifname = ifn;
}

NetDevice::~NetDevice() {
	close(skfd);
}

void NetDevice::call_ioctl(struct ifreq &ifr, int request) const {
	strncpy(ifr.ifr_name,ifname.c_str(),IFNAMSIZ);

	if (ioctl(skfd, request, &ifr) == -1) {
		throw NetDevice_Exception(strerror(errno));
	}
}

int NetDevice::getIfIndex() const {
#ifdef LINUX
	struct ifreq ifr;
	call_ioctl(ifr,SIOCGIFINDEX);
	return ifr.ifr_ifindex;
#else	
	//MacOS
	return INADDR_ANY;
#endif
}

bool NetDevice::isUp() {
	struct ifreq ifr;
	call_ioctl(ifr,SIOCGIFFLAGS);
	if((ifr.ifr_flags | (IFF_UP | IFF_RUNNING)) == (IFF_UP | IFF_RUNNING))
		return true;
	return false;
}

#ifdef LINUX
struct sockaddr NetDevice::getHWAddress() {
	struct ifreq ifr;
	call_ioctl(ifr,SIOCGIFHWADDR);
	return ifr.ifr_hwaddr;
}
#endif

void NetDevice::bringUp() {
	struct ifreq ifr;
	call_ioctl(ifr,SIOCGIFFLAGS);
	if((ifr.ifr_flags | (IFF_UP | IFF_RUNNING)) != (IFF_UP | IFF_RUNNING)) {
		P_DEBUG("Bringing up " << ifname);
		ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
		call_ioctl(ifr,SIOCSIFFLAGS);
	}
}

void NetDevice::setPromisc() {
	struct ifreq ifr;
	call_ioctl(ifr,SIOCGIFFLAGS);
	if((ifr.ifr_flags | (IFF_PROMISC)) != (IFF_PROMISC)) {
		P_DEBUG("Setting " << ifname << " to promisc");
		ifr.ifr_flags |= IFF_PROMISC;
		call_ioctl(ifr,SIOCSIFFLAGS);
	}
}

#ifdef LINUX
void NetDevice::setHWAddress(struct sockaddr &addr) {
	struct ifreq ifr;
	memcpy(&ifr.ifr_hwaddr,&addr,sizeof(struct sockaddr));
	P_DEBUG("Setting " << ifname << " to mac: " << ifr.ifr_hwaddr.sa_data);
	call_ioctl(ifr,SIOCSIFHWADDR);
}

int default_scope(inet_prefix *lcl)
{
        if (lcl->family == AF_INET) {
                if (lcl->bytelen >= 1 && *(__u8*)&lcl->data == 127)
                        return RT_SCOPE_HOST;
        }
        return 0;
}

bool NetDevice::addNeighbour(lladdress &lla, ipv6address &ip) {
    struct rtnl_handle rth;
    struct {
        struct nlmsghdr         n;
        struct ndmsg            ndm;
        char                    buf[256];
    } req;
    inet_prefix dst;

    memset(&req, 0, sizeof(req));
    
    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ndmsg));
    req.n.nlmsg_flags = NLM_F_REQUEST|NLM_F_CREATE|NLM_F_EXCL; //my flags: NLM_F_CREATE|NLM_F_EXCL
    req.n.nlmsg_type = RTM_NEWNEIGH; // my command: RTM_NEWNEIGH
    req.ndm.ndm_family = AF_INET6;
    req.ndm.ndm_state = NUD_REACHABLE;

    memset(&dst, 0, sizeof(inet_prefix));
    dst.family = AF_INET6;
    inet_pton(AF_INET6, ip.getIP().c_str(), dst.data);
    dst.bytelen = 16;
    dst.bitlen = -1;
    
    
    addattr_l(&req.n, sizeof(req), NDA_DST, &dst.data, dst.bytelen);
    addattr_l(&req.n, sizeof(req), NDA_LLADDR, lla.getLLAddr(), ETHER_ADDR_LEN);

    if (rtnl_open(&rth, 0) < 0)
        return false;

    ll_init_map(&rth);

    req.ndm.ndm_ifindex = getIfIndex();

    if (rtnl_talk(&rth, &req.n, 0, 0, NULL, NULL, NULL) < 0)
        return false;
    return true;
}

struct sockaddr NetDevice::getNeighbourHWAddress(const ipv6address &ip) {
    struct nlmsghdr	*nlmsg;
	struct ndmsg	*ndmsg;
	uint8_t	buf[16384];
	uint8_t	*tmp = buf;
	ssize_t	len;
	ssize_t ssize;
	int nread=0, ntread=0;
	int	fd;
	bool flag = false;

	P_DEBUG("Resolving IPv6: " << ip << " to LL Address");
    
	memset(buf, 0, sizeof(buf));
  	nlmsg = (struct nlmsghdr *)buf;
  	nlmsg->nlmsg_len   = NLMSG_LENGTH(sizeof(struct ndmsg));
  	nlmsg->nlmsg_type  = RTM_GETNEIGH;
  	nlmsg->nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
  	nlmsg->nlmsg_seq   = 20394;
	nlmsg->nlmsg_pid   = getpid();

  	ndmsg = (struct ndmsg *) NLMSG_DATA(nlmsg);
  	ndmsg->ndm_family = AF_INET6;

	addattr_l(nlmsg, sizeof(buf), NDA_DST, ip.get_in6_addr(), 16);

	if((fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) == -1) {
		P_DEBUG("could not open netlink");
		//TODO throw error!
	}	

	len = nlmsg->nlmsg_len;
  	if((ssize = send(fd, buf, len, 0)) < len) {
    	if(ssize == -1) {
		P_DEBUG("could not send netlink");
        //TODO throw error!
		}
    }

	do {
		if( (nread = read(fd, tmp, sizeof(buf) - ntread )) < 0) {
			P_DEBUG("could not read netlink");
    	    //TODO throw error!
			}
		nlmsg = (struct nlmsghdr *)tmp;
		if( !(NLMSG_OK(nlmsg, nread)) || (nlmsg->nlmsg_type == NLMSG_ERROR) ) {
			P_DEBUG("Error in netlink message");
	        //TODO throw error!
		}
		// Skip last message (contains no data)
		if(nlmsg->nlmsg_type == NLMSG_DONE) {
			flag = true;
		} else{
			tmp += nread;
			ntread += nread;
		}
		//in this case we never had a multipart message
		if( (nlmsg->nlmsg_flags & NLM_F_MULTI) == 0 )
			flag = true;
	}while(nlmsg->nlmsg_seq != 20394 || !flag); //TODO understand why pid is not equal to getpid!!!
	//loop thru data buffer returned and print out info
	struct rtattr	*attrp = NULL;
	int attrlen = 0;
  	nlmsg = (struct nlmsghdr *)buf;
	flag=false;
	for( ;  NLMSG_OK(nlmsg, ntread); nlmsg = NLMSG_NEXT(nlmsg, ntread)){
		//get pointers to all of our headers/data
		ndmsg = (struct ndmsg *)NLMSG_DATA(nlmsg);
		attrp = (struct rtattr *)NDA_RTA(ndmsg);
		attrlen = NDA_PAYLOAD(nlmsg);

		for( ; RTA_OK(attrp, attrlen); attrp = RTA_NEXT(attrp, attrlen) ){
			if(attrp->rta_type==NDA_DST) {
	//			P_DEBUG(ipv6address(*(const struct in6_addr *) RTA_DATA(attrp),0));
				if(memcmp(ip.get_in6_addr(), RTA_DATA(attrp), RTA_PAYLOAD(attrp))==0)
					flag=true;	
			};
			
			if(attrp->rta_type==NDA_LLADDR && flag) {
	//			P_DEBUG(ether_ntoa((struct ether_addr *)RTA_DATA(attrp)));
				struct sockaddr t;
				memcpy(t.sa_data, RTA_DATA(attrp),RTA_PAYLOAD(attrp)); 
				return t;
			}
		}
	}
	throw NetDevice_Exception("Failed resolve address to LL");
}


bool NetDevice::setIPv6Address(const ipv6address &ip, unsigned int netmask) {
    struct rtnl_handle rth;
    struct {
    	struct nlmsghdr         n;
        struct ifaddrmsg        ifa;
        char                    buf[256];
    } req;
    inet_prefix lcl;
    inet_prefix peer;

	P_DEBUG("Setting " << ifname << " to IPv6: " << ip);
    memset(&req, 0, sizeof(req));

    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    req.n.nlmsg_flags = NLM_F_REQUEST;
    req.n.nlmsg_type = RTM_NEWADDR;
    req.ifa.ifa_family = AF_INET6;


	memset(&lcl, 0, sizeof(lcl));
	inet_pton(AF_INET6, ip.getIP().c_str(), lcl.data);
	lcl.family=AF_INET6;
	lcl.bytelen = 16;
	lcl.bitlen = netmask;

	peer = lcl; 
	addattr_l(&req.n, sizeof(req), IFA_ADDRESS, &lcl.data, lcl.bytelen);
	req.ifa.ifa_prefixlen = lcl.bitlen;
	req.ifa.ifa_scope = default_scope(&lcl);
		
	if (rtnl_open(&rth, 0) < 0)
                return false;

	ll_init_map(&rth);

    if ((req.ifa.ifa_index = getIfIndex()) == 0) {
    	std::cerr << "Cannot find device " << ifname << std::endl;
        return false;
	}

    if (rtnl_talk(&rth, &req.n, 0, 0, NULL, NULL, NULL) < 0)
		return false;
	return true;	
}
#endif
ipv6address NetDevice::getIPv6Address() {
	struct ifaddrs *ifap0, *ifap;
        char buf[BUFSIZ];

        memset(buf, 0, sizeof(buf));

        if (getifaddrs(&ifap0)) {
                throw NetDevice_Exception("Failed to retrieve Interfaces Addresses");
        }
	for (ifap = ifap0; ifap; ifap=ifap->ifa_next) {
		if (ifap->ifa_addr == NULL) continue;
		if (ifap->ifa_addr->sa_family != AF_INET6) continue;
		if (strcmp(ifname.c_str(),ifap->ifa_name)!=0) continue;
	        char host[NI_MAXHOST];
		if(getnameinfo(ifap->ifa_addr, sizeof(struct sockaddr_in6),
                                  host, sizeof(host), NULL, 0, NI_NUMERICHOST) < 0)
			throw NetDevice_Exception(strerror(errno));
		return ipv6address(host);
	}
        throw NetDevice_Exception("Failed to retrieve Interfaces Addresses (No interface configured with IPv6)");
}

ipv4address NetDevice::getIPv4Address() {
	struct ifaddrs *ifap0, *ifap;
        char buf[BUFSIZ];

        memset(buf, 0, sizeof(buf));

        if (getifaddrs(&ifap0)) {
                throw NetDevice_Exception("Failed to retrieve Interfaces Addresses");
        }
	for (ifap = ifap0; ifap; ifap=ifap->ifa_next) {
		if (ifap->ifa_addr == NULL) continue;
		if (ifap->ifa_addr->sa_family != AF_INET) continue;
		if (strcmp(ifname.c_str(),ifap->ifa_name)!=0) continue;
	        char host[NI_MAXHOST];
		if(getnameinfo(ifap->ifa_addr, sizeof(struct sockaddr_in),
                                  host, sizeof(host), NULL, 0, NI_NUMERICHOST) < 0)
			throw NetDevice_Exception(strerror(errno));
		return ipv4address(host);
	}
        throw NetDevice_Exception("Failed to retrieve Interfaces Addresses (No interface configured with IPv4)");
}
