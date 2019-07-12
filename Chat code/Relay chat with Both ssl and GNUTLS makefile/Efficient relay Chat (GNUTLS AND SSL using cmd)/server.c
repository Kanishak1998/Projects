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



//running termial lib and command
//gcc -Wall -c test.c -o test.o `pkg-config gnutls --cflags` -L/usr/lib -lssl -lcrypto 
//gcc test.o -o test `pkg-config gnutls --libs` -L/usr/lib -lssl -lcrypto 

max_clients = 30;


//strc for ssl enc
struct sslenc
{
	SSL *ssl;
	SSL_CTX *ctx;
};

//union to decide b/w them

union session
{
	gnutls_session_t gn;
	struct sslenc *ssle;
};

/*
Name:psk_creds
param:key pointer to store a preshared secret key
desc:For this we ignore the username and return the same key every
    time. In a real application, you would look up the key for the username
    and return that. If the username does not exist, return a negative
    number .
return:0
*/

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


/*
Name:data_push
param:gnutls transport pointer and data maximum length
desc:GnuTLS calls this function to send data through the transport layer. We set
   this callback with gnutls_transport_set_push_function(). It should behave like send().
return:integer  ,0 if fail. 
*/
ssize_t data_push(gnutls_transport_ptr_t ptr, const void* data, size_t len)
{
    int sockfd = *(int*)(ptr);
    return send(sockfd, data, len, 0);
}
/*
Name:data_pull
param:gnutls transport pointer and data maximum length
desc:GnuTLS calls this function to receive data from the transport layer. We set
   this callback with gnutls_transport_set_pull_function(). It should act like
   recv() (see the manual for specifics).
return:integer  ,0 if fail. 
*/

ssize_t data_pull(gnutls_transport_ptr_t ptr, void* data, size_t maxlen)
{
    int sockfd = *(int*)(ptr);
    return recv(sockfd, data, maxlen, 0);
}
/*
Name:print_logs
param:level and a string msg
desc:GnuTLS will call this function whenever there is a new debugging log message.
return:NULL. 
*/
void print_logs(int level, const char* msg)
{
    printf("GnuTLS [%d]: %s", level, msg);
}
/*
Name:print_audit_logs
param:gnutls session and string message
desc:GnuTLS will call this function whenever there is a new audit log message.
return:NULL. 
*/
void print_audit_logs(gnutls_session_t gnutls, const char* message)
{
    printf("GnuTLS Audit: %s", message);
}
void error_exit(const char *msg) 
{
    printf("ERROR: %s", msg);
    exit(1);
}
/*
Name: InitCTX
param:NULL
Desc: load algorithms and create a context for each client
Return:context for each client
*/
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
 
/*
Name:LoadCertificates 
Desc:get the server's certificate and add send it to the client 
return:NULL
*/
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
/*
Name:ShowCerts
param:SSL pointer arg pointing to the client callin the function.
Desc:get the server's certificate and add send it to the client 
return:NULL
*/ 
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
/*
Name:ShowCerts
param:char array of sting 
Desc:substing to get inial letter of fro the client as id
return:the starting char of the string
*/
char substring(char s[]) {
      char sub;
      sub = s[0];
      return sub;
}
/*
Name:openconnection
param:NULL
desc:establish the connection between the server and the clients
return:file rescripted for the socket
*/
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

//init the Gnutls or SSL
/*
Name:start
param:wheather gnutls or ssl
desc:init gnu tls or the ssl
return:union session either for the ssl or gnutls
*/ 
union session start(int secure)
{
	union session security;
	if(secure==1)//gnutls
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
	else//ssl
	{
		struct sslenc* sec=NULL;
		sec=(struct sslenc*)malloc(sizeof(struct sslenc));
		sec->ctx= InitServerCTX();
		LoadCertificates(sec->ctx, "mycert.pem", "mycert.pem");
		security.ssle=sec;
	}
	return security;
	
}
/*
Name:send_init
param:uion for either ssl or gnutls
desc://communication init... using gnutls or ssl
return:NULL
*/

