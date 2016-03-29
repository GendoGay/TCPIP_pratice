#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()
#include <unistd.h>  //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_ntoa
#include <netinet/in.h>
#include <sys/time.h> // gettimeofday func

#define Server_PortNumber 1234
#define Server_Address "140.120.14.241"
#define DATAGRAM_NUM 279

int main(int argc, char *argv[]) {
    struct sockaddr_in address;
    struct timeval start_t,end_t;
    //unsigned long long start_utime, end_utime;
    long double start_sec, end_sec, t_interval;
    int sock, byte_sent;
    char buffer[6]="hello\0";
    int i = 0, bytes = 0;


    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0)    printf("Error creating socket\n");

    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(Server_PortNumber);
    address.sin_addr.s_addr = inet_addr(Server_Address);
    int address_length = sizeof(address);

    /* clock start */
    gettimeofday(&start_t,NULL);
    //start_utime = start_t.tv_sec * 1000000 + start_t.tv_usec;
    start_sec = (double)start_t.tv_sec + (double)start_t.tv_usec / 1000000;

    for(i = 0; i < DATAGRAM_NUM; i++){
	
	byte_sent = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&address, address_length);
	if (byte_sent < 0)    printf("Error sending packet\n");
	bytes = bytes + sizeof(buffer);
	//printf("sizeof buffer: %d\n", bytes);
    }

    /* clock end */
    gettimeofday(&end_t,NULL);
    //end_utime = end_t.tv_sec * 1000000 + end_t.tv_usec;
    end_sec = (double)end_t.tv_sec + (double)end_t.tv_usec / 1000000;
    
    t_interval = end_sec - start_sec;
    //Time interval: %llu\n  = end_utime - start_utime
    printf("Server IP: %s\nDatagran number: %d\nTime interval1: %llf\nThroughput: %llf Mbps\n",Server_Address, DATAGRAM_NUM, t_interval, (double)bytes / (t_interval * 1000));



    close(sock);
    return 0;

}

