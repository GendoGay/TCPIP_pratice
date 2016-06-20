#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>

#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>


#define DEFAULT_DLENTH 12
#define MAX_IP_LEN 60
#define MAX_ICMP_LEN 76
#define SERVER_IP "140.120.1.20"
#define	MAX_PACKET (65536-60-8)/* max packet size */

unsigned short cal_cksum(u_short * addr, int len)
{
	register int nleft;
	register u_short *w;
	register int sum = 0;
	u_short answer = 0;

	nleft = len;
	w = addr;

	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;//?
	}
	/* Take in an odd byte if present */
	if (nleft == 1) {
		*(u_char *)(&answer) = *(u_char *)w ;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return answer;

}

int main(int argc, char* argv[])
{
	struct icmp* icmp_hdr;
	struct sockaddr_in server_addr;
	int sock, data_len = DEFAULT_DLENTH, pack_len;
	//char *recv_buf = NULL; (X)
	//unsigned short *recv_buf=NULL; (X)
	//u_short *recv_buf = NULL; (X)
	// ***need allocate memory, thus we shouldn't just define this varialbe as a pointer
	u_char send_buf[MAX_PACKET], recv_buf[MAX_PACKET]; 

	struct timeval tvori, tvback;
	long tsori, tsrecv, tstran, tsback, RTT;


	// set server IP
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	// define raw socket with ICMP
	if( (sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("socket");
	}

	// define ICMP timpstamp
	icmp_hdr = (struct icmp*)send_buf;
	icmp_hdr->icmp_type = 13;
	icmp_hdr->icmp_code = 0;

	// get time and transform to sending format (millisec)
	gettimeofday(&tvori, NULL);
	tsori = (tvori.tv_sec % (24*60*60)) *1000 + tvori.tv_usec / 1000;

	icmp_hdr->icmp_otime = htonl(tsori);
	icmp_hdr->icmp_rtime = 0;
	icmp_hdr->icmp_ttime = 0;
	icmp_hdr->icmp_seq = 12345;	/* seq and id must be reflected */
	icmp_hdr->icmp_id = getpid();

	// out pack_len 8 bytes of header, 12 bytes of data 
	pack_len = data_len + 8; 

	// compute check-sum
	icmp_hdr->icmp_cksum = cal_cksum((u_short *)icmp_hdr, pack_len);

	// send packet
	if(sendto(sock, (char *)send_buf, pack_len, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("sendto error");
	}

	// long (+ int) => %lu ; icmp_time(uint32_t) => %u 
	printf("icmp_type = %d, icmp_code = %d\n", (int)icmp_hdr->icmp_type, (int)icmp_hdr->icmp_code);
	printf("Original timestamp = %u\n",ntohl(icmp_hdr->icmp_otime));
	printf("Receive timestamp  = %u\n",ntohl(icmp_hdr->icmp_rtime));
	printf("Transmit timestamp = %u\n",ntohl(icmp_hdr->icmp_ttime));
	printf("=======================================\n");

	// in pack_len

	int errno;
	//pack_len = data_len + MAX_IP_LEN + MAX_ICMP_LEN;

		// receive packet
	errno = recvfrom(sock, recv_buf, sizeof(recv_buf), 0, NULL, NULL);
	if(errno < 0)
	{
		if(errno != EINTR)
			perror("recvfrom error");
	}
	else
	{
		int hlen;
		struct ip *ip_hdr;
		struct icmp *icmp_hdr_recv; // skip ip header

		// catch and skip the part of ip header
		ip_hdr = (struct ip *)recv_buf;
		hlen = ip_hdr->ip_hl << 2;

		icmp_hdr_recv = (struct icmp *)(recv_buf + hlen);

		// timestamp only
		if(icmp_hdr_recv->icmp_type == ICMP_TSTAMPREPLY)
		{
			if (ntohl(icmp_hdr_recv->icmp_otime) != tsori)
				printf("originate timestamp not echoed: sent %lu, received %u\n",tsori, ntohl(icmp_hdr_recv->icmp_otime));

			//calculate round trip time
			gettimeofday(&tvback, NULL);//get final timestamp
			tsback = (tvback.tv_sec % (24*60*60)) *1000 + tvback.tv_usec / 1000;
			tsrecv = ntohl(icmp_hdr_recv->icmp_rtime); // get receive timestamp
			tstran = ntohl(icmp_hdr_recv->icmp_ttime); // get transimit timestamp

			RTT = ( tsback - tstran)+( tsrecv - tsori);
			printf("icmp_type = %d, icmp_code = %d\n", (int)icmp_hdr_recv->icmp_type, (int)icmp_hdr_recv->icmp_code);
			printf("Original timestamp = %u\n", ntohl(icmp_hdr_recv->icmp_otime));
			printf("Receive timestamp  = %lu\n", tsrecv);
			printf("Transmit timestamp = %lu\n", tstran);
			printf("Final timestamp = %lu\n", tsback);

			printf("RTT: %lu = (%zu - %lu) + (%lu - %lu)\n",RTT, tsback, tstran, tsrecv, tsori);
		}
	}

	close(sock);
	return 0;
}






