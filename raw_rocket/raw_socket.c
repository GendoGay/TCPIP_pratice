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

#define	IFF_PROMISC  0x100  /*receive all packets */


int main(int argc, char *argv[])
{
	struct ifreq ifr;
	struct ether_header *peth;
	struct iphdr *pip;
	struct udphdr *pudp;
	struct tcphdr *ptcp;
	struct icmphdr *picmp;
	struct igmphdr *pigmp;
	char *ptemp;
	char buffer[ETH_FRAME_LEN];
	int fd;	
	int ip_counter = 0, arp_counter = 0, rarp_counter = 0, udp_counter = 0, tcp_counter = 0,  icmp_counter = 0, igmp_counter = 0, counter = 0;


	// You need defining  a Raw Socket to get L2,L3 information.
	fd = socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

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
			counter = counter + 1;
			//r = recvfrom(sock, (char *)buf, sizeof(buf), 0, (struct sockaddr *)&addr, &len);
			ptemp = buffer;
			peth = (struct ether_header *)ptemp; //ethernet header
			switch(ntohs(peth->ether_type))
			{
				case 0x0800:
					ip_counter++;
					break;
				case 0x0806:
					arp_counter++;
					break;
				case 0x8035:
					rarp_counter++;
					break;
			}	
			ptemp += sizeof(struct ether_header);
			pip = (struct ip *)ptemp;
			switch(pip->protocol)
			{
				case IPPROTO_TCP:
					ptcp = (struct tcphdr *)ptemp;
					tcp_counter++;
					break;
				case IPPROTO_UDP:
					pudp = (struct udphdr *)ptemp;
					udp_counter++;
					break;
				case IPPROTO_ICMP:
					picmp = (struct icmphdr *)ptemp;
					icmp_counter++;
					break;
				case IPPROTO_IGMP:
					pigmp = (struct igmphdr *)ptemp;
					igmp_counter++;
					break;
			}
		}
		if(counter == 200)
			break;
	}
	printf("------statistics------\nIP\t:%d \nARP\t:%d \nRARP\t:%d \nTCP\t:%d \nUDP\t:%d \nICMP\t:%d \nIGMP\t:%d \n-----finish-----\n", ip_counter, arp_counter, rarp_counter, tcp_counter, udp_counter,  icmp_counter, igmp_counter);
	
	// before program terminated, remember to restore NIC’s flag
	ifr.ifr_flags&=~IFF_PROMISC;
	close(fd);
	return 0;
}






