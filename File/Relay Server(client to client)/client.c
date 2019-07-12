#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include<ctype.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[1025];
	if (argc < 3)
	{
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	if (server == NULL) 
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");
	printf("[+]Connected to Server.\n");
	printf("Choose your id:");
        bzero(buffer,1025);
        fgets(buffer,1024,stdin);
	send(sockfd, buffer, strlen(buffer), 0);
	printf("1.SEND\n2.RECIVE\n");
	int choice;	
	scanf("%d",&choice);
	bzero(buffer,1025);
	read(sockfd,buffer,1024);
	printf("Server says\"%s\"\n",buffer);
	switch(choice)
	{
		case 1:
		{
			printf("Enter id to connect:\n");
			bzero(buffer,1025);
			scanf("%s",buffer);
			n=write(sockfd,buffer, strlen(buffer));
			if (n < 0) 
             			perror("ERROR writing to socket");
			bzero(buffer,1025);
			n = read(sockfd,buffer,1024);
			if (n < 0) 
			     perror("ERROR reading from socket");
			printf("Server : %s\n",buffer);
			bzero(buffer,1025);
			int words = 0;
			char c;
			FILE *f;
			char file_name[256];
			printf("Enter file name:");
			scanf("%s",file_name);
			char buf[100];
			snprintf(buf, sizeof(buf), "%s.txt", file_name);
			if(f=fopen(buf, "r"))
			{
				printf("ok\n");
			}
			else
			{
				printf("File not found\n");
			}
	
			while((c=getc(f))!=EOF)			//Counting No of words in the file
			{	
				//fscanf(f , "%s" , buffer);
				//if(isspace(c)||c=='\t')
				words++;	
			}
			printf("Words = %d \n"  , words);	//Ignore

			write(sockfd, &words, sizeof(int));
			rewind(f);
			fseek(f, 0L, SEEK_END);    	// tells size of the file. Not rquired for the functionality in code.
			int sz = ftell(f);				//Just written for curiosity.
			printf("Size is %d \n" , sz);
			rewind(f);  
			char ch ;
			//ch = fgetc(f);
			//printf("The file was sent successfully\n");
			/*
			while(ch != EOF)
			{
				//bzero(buffer,256);
				fscanf(f,"%[^\n]",buffer);
				//fscanf(f , "%s" , buffer);
				//printf("%s\n" , buffer);	//Ignore
				write(sockfd,buffer,1024);
				ch = fgetc(f);
			}*/
			
			while((ch = fgetc(f)) != EOF)
	      		{
				write(sockfd,&ch,sizeof(char));
				printf("%c",ch);
				
			}
	                fclose(f);
			printf("The file was sent successfully\n");
			break;
		}
		case 2:
		{
			FILE *fp;
			int ch = 0;
			fp = fopen("recv.txt","w");            
			int words;
			read(sockfd, &words, sizeof(int));
			printf("Passed integer is : %d\n" , words);      //Ignore , Line for Testing
			while(ch != words)
			{
				char c;
				 //bzero(buffer,256);
				 //read(sockfd, buffer ,1024);
				read(sockfd,&c,sizeof(char));
				printf("%c",c);  
				fputc(c,fp);
				 //fprintf(fp , "%s\n" , buffer);
				 //printf(" %s %d "  , buffer , ch); //Line for Testing , Ignore
				 ch++;
			}
			//while(1){}
			printf("The file was received successfully\n");
			printf("The new file created is recv.txt\n");
			break;
		}
	}
	close(sockfd);
	return 0;
}
