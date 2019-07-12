servergnutls:
	gcc -Dsecure=1 -Wall -c server.c -o servergnutls.o `pkg-config gnutls --cflags` 
	gcc -Dsecure=1 servergnutls.o -o servergnutls `pkg-config gnutls --libs` 
serverssl:
	gcc -Dsecure=0 -Wall -o  serverssl server.c -L/usr/lib -lssl -lcrypto 
sslclient:
	gcc -Wall -o sslclient  sslclient.c -L/usr/lib -lssl -lcrypto
gnutlsclient:		
	gcc -Wall -c gnuclient.c -o gnuclient.o `pkg-config gnutls --cflags`
	gcc gnuclient.o -o gnuclient `pkg-config gnutls --libs`
	
clean:
	rm *.o output


gcc -Wall -c server.c -o servergnutls.o `pkg-config gnutls --cflags` -L/usr/lib -lssl -lcrypto 
