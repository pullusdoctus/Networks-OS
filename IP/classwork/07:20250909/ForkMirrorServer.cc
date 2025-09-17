/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  *   Socket client/server example
  *
  * (Fedora version)
  *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>	// memset
#include <unistd.h>

#include "Socket.h"

#define PORT 1234
#define BUFSIZE 512

int main( int argc, char ** argv ) {
   VSocket * s1, * s2;
   int childpid;
   char a[ BUFSIZE ];

   s1 = new Socket( 's' );		// Create a stream IPv4 socket

   s1->Bind( PORT );			// Port to access this mirror server
   s1->MarkPassive( 5 );		// Set passive socket and backlog queue to 5 connections

   for( ; ; ) {
      s2 = s1->AcceptConnection();	// Wait for a new connection, connection info is in s2 variable
      childpid = fork();		// Create a child to serve the request
      if ( childpid < 0 ) {
         perror( "server: fork error" );
      } else {
         if (0 == childpid) {		// child code
            s1->Close();			// Close original socket "s1" in child
            memset( a, 0, BUFSIZE );
            s2->Read( a, BUFSIZE );	// Read a string from client using new conection info
            s2->Write( a );		// Write it back to client, this is the mirror function
            exit( 0 );			// Exit, finish child work
         }
      }

      s2->Close();			// Close socket s2 in parent, then go wait for a new conection

   }

}

