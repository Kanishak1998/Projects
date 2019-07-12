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
#include <gnutls/gnutls.h>

#define SECRET_KEY "Kanishak"
// This is the port number that the server will listen on.
#define PORT 8050
// GnuTLS log level. 9 is the most verbose.
#define LOG_LEVEL 0


#define FAIL    -1

#define TRUE 1 
#define FALSE 0 


//gcc -Wall -c server.c -o server.o `pkg-config gnutls --cflags` -L/usr/lib -lssl -lcrypto 
//gcc server.o -o server `pkg-config gnutls --libs` -L/usr/lib -lssl -lcrypto 

int max_clients = 30;

struct sslenc
{
	SSL *ssl;
	SSL_CTX *ctx;
};

union session
{
	gnutls_session_t gn;
	struct sslenc *ssle;
};


int psk_creds(gnutls_session_t gnutls, const char *username, gnutls_datum_t *key)
{

    key->size = strlen(SECRET_KEY);
    key->data = gnutls_malloc(key->size);
    if (key->data == NULL) {
        return -1;
    }
    memcpy(key->data, SECRET_KEY, key->size);
    return 0;
}

ssize_t data_push(gnutls_transport_ptr_t ptr, const void* data, size_t len)
{
    int sockfd = *(int*)(ptr);
    return send(sockfd, data, len, 0);
}
ssize_t data_pull(gnutls_transport_ptr_t ptr, void* data, size_t maxlen)
{
    int sockfd = *(int*)(ptr);
    return recv(sockfd, data, maxlen, 0);
}
void print_logs(int level, const char* msg)
{
    printf("GnuTLS [%d]: %s", level, msg);
}
void print_audit_logs(gnutls_session_t gnutls, const char* message)
{
    printf("GnuTLS Audit: %s", message);
}
void error_exit(const char *msg) 
{
    printf("ERROR: %s", msg);
    exit(1);
}

SSL_CTX* InitServerCTX(void)
{   const SSL_METHOD *method;
    SSL_CTX *ctx;
 
    OpenSSL_add_all_algorithms();  
    SSL_load_error_strings();  
    method = TLSv1_2_server_method();  
    ctx = SSL_CTX_new(method);   
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

char substring(char s[]) {
      char sub;
      sub = s[0];
      return sub;
   
}

int openconnection()
{
	int master_socket;
	int opt = TRUE; 
	struct sockaddr_in address; 
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 

	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,sizeof(opt)) < 0 ) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("Listener on port %d \n", PORT); 
	if (listen(master_socket, 30) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	}
	return master_socket;
}


union session start(int secure)
{
	union session security;
	if(secure==1)
	{
		int res;
		//gnutls_session_t gnutls;
		res = gnutls_init(&security.gn, GNUTLS_SERVER);
		if (res != GNUTLS_E_SUCCESS) {
		error_exit("gnutls_init() failed.\n");
		}
		gnutls_psk_server_credentials_t cred;
		res = gnutls_psk_allocate_server_credentials(&cred);
		if (res != 0) {
		error_exit("gnutls_psk_allocate_server_credentials() failed.\n");
		}
		gnutls_psk_set_server_credentials_function(cred, psk_creds);
		res = gnutls_credentials_set(security.gn, GNUTLS_CRD_PSK, cred);
		if (res != 0) {
		error_exit("gnutls_credentials_set() failed.\n");
		}
		const char *error = NULL;
		res = gnutls_priority_set_direct(
		security.gn,
		"SECURE128:-VERS-SSL3.0:-VERS-TLS1.0:-ARCFOUR-128:+PSK:+DHE-PSK",
		&error
		);
		if (res != GNUTLS_E_SUCCESS) {
		error_exit("gnutls_priority_set_direct() failed.\n");
		}
	}
	else
	{
		struct sslenc* sec=NULL;
		sec=(struct sslenc*)malloc(sizeof(struct sslenc));
		sec->ctx= InitServerCTX();
		LoadCertificates(sec->ctx, "mycert.pem", "mycert.pem");
		security.ssle=sec;
	}
	return security;
	
}
void send_init(union session security,int new_socket,int secure)
{
	//printf("ssl1\n");
	if(secure==1)
	{
		int res;
		int *connfdPtr = malloc(sizeof(int));
		*connfdPtr = new_socket;
		gnutls_transport_set_ptr(security.gn, connfdPtr);

		gnutls_transport_set_push_function(security.gn, data_push);

		gnutls_transport_set_pull_function(security.gn, data_pull);

		do {
		res = gnutls_handshake(security.gn);

		} while ( res != 0 && !gnutls_error_is_fatal(res) );

		if (gnutls_error_is_fatal(res)) {
		error_exit("Fatal error during handshake.\n");
		} 
	}
	else
	{
		//printf("ssl2\n");
		security.ssle->ssl= SSL_new(security.ssle->ctx);           
		SSL_set_fd(security.ssle->ssl,new_socket); 
		if ( SSL_accept(security.ssle->ssl) == FAIL ) 
		{
			printf("pron in  conn");
			ERR_print_errors_fp(stderr);
		}
	}
}