void send_init(union session security,int new_socket,int secure)
{
	//printf("ssl1\n");
	if(secure==1)//gnutls
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
	else//ssl
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
/*
Name:send_secure and recv_secure
param:uion for either ssl or gnutls,buffer msg,ssecutrity protocol
desc:send and recv using gnutls secureity or ssl
return:NULL
*/
void send_secure(union session security,char buffer[256],int secure)
{
	int n;
	if(secure==1)
	{
		int n = gnutls_record_send(security.gn,buffer,strlen(buffer));
	}
	else
	{
		int n = SSL_write(security.ssle->ssl,buffer,strlen(buffer));
	}				
	if (n < 0) perror("ERROR writing to socket");

}

void recv_secure(union session security,char buffer[256],int secure)
{
	if(secure==1)
	{
		//	printf("got it");
		gnutls_record_recv(security.gn,buffer, 255);
	}
	else
	{
		SSL_read(security.ssle->ssl,buffer,255);
	}

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
	//decide weather gnutls of ssl selected 
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
	//init gnutls
	if(secure==1)
	{
		int res;
		printf("GnuTLS Encrypted\n");
		gnutls_global_init();
		gnutls_global_set_log_level(LOG_LEVEL);
		gnutls_global_set_log_function(print_logs);
	}
	//or ssl
	else
	{
		printf("SSL encrypted\n");
		SSL_library_init();
		
	}
	//union call init.. of security protocol
	for(int i=0;i<30;i++)
	{
		security[i]=start(secure);
	}
	//max 30 clinets
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
					send_init(security[i],new_socket,secure);//init the security protocol in the file descripter
					client_socket[i] = new_socket; 
					printf("Adding to list of sockets as %d\n" , i); 
					//printf("hey0\n");
					bzero(buffer,256);
//					gnutls_record_recv(security[i].gn,buffer, 255);		
					recv_secure(security[i], buffer,secure);
					//int p=SSL_read(security[i].ssle.ssl,buffer,255);
					//if (p < 0) perror("ERROR writing to socket");		
					//take the initial letter as the id of the user					
					char q;
					q=substring(buffer);
					name[i]=q;
					printf("Name of client %c\n",name[i]);
					int n;
					//send ok msg
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
//else its some IO operation on some other socket 
		
		for (i = 0; i < max_clients; i++) 
		{ 
			sd = client_socket[i]; 
			bzero(buffer,256);
			//if any activity on the server 
			if (FD_ISSET( sd , &readfds)) 
			{ 
				int n;
				//char q;
				recv_secure(security[i], buffer,secure);
				printf("mesfID:%s\n",buffer);
				char q=substring(buffer);
				char id=q;
				int fd_id=-1;
				//recv id to send on 
				for(int j=0;j<max_clients; j++)
				{
					if(name[j]==id)
					{
						fd_id=j;	
					}
				}
				//if id not found send not found
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
					bzero(buffer,256);
					sprintf(buffer, "MSG to be sent to ID %d",fd_id); 
					//store reciver id in w
					int w=fd_id;
					int rsd=client_socket[w];
					send_secure(security[i],buffer,secure);
					while(1)
					{
				
						bzero(buffer,256);
						recv_secure(security[i], buffer,secure);
						//rec msg from the init client
						rsd=client_socket[w];
						char buf[256];
						sprintf(buf,"from \"%c\":%s",name[w],buffer); 
						//forward the msg to the reciver client
						send_secure(security[w],buf,secure);
						fputs("Message delivered\n", stdout);				
						if (n < 0) perror("ERROR writing to socket");
						bzero(buffer,256);
						//take reply from the reciver
						recv_secure(security[w], buffer,secure);
						fputs("Message reciverd\n", stdout);
						sd = client_socket[i]; 
						//forward it to the sender
						send_secure(security[i],buffer,secure);
						fputs("Message fowarded to host\n", stdout);
					}//do until sendder disconnect

				}
			} 
		} 
	} 
	return 0; 
} 

