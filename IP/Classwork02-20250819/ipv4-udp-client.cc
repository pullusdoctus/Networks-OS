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
  *   Client side implementation of UDP client-server model 
  *
 **/

#include <stdio.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>

#include "VSocket.h"
#include "Socket.h"

#define PORT    1234 
#define MAXLINE 1024 

int main() {
   VSocket * client;
   int sockfd; 
   int n, len; 
   char buffer[MAXLINE]; 
   char *hello = (char *) "Hello from CI0123 client"; 
   struct sockaddr_in other;

   client = new Socket( 'd' );	// Creates an UDP socket: datagram

   memset( &other, 0, sizeof( other ) ); 
   
   other.sin_family = AF_INET; 
   other.sin_port = htons( PORT ); 
   n = inet_pton( AF_INET, "10.1.35.50", &other.sin_addr );	// IP address to test our client with a Python server on lab 3-5
   if ( 1 != n ) {
      printf( "Error converting from IP address\n" );
      exit( 23 );
   }

   n = client->sendTo( (void *) hello, strlen( hello ), (void *) & other ); 
   printf("Client: Hello message sent.\n"); 
   
   n = client->recvFrom( (void *) buffer, MAXLINE, (void *) & other );
   buffer[n] = '\0'; 
   printf("Client message received: %s\n", buffer); 

   client->Close(); 

   return 0;
 
} 

