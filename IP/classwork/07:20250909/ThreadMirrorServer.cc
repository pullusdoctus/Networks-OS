/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  *   Socket client/server example with threads
  *
  * (Fedora version)
  *
 **/
 
#include <iostream>
#include <thread>

#include "Socket.h"

#define PORT 1234
#define BUFSIZE 512


/**
 *   Task each new thread will run
 *      Read string from socket
 *      Write it back to client
 *
 **/
void task( VSocket * client ) {
   char a[ BUFSIZE ];

   client->Read( a, BUFSIZE );	// Read a string from client, data will be limited by BUFSIZE bytes
   std::cout << "Server received: " << a << std::endl;
   client->Write( a );		// Write it back to client, this is the mirror function
   client->Close();		// Close socket in parent

}


/**
 *   Create server code
 *      Infinite for
 *         Wait for client conection
 *         Spawn a new thread to handle client request
 *
 **/
int main( int argc, char ** argv ) {
   std::thread * worker;
   VSocket * s1, * client;

   s1 = new Socket( 's' );

   s1->Bind( PORT );		// Port to access this mirror server
   s1->MarkPassive( 5 );	// Set socket passive and backlog queue to 5 connections

   for( ; ; ) {
      client = s1->AcceptConnection();	 	// Wait for a client connection
      worker = new std::thread( task, client );
   }

}

