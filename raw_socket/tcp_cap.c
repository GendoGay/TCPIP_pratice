#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h> 

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>  // inet_ntoa

#define	IFF_PROMISC  0x100  /*receive all packets */

int main(int argc, char *argv[])
{
	struct ifreq ifr;
	struct ether_header *peth;
	struct iphdr *pip;
	struct tcphdr *ptcp;
	char *ptemp;
	char buffer[ETH_FRAME_LEN], ip_dadd[18], ip_sadd[18], my_add[18];
	int fd;	
	int ip_counter = 0, arp_counter = 0, rarp_counter = 0, udp_counter = 0, tcp_counter = 0,  icmp_counter = 0, igmp_counter = 0, counter = 0;


	// You need defining  a Raw Socket to get L2,L3 information.
	fd = socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

	// get my addr
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	strcpy(my_add, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr) );

	// Using command SIOCGIFFLAGS to get the original flag
	strncpy(ifr.ifr_name, "eth0", 5);	// IFNAMSIZ == 5 ?
	if(ioctl(fd,SIOCGIFFLAGS,&ifr) == -1)
	{
		perror("ioctl");exit(1);
	} 
	ifr.ifr_flags |= IFF_PROMISC;
	if(ioctl(fd, SIOCGIFFLAGS, &ifr) == -1)
	{
		perror("ioctl");exit(3);
	}

	// You need to set NIC’s flag to IFF_PROMISC
	ifr.ifr_flags|=IFF_PROMISC;
	ioctl(fd,SIOCSIFFLAGS,&ifr);


	//recvfrom(fd, buffer, ETH_FRAME_LEN, 0, NULL, NULL);
	for(;;)
	{	
		if(recvfrom(fd, buffer, ETH_FRAME_LEN, 0, NULL, NULL) > 0)
		{
			ptemp = buffer;
			peth = (struct ether_header *)ptemp; //ethernet header
			if(ntohs(peth->ether_type) == 0x0800)
			{
				ptemp += sizeof(struct ether_header);
				pip = (struct iphdr*)ptemp;
				if(pip->protocol == IPPROTO_TCP)
				{
					// to get tcp port
					ptemp += sizeof(struct iphdr);
					ptcp = (struct tcphdr *)ptemp;
					// put source and destination into ip_sadd and ip_dadd
					sprintf(ip_sadd, "%d.%d.%d.%d",  (pip->saddr)&(0xFF), (pip->saddr >> 8)&(0xFF), (pip->saddr >> 16)&(0xFF),(pip->saddr >> 24)&(0xFF));
					sprintf(ip_dadd, "%d.%d.%d.%d",  (pip->daddr)&(0xFF), (pip->daddr >> 8)&(0xFF), (pip->daddr >> 16)&(0xFF),(pip->daddr >> 24)&(0xFF));

					// filter
					//if(strcmp(ip_dadd, MY_HOST)  && strcmp(ip_dadd, DEFAULT))
					if(!strcmp(my_add, ip_sadd) )
					{
						counter = counter + 1;
						printf("Source MAC address\t: %02x:%02x:%02x:%02x:%02x:%02x\n", peth->ether_shost[0], peth->ether_shost[1], peth->ether_shost[2], peth->ether_shost[3], peth->ether_shost[4], peth->ether_shost[5]);
						printf("Destination MAC address : %02x:%02x:%02x:%02x:%02x:%02x\n", peth->ether_dhost[0], peth->ether_dhost[1], peth->ether_dhost[2], peth->ether_dhost[3], peth->ether_dhost[4], peth->ether_dhost[5]);
						printf("protocol\t: %d\n", pip->protocol);
						printf("source ip\t: %s\n", ip_sadd);
						printf("destination ip\t: %s\n", ip_dadd);
						printf("source port\t: %d\n", ntohs(ptcp->th_sport)); // port can't match
						printf("destination port: %d\n\n", ntohs(ptcp->th_dport));
					}
				}
			}
		}
		if(counter == 10)
			break;
	}
	// before program terminated, remember to restore NIC’s flag
	ifr.ifr_flags&=~IFF_PROMISC;
	close(fd);
	return 0;
}






