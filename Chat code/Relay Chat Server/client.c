#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4443

int main(){

	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[256];

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");
	printf("Choose your id:");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
	send(clientSocket, buffer, strlen(buffer), 0);
	printf("1.send\n2.recive\n");
	int v;
	scanf("%d",&v);
	bzero(buffer,256);
	int n = read(clientSocket,buffer,255);
	if (n < 0) 
		perror("ERROR reading from socket");
	printf("Server says\"%s\"\n",buffer);
	switch (v)
	{
		case 1:
		{	
			printf("Enter id to connect:\n");
			bzero(buffer,256);
			scanf("%s",&buffer);
			n=write(clientSocket,buffer, strlen(buffer));
			if (n < 0) 
             			perror("ERROR writing to socket");
			printf("message to be  delivered to k\n");
			bzero(buffer,256);
			n = read(clientSocket,buffer,255);
			if (n < 0) 
			     perror("ERROR reading from socket");
			printf("Server : %s\n",buffer);
			while(1)
			{
			printf("Msg:");
			bzero(buffer,256);
			scanf("%s",&buffer);
			write(clientSocket, buffer, strlen(buffer));	
			bzero(buffer,256);
			n = read(clientSocket,buffer,255);
			//if (n < 0) 
			 //    perror("ERROR reading from socket");
			//}
			printf("User: %s\n",buffer);	
			//if(strcmp(buffer, "Bye") == 0)
			//{
		//		close(clientSocket);
		//		printf("[-]Disconnected from server.\n");
		//		exit(1);*/
			}
	
		}
		case 2:
		{
			printf("waiting for msg....\n");	
			while(1){
			bzero(buffer,256);
			n = read(clientSocket,buffer,255);
			if (n < 0) 
			     perror("ERROR reading from socket");
			printf("U got a mseg: %s\n",buffer);
			printf("Send Msg:");
			bzero(buffer,256);
			scanf("%s",&buffer);
			//printf("send %s",buffer);
			n=write(clientSocket, buffer, strlen(buffer));
			//if (n < 0) 
			  //   perror("ERROR reading from socket");
			
			}
		}
	}


	return 0;
}
