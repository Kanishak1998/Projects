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
 
#define FAIL    -1
#define PORT 8050	

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
	    SSL_library_init();
	    ctx = InitCTX();
   	ssl = SSL_new(ctx);







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
	

	
	SSL_set_fd(ssl,clientSocket);
    	if ( SSL_connect(ssl) == FAIL )
	{
	   /* perform the connection */
        	ERR_print_errors_fp(stderr);
		printf("Connecte prob\n");
	}
	printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
	ShowCerts(ssl);



	printf("[+]Connected to Server.\n");
	printf("Choose your id:");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
	SSL_write(ssl, buffer, strlen(buffer));
	printf("1.send\n2.recive\n");
	int v;
	scanf("%d",&v);
	bzero(buffer,256);
	int n = SSL_read(ssl,buffer,255);
	if (n < 0) 
		perror("ERROR reading from socket");
	printf("Server says\"%s\"\n",buffer);
	
	switch(v)
	{
		case 1:
		{
			printf("Enter id to connect:\n");
			bzero(buffer,1025);
			scanf("%s",buffer);
			n=SSL_write(ssl,buffer, strlen(buffer));
			if (n < 0) 
             			perror("ERROR writing to socket");
			bzero(buffer,1025);
			n = SSL_read(ssl,buffer,1024);
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
				words++;	
			}
			printf("Words = %d \n"  , words);	//Ignore

			SSL_write(ssl, &words, sizeof(int));
			rewind(f);
			fseek(f, 0L, SEEK_END);    	// tells size of the file. Not rquired for the functionality in code.
			int sz = ftell(f);				//Just written for curiosity.
			printf("Size is %d \n" , sz);
			rewind(f);  
			char ch ;			
			while((ch = fgetc(f)) != EOF)
	      		{
				SSL_write(ssl,&ch,sizeof(char));
				//printf("%c",ch);
				
			}
	                fclose(f);
			printf("The file was sent successfully\n");
			break;
		}
		case 2:
		{
			FILE *fp;
			int ch = 0;
			fp = fopen("recv.txt","a");            
			int words;
			SSL_read(ssl, &words, sizeof(int));
			printf("Passed integer is : %d\n" , words);      //Ignore , Line for Testing
			while(ch != words)
			{
				char c;
				SSL_read(ssl,&c,sizeof(char));
				//printf("%c",c);  
				fputc(c,fp);
				 ch++;
			}
			//while(1){}
			fclose(fp);
			printf("The file was received successfully\n");
			printf("The new file created is recv.txt\n");
			break;
		}
	}
	SSL_free(ssl);
	close(clientSocket);         /* close socket */
	SSL_CTX_free(ctx); 

	return 0;
}
