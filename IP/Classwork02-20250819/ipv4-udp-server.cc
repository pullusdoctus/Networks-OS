/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  ****** Socket class interface
  *
  * (Fedora version)
  *
  *  Server-side implementation of UDP client-server model	
  *
 **/

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "VSocket.h" 
#include "Socket.h" 

#define PORT	1234 
#define MAXLINE 1024 

int main() { 
   VSocket * server;
   int len, n; 
   int sockfd;
   struct sockaddr other;
   char buffer[MAXLINE]; 
   char *hello = (char *) "Hello from CI0123 server"; 
	
   server = new Socket( 'd', false );
   server->Bind( PORT );

   memset( &other, 0, sizeof( other ) );

   n = server->recvFrom( (void *) buffer, MAXLINE, (void *) &other );	// Mensaje de los www servers
   buffer[n] = '\0'; 
   printf("Server: message received: %s\n", buffer);

   server->sendTo( (const void *) hello, strlen( hello ), (void *) &other );
   printf("Server: Hello message sent.\n"); 

   server->Close();
   
   return 0;

} 

