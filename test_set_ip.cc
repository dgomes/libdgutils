#include <iostream>

#include "NetDevice.h"

extern "C" {
#include <asm/types.h> 
#include <linux/netlink.h> 
#include <linux/rtnetlink.h> 
#include <sys/socket.h>
}

using namespace std;

int main(int argc, char *argv[]) {
	struct {
               struct nlmsghdr nh;
               struct ifinfomsg   if_;
               char            attrbuf[512];
        } req;
        struct rtattr *rta;
        unsigned int mtu = 1000;
        int rtnetlink_sk = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);

        memset(&req, 0, sizeof(req));
        req.nh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
        req.nh.nlmsg_flags = NLM_F_REQUEST;
        req.nh.nlmsg_type = RTML_NEWLINK;
        req.if_.ifi_family = AF_UNSPEC;
        req.if_.ifi_index = INTERFACE_INDEX;
        req.if_.ifi_change = 0xffffffff; /* ???*/
        rta = (struct rtattr *)(((char *) &req) +
                        NLMSG_ALIGN(req.nn->nlmsg_len));
        rta->rta_type = IFLA_MTU;
        rta->rta_len = sizeof(unsigned int);
        req.nh.nlmsg_len = NLMSG_ALIGN(req.nh.nlmsg_len) +
                        RTA_LENGTH(sizeof(mtu));
        memcpy(RTA_DATA(rta), &mtu, sizeof (mtu));
        send(rtnetlink_sk, &req, req.n.nlmsg_len);
}
