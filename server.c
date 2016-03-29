#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()
#include <unistd.h>  //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h> // gettimeofday func
#define PortNumber 1234
#define DATAGRAM_NUM 279

int main(int argc, char *argv[]) {
    struct sockaddr_in address;
    struct timeval start_t,end_t;
    //unsigned long long start_utime, end_utime;
    long double start_sec, end_sec, t_interval;
    int sock, byte_recv;
    char buffer[50];
    int counter = 0, i = 0;
    int bytes = 0;
	

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0)    printf("Error creating socket\n");

    /* Set up server address */
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(PortNumber);
    address.sin_addr.s_addr = INADDR_ANY;  

    /* #define INADDR_ANY  ((unsigned long int) 0x00000000) 
    * INADDR_ANY allows the server to accept a client connection on any interface, 
    * in case the server host has multiple interfaces.
    */

    if (bind(sock,(struct sockaddr *)&address, sizeof(address)) == -1) {
        printf("error binding\n");
        close(sock);
    }

    int address_length = sizeof(address);      
  
    for(i = 0 ; i < DATAGRAM_NUM ; i++){

	byte_recv = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr  
             *)&address, &address_length);

	/* start at the first packet arrive*/	
	if(i == 1){
	    /* clock start */
	    gettimeofday(&start_t,NULL);
	    //start_utime = start_t.tv_sec * 1000000 + start_t.tv_usec;
	    start_sec = (double)start_t.tv_sec + (double)start_t.tv_usec / 1000000;
	}	

        if (byte_recv < 0)    
	    printf("Error recving packet\n");   

	/* show client info */
	char *client_addr = inet_ntoa(address.sin_addr);
	//printf("Receive data frome %s : %d\n",inet_ntoa(address.sin_addr), htons(address.sin_port));
	counter = counter + 1;

	/* lenth of buffer and 1byte '\0' */
	bytes = bytes + strlen(buffer)+1; 
        printf("datagram %d : %s %d\n",counter ,buffer, bytes);

	/* TODO:return to client */
	
    }
    
    /* clock end */
    if(counter == DATAGRAM_NUM){   
        gettimeofday(&end_t,NULL);
	//end_utime = end_t.tv_sec * 1000000 + end_t.tv_usec;
        end_sec = (double)end_t.tv_sec + (double)end_t.tv_usec / 1000000;
	//Time interval: %llu\n
	t_interval = end_sec - start_sec;
	printf("Client send completed!\nDatagram number : %d\nTime interval: %llf\nThroughput: %llf Mbps\n",counter, t_interval, (double)bytes / (t_interval * 1000));
    }  

    close(sock);
    return 0;
}

