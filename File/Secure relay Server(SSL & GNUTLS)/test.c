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
	
#include <gnutls/gnutls.h>

#define SECRET_KEY "Kanishak"
// This is the port number that the server will listen on.
#define PORT 8050
// GnuTLS log level. 9 is the most verbose.
#define LOG_LEVEL 0

#define TRUE 1 
#define FALSE 0 
	


int accept_one_connection(int port);
void error_exit(const char *msg);
// GnuTLS callbacks.
int psk_creds(gnutls_session_t, const char*, gnutls_datum_t*);
ssize_t data_push(gnutls_transport_ptr_t, const void*, size_t);
ssize_t data_pull(gnutls_transport_ptr_t, void*, size_t);
void print_logs(int, const char*);
void print_audit_logs(gnutls_session_t, const char*);





char substring(char s[]) {
      char sub;
      sub = s[0];
      return sub;
   
}

void init_gnutls(gnutls_session_t* session,gnutls_psk_server_credentials_t cred,int psk_creds)
{
int res;
res = gnutls_init(session, GNUTLS_SERVER);
	    if (res != GNUTLS_E_SUCCESS) {
		error_exit("gnutls_init() failed.\n");
	    }

		// Allocate the structure we use to tell GnuTLS what the credentials
    // (pre-shared key) are

    res = gnutls_credentials_set(session, GNUTLS_CRD_PSK, cred);
    if (res != 0) {
        error_exit("gnutls_credentials_set() failed.\n");
    }

    // Set the cipher suite priorities.
    // See section 6.10 of the GnuTLS manuals for a description of this string.
    // Run `gnutls-cli -l` to see what your GnuTLS supports.
    const char *error = NULL;
    // Here we allow all 128+ bit ciphers except RC4 and disable SSL3 and TLS1.0.

    res = gnutls_priority_set_direct(
        session,
        "SECURE128:-VERS-SSL3.0:-VERS-TLS1.0:-ARCFOUR-128:+PSK:+DHE-PSK",
        &error
    );
    if (res != GNUTLS_E_SUCCESS) {
        error_exit("gnutls_priority_set_direct() failed.\n");
    }

	
}
int main(int argc , char *argv[]) 
{ 
	int opt = TRUE; 
	int master_socket , addrlen , new_socket , client_socket[30] , 
		max_clients = 30 , activity, i , valread , sd; 
	int max_sd; 
	struct sockaddr_in address; 
	char name[30];
		
	char buffer[1025]; //data buffer of 1K 
		
	//set of socket descriptors 
	fd_set readfds; 
	int res;

	    // Initialize GnuTLS's global state. You only have to do this once.
	    gnutls_global_init();

	    // Enable logging (for debugging).
	    // Be careful with this in a real application. You don't want to reveal the
	    // logging information to any potential attackers.
	    gnutls_global_set_log_level(LOG_LEVEL);
	    gnutls_global_set_log_function(print_logs);
	    // Enable logging (for auditing).
	    // (commented out because it's not available in my version of GnuTLS)
	    // gnutls_global_set_audit_log_function(print_audit_logs);

	    // A GnuTLS session is like a socket for an SSL/TLS connection.
	    gnutls_session_t session[30];

	    // Initialize the session for our connection from the client.
for(int i=0;i<30;i++)
{
	    res = gnutls_init(&session[i], GNUTLS_SERVER);
	    if (res != GNUTLS_E_SUCCESS) {
		error_exit("gnutls_init() failed.\n");
	    }
}
		// Allocate the structure we use to tell GnuTLS what the credentials
    // (pre-shared key) are.
    gnutls_psk_server_credentials_t cred;
    res = gnutls_psk_allocate_server_credentials(&cred);
    if (res != 0) {
        error_exit("gnutls_psk_allocate_server_credentials() failed.\n");
    }
    // GnuTLS will call psk_creds to ask for the key associated with the
    // client's username.
    gnutls_psk_set_server_credentials_function(cred, psk_creds);
    // Pass the "credentials" to the GnuTLS session. GnuTLS does NOT make an
    // internal copy of the information, so we have to keep the 'cred' structure
    // in memory (and not modify it) until we're done with this session.
for(int i=0;i<30;i++)
{
    res = gnutls_credentials_set(session[i], GNUTLS_CRD_PSK, cred);
    if (res != 0) {
        error_exit("gnutls_credentials_set() failed.\n");
    }
}
    // Set the cipher suite priorities.
    // See section 6.10 of the GnuTLS manuals for a description of this string.
    // Run `gnutls-cli -l` to see what your GnuTLS supports.
    const char *error = NULL;
    // Here we allow all 128+ bit ciphers except RC4 and disable SSL3 and TLS1.0.
for(int i=0;i<30;i++)
{
    res = gnutls_priority_set_direct(
        session[i],
        "SECURE128:-VERS-SSL3.0:-VERS-TLS1.0:-ARCFOUR-128:+PSK:+DHE-PSK",
        &error
    );
    if (res != GNUTLS_E_SUCCESS) {
        error_exit("gnutls_priority_set_direct() failed.\n");
    }
}




	
















	
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
					int *connfdPtr = malloc(sizeof(int));
					    *connfdPtr = new_socket;
					    gnutls_transport_set_ptr(session[i], connfdPtr);

					    gnutls_transport_set_push_function(session[i], data_push);

					    gnutls_transport_set_pull_function(session[i], data_pull);

					    do {
						res = gnutls_handshake(session[i]);

					    } while ( res != 0 && !gnutls_error_is_fatal(res) );

					    if (gnutls_error_is_fatal(res)) {
						error_exit("Fatal error during handshake.\n");
					    } 
					client_socket[i] = new_socket; 
					printf("Adding to list of sockets as %d\n" , i); 
					bzero(buffer,256);
					valread = gnutls_record_recv(session[i] , buffer, 255);
					char q;
					q=substring(buffer);
					name[i]=q;
					printf("Name of client %c\n",name[i]);
					int n = gnutls_record_send(session[i],"OK",strlen("OK"));
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
				
				int n;
				if ((n = gnutls_record_recv(session[i], buffer, 255)) == 0) 
				{ 
					//Somebody disconnected , get his details and print 
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
					if (n < 0) 
			     				perror("ERROR reading from socket");
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
						n = gnutls_record_send(session[i],"not found",strlen("not found"));
		   				if (n < 0) perror("ERROR writing to socket");
					}
					else
					{	
						bzero(buffer,1025);
						sprintf(buffer, "MSG to be sent to ID %d",fd_id); 
						int w=fd_id;
						int rsd=client_socket[w];
						int n = gnutls_record_send(session[i],buffer,strlen(buffer));
					   	if (n < 0) perror("ERROR writing to socket");
						int words;
						int ch = 0;
						gnutls_record_recv(session[i], &words, sizeof(int));
						gnutls_record_send(session[w], &words, sizeof(int));
						printf("Passed integer is : %d\n" , words);      //Ignore , Line for Testing
						
						while(ch != words)
						{
							char c;
							 bzero(buffer,1025);
							 //read(sd, buffer , 1024); 
							// write(rsd,buffer,1024); 
							gnutls_record_recv(session[i],&c,sizeof(char)); 
							 gnutls_record_send(session[w],&c,sizeof(char));  
							printf(" %c",c); //Line for Testing , Ignore
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

int psk_creds(gnutls_session_t session, const char *username, gnutls_datum_t *key)
{
    // For this example, we ignore the username and return the same key every
    // time. In a real application, you would look up the key for the username
    // and return that. If the username does not exist, return a negative
    // number (see the manual).
    key->size = strlen(SECRET_KEY);
    key->data = gnutls_malloc(key->size);
    if (key->data == NULL) {
        return -1;
    }
    memcpy(key->data, SECRET_KEY, key->size);
    return 0;
}

// GnuTLS calls this function to send data through the transport layer. We set
// this callback with gnutls_transport_set_push_function(). It should behave
// like send() (see the manual for specifics).
ssize_t data_push(gnutls_transport_ptr_t ptr, const void* data, size_t len)
{
    int sockfd = *(int*)(ptr);
    return send(sockfd, data, len, 0);
}

// GnuTLS calls this function to receive data from the transport layer. We set
// this callback with gnutls_transport_set_pull_function(). It should act like
// recv() (see the manual for specifics).
ssize_t data_pull(gnutls_transport_ptr_t ptr, void* data, size_t maxlen)
{
    int sockfd = *(int*)(ptr);
    return recv(sockfd, data, maxlen, 0);
}

// GnuTLS will call this function whenever there is a new debugging log message.
void print_logs(int level, const char* msg)
{
    printf("GnuTLS [%d]: %s", level, msg);
}

// GnuTLS will call this function whenever there is a new audit log message.
void print_audit_logs(gnutls_session_t session, const char* message)
{
    printf("GnuTLS Audit: %s", message);
}

// Listens on 'port' for a TCP connection. Accepts at most one connection.

void error_exit(const char *msg) 
{
    printf("ERROR: %s", msg);
    exit(1);
}	
