/*
 *  Ejemplo de sockets con IPv4
 *
 */

#include <stdio.h>
#include <string.h>

#include "VSocket.h"
#include "Socket.h"

int main( int argc, char * argv[] ) {
   const char * os = "http://os.ecci.ucr.ac.cr/";
   const char * osi = "10.84.166.62";
   const char * ose = "163.178.104.62";
   const char * request = "GET /pirofs/index.php?disk=Disk-01&cmd=ls HTTP/1.1\r\nhost: redes.ecci\r\n\r\n";

   VSocket * s;	
   char a[512];

   s = new Socket( 's' );
   s->MakeConnection( osi, 80 );
   s->Write(  request );
   s->Read( a, 512 );
   printf( "%s\n", a);

}