int send_secure(union session security,char buffer[256],int size,int secure)
{
	int n;
	if(secure==1)
	{
		n = gnutls_record_send(security.gn,buffer,size);
	}
	else
	{
		n = SSL_write(security.ssle->ssl,buffer,size);
	}				
	return n;

}
int recv_secure(union session security,char buffer[256],int size,int secure)
{
	int n;
	if(secure==1)
	{
		//	printf("got it");
		n=gnutls_record_recv(security.gn,buffer,size);
	}
	else
	{
		n=SSL_read(security.ssle->ssl,buffer,size);
	}
	return n;
}


int main(int argc , char *argv[]) 
{ 

	int master_socket , addrlen , new_socket , client_socket[max_clients],activity, i, sd; 
	int max_sd; 
	struct sockaddr_in address; 
	char name[max_clients];
	//gnutls_session_t gnutls[30];
        //SSL_CTX *ctx[30];
	//SSL *ssl[30];
	union session security[max_clients];
	char buffer[1025]; 	
	fd_set readfds; 
	int secure=0;
	if(argc<2)
	{
		printf("No protocol provided running with Ssl Encrption\n");
	
	}
	else if(strcmp(argv[1],"gnutls")==0)
	{
		printf("Gnutls Started\n");
		secure=1;
	}
	else if(strcmp(argv[1],"ssl")==0)
	{
		printf("Ssl started\n");
		secure=0;
	}
	if(secure==1)
	{
		
		printf("GnuTLS Encrypted\n");
		gnutls_global_init();
		gnutls_global_set_log_level(LOG_LEVEL);
		gnutls_global_set_log_function(print_logs);
	}
	else
	{
		printf("SSL encrypted\n");
		SSL_library_init();
		
	}
	for(int i=0;i<30;i++)
	{
		security[i]=start(secure);
	}

	for (i = 0; i < max_clients; i++) 
	{ 
		client_socket[i] = 0; 
	}
	master_socket=openconnection(); 
	addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 
	while(TRUE) 
	{ 
		FD_ZERO(&readfds); 
		FD_SET(master_socket, &readfds); 
		max_sd = master_socket; 
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
	
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} 
		if (FD_ISSET(master_socket, &readfds)) 
		{ 
			if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			} 
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
				(address.sin_port)); 
			//printf("ssl0\n");
			for (i = 0; i < max_clients; i++) 
			{ 
				//if position is empty 
				if( client_socket[i] == 0 ) 
				{ 					
					send_init(security[i],new_socket,secure);
					client_socket[i] = new_socket; 
					printf("Adding to list of sockets as %d\n" , i); 
					//printf("hey0\n");
					bzero(buffer,256);
//					gnutls_record_recv(security[i].gn,buffer, 255);		
					recv_secure(security[i], buffer,255,secure);
					//int p=SSL_read(security[i].ssle.ssl,buffer,255);
					//if (p < 0) perror("ERROR writing to socket");		
					char q;
					q=substring(buffer);
					name[i]=q;
					printf("Name of client %c\n",name[i]);
					int n;
					if(secure==1)
					{
						n = gnutls_record_send(security[i].gn,"OK",strlen("OK"));
					}
					else
					{
						n = SSL_write(security[i].ssle->ssl ,"OK",strlen("OK"));
					}
			   		if (n < 0) perror("ERROR writing to socket");

					break; 
				} 
			} 
		} 

		for (i = 0; i < max_clients; i++) 
		{ 
			sd = client_socket[i]; 
			bzero(buffer,256);
			if (FD_ISSET( sd , &readfds)) 
			{ 
				int n;
				if((recv_secure(security[i],buffer,255,secure))==0)
				{
					getpeername(sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen); 
					printf("Host disconnected , ip %s , port %d \n" , 
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
					
					//Close the socket and mark as 0 in list for reuse 
					close( sd ); 
					
					client_socket[i] = 0; 
					name[i]='!';	
				}
				else
				{				
					char q;
					q=substring(buffer);
					char id=q;
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
						if(secure==1)
						{
							n = gnutls_record_send(security[i].gn,"not found",strlen("not found"));
			   				if (n < 0) perror("ERROR writing to socket");
						}
						else
						{
							n = SSL_write(security[i].ssle->ssl,"not found",strlen("not found"));
			   				if (n < 0) perror("ERROR writing to socket");
						}
					}
					else
					{	
						bzero(buffer,1025);
						sprintf(buffer, "MSG to be sent to ID %d",fd_id); 
						int w=fd_id;
						send_secure(security[i],buffer,strlen(buffer),secure);
					   	if (n < 0) perror("ERROR writing to socket");
						int words;
						int ch = 0;
						recv_secure(security[i], &words, sizeof(int),secure);
						send_secure(security[w], &words, sizeof(int),secure);
						printf("Passed integer is : %d\n" , words);      //Ignore , Line for Testing
						while(ch != words)
						{
							char c;
							 bzero(buffer,1025);
							 recv_secure(security[i],&c,sizeof(char),secure); 
							 send_secure(security[w],&c,sizeof(char),secure);  
							 ch++;
						}
						printf("The file Forwarded successfully\n");
					}
				}
			} 
		} 
	} 
	return 0; 
} 
