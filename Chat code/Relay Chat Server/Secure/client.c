#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 4443
 
#define FAIL    -1

SSL_CTX* InitCTX(void)
{   SSL_METHOD *method;
    SSL_CTX *ctx;
 
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = TLSv1_2_client_method();  /* Create new client-method instance */
    ctx = SSL_CTX_new(method);   /* Create new context */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}
 
void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;
    
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}



int main(){
	SSL_CTX *ctx;
	SSL *ssl;
	
	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[256];

	SSL_library_init();	
		
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");
		
	ctx = InitCTX();

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	 ssl = SSL_new(ctx);      /* create new SSL connection state */
    	SSL_set_fd(ssl,clientSocket);    /* attach the socket descriptor */
    	if ( SSL_connect(ssl) == FAIL )   /* perform the connection */
        	ERR_print_errors_fp(stderr);

else
{
	printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
	ShowCerts(ssl);
	printf("[+]Connected to Server.\n");
	printf("Choose your id:");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
	SSL_write(clientSocket, buffer, strlen(buffer));
	printf("1.send\n2.recive\n");
	int v;
	scanf("%d",&v);
	bzero(buffer,256);
	int n = SSL_read(clientSocket,buffer,255);
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
			n=SSL_write(ssl,buffer, strlen(buffer));
			if (n < 0) 
             			perror("ERROR writing to socket");
			printf("message to be  delivered to k\n");
			bzero(buffer,256);
			n = SSL_read(ssl,buffer,255);
			if (n < 0) 
			     perror("ERROR reading from socket");
			printf("Server : %s\n",buffer);
			while(1)
			{
			printf("Msg:");
			bzero(buffer,256);
			scanf("%s",&buffer);
			SSL_write(ssl, buffer, strlen(buffer));	
			bzero(buffer,256);
			n = SSL_read(ssl,buffer,255);
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
			n = SSL_read(ssl,buffer,255);
			if (n < 0) 
			     perror("ERROR reading from socket");
			printf("U got a mseg: %s\n",buffer);
			printf("Send Msg:");
			bzero(buffer,256);
			scanf("%s",&buffer);
			//printf("send %s",buffer);
			n=SSL_write(ssl, buffer, strlen(buffer));
			//if (n < 0) 
			  //   perror("ERROR reading from socket");
			
			}
		}
	}
	SSL_free(ssl);
}
    close(clientSocket);         /* close socket */
    SSL_CTX_free(ctx); 


	return 0;
}
