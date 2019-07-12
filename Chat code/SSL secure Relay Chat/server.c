//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include "openssl/ssl.h"
#include "openssl/err.h"
 
#define FAIL    -1
	


#define TRUE 1 
#define FALSE 0 
#define PORT 4443
	
char substring(char s[]) {
      char sub;
      sub = s[0];
      return sub;
   
}





SSL_CTX* InitServerCTX(void)
{   const SSL_METHOD *method;
    SSL_CTX *ctx;
 
    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    method = TLSv1_2_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   /* create new context from method */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}
 
void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}
 
void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;
 
    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("No certificateslll.\n");
}
 






int main(int argc , char *argv[]) 
{ 

	  SSL_CTX *ctx[30];
	 // Initialize the SSL library
    SSL_library_init();
for(int i=0;i<30;i++)
{
	ctx[i]= InitServerCTX();
	LoadCertificates(ctx[i], "mycert.pem", "mycert.pem"); /* load certs */
}
       /* initialize SSL */
   // LoadCertificates(ctx, "mycert.pem", "mycert.pem"); /* load certs */
        SSL *ssl[30];



	int opt = TRUE; 
	int master_socket , addrlen , new_socket , client_socket[30] , 
		max_clients = 30 , activity, i , valread , sd; 
	int max_sd; 
	struct sockaddr_in address; 
	char name[30];
		
	char buffer[1025]; //data buffer of 1K 
		
	//set of socket descriptors 
	fd_set readfds; 
		

	
	//initialise all client_socket[] to 0 so not checked 
	for (i = 0; i < max_clients; i++) 
	{ 
		client_socket[i] = 0; 
	} 
		
	//create a master socket 
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this 
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
		sizeof(opt)) < 0 ) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	
	//type of socket created 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
		
	//bind the socket to localhost port 8888 
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("Listener on port %d \n", PORT); 
		
	//try to specify maximum of 3 pending connections for the master socket 
	if (listen(master_socket, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
		
	//accept the incoming connection 
	addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 
		
	while(TRUE) 
	{ 
		//clear the socket set 
		FD_ZERO(&readfds); 
	
		//add master socket to set 
		FD_SET(master_socket, &readfds); 
		max_sd = master_socket; 
			
		//add child sockets to set 
		for ( i = 0 ; i < max_clients ; i++) 
		{ 
			//socket descriptor 
			sd = client_socket[i]; 
				
			//if valid socket descriptor then add to read list 
			if(sd > 0) 
				FD_SET( sd , &readfds); 
				
			//highest file descriptor number, need it for the select function 
			if(sd > max_sd) 
				max_sd = sd; 
		} 
	
		//wait for an activity on one of the sockets , timeout is NULL , 
		//so wait indefinitely 
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} 
			
		//If something happened on the master socket , 
		//then its an incoming connection 
		if (FD_ISSET(master_socket, &readfds)) 
		{ 
			if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			} 
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
				(address.sin_port)); 
			for (i = 0; i < max_clients; i++) 
			{ 
				//if position is empty 
				if( client_socket[i] == 0 ) 
				{ 
					ssl[i]= SSL_new(ctx[i]);              /* get new SSL state with context */
        				SSL_set_fd(ssl[i],new_socket); 
					client_socket[i] = new_socket; 
					printf("Adding to list of sockets as %d\n" , i); 
					bzero(buffer,256);
					 if ( SSL_accept(ssl[i]) == FAIL ) 
{
			printf("pron in  conn");
        					ERR_print_errors_fp(stderr);
}
					valread = read(client_socket[i] , buffer, 255);
					char q;
					q=substring(buffer);
					name[i]=q;
					printf("Name of client %c\n",name[i]);
					int n = send(client_socket[i] ,"OK",strlen("OK"),0);
			   		if (n < 0) perror("ERROR writing to socket");
					break; 
				} 
			} 
		} 
			
			
		//else its some IO operation on some other socket 
		

		for (i = 0; i < max_clients; i++) 
		{ 
			sd = client_socket[i]; 
			//s	printf("Waiting for reciver....id");
			bzero(buffer,256);
			if (FD_ISSET( sd , &readfds)) 
			{ 	
				
				   
				//ShowCerts(ssl[i]);
				//printf("Waiting for sender....id");
				//int n = write(sd,"Enter username of reciver",strlen("Enter username of reciver"));
			   	//if (n < 0) perror("ERROR writing to socket");
				int n;
				
				n = SSL_read(ssl[i], buffer, 255);
				if (n < 0) 
             				perror("ERROR reading from socket");
				char q;
				q=substring(buffer);
				char id=q;
				fputs("Message reciverd\n", stdout);
				printf("%c\n",id);
				int fd_id=-1;
				for(int j=0;j<max_clients; j++)
				{
					if(name[j]==id)
					{
						fd_id=j;	
					}
				}
				if(fd_id==-1)
				{
					n = SSL_write(ssl[i],"not found",strlen("not found"));
           				if (n < 0) perror("ERROR writing to socket");
				}
else
{	
				bzero(buffer,256);
				sprintf(buffer, "MSG to be sent to ID %d",fd_id); 
				int w=fd_id;
				int rsd=client_socket[w];
				int n = SSL_write(ssl[i],buffer,strlen(buffer));
			   	if (n < 0) perror("ERROR writing to socket");
			while(1)
			{
				
				bzero(buffer,256);
				valread = SSL_read(ssl[i], buffer, 255);
				rsd=client_socket[w];
				n = SSL_write(ssl[w],buffer,strlen(buffer));
				fputs("Message delivered\n", stdout);				
				if (n < 0) perror("ERROR writing to socket");
				//write(sd,"Message Delivered",strlen("Message Delivered"));
				bzero(buffer,256);
				SSL_read(ssl[w],buffer, 255);
				fputs("Message reciverd\n", stdout);
				sd = client_socket[i]; 
				n=SSL_write(ssl[i],buffer,strlen(buffer));
				fputs("Message fowarded to host\n", stdout);
				//if (n < 0) perror("ERROR writing to socket");
			}

}
			} 
		} 
	} 
		
	return 0; 
} 

