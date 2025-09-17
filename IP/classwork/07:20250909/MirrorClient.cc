/**
 *   UCR-ECCI
 *   CI-0123 Proyecto integrador de redes y sistemas operativos
 *
 *   Socket client/server example
 *
 *   Deben determinar la dirección IP del equipo donde van a correr el servidor
 *   para hacer la conexión en ese punto (ip addr)
 *
 **/

#include <stdio.h>
#include <cstring>
#include "Socket.h"

#define PORT 1234
#define BUFSIZE 512

int main( int argc, char ** argv ) {
   VSocket * s;
   char buffer[ BUFSIZE ];

   s = new Socket( 's' );     // Creaite a new stream IPv4 socket
   memset( buffer, 0, BUFSIZE );	// Zero fill buffer

   s->MakeConnection("192.168.18.2", PORT ); // Same port as server
   if ( argc > 1 ) {
      s->Write( argv[1] );		// If provided, send first program argument to server
   } else {
      s->Write( "Hello world 2025 ..." );
   }
   s->Read( buffer, BUFSIZE );	// Read answer sent back from server
   printf( "%s", buffer );	// Print received string, mirror example this will print same sent string
}

