#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()
#include <unistd.h>  //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PortNumber 5555
#define Packet_Num 20

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr,client_addr;
    int sock, byte_recv, server_addr_length = sizeof(server_addr), client_addr_length=sizeof(client_addr), recfd; 
    char buffer[50];
    int i = 0;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)    printf("Error creating socket\n");

    bzero(&server_addr, server_addr_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PortNumber);
    server_addr.sin_addr.s_addr = INADDR_ANY;  
   /*#define INADDR_ANY  ((unsigned long int) 0x00000000) 
      INADDR_ANY allows the server to accept a client connection on any interface, in case the server host has multiple interfaces.*/

	if (bind(sock,(struct sockaddr *)&server_addr, server_addr_length) == -1) {
		printf("error binding!\n");
		close(sock);}

	if (listen(sock, 20) == -1) {
		printf("listen failed!\n");
		close(sock);}
	if((recfd = accept(sock,(struct sockaddr *)&client_addr,&client_addr_length))==-1) 		{
		printf("accept failed!\n");
		close(sock);
	} else {
		for (i=0;i<Packet_Num;i++) {
			byte_recv = recv(recfd, buffer, sizeof(buffer),0);
			if (byte_recv < 0)	printf("Error recving packet\n");
			else			printf("data: %s\n",buffer);
		
		}	
	}
	close(sock);
}

