/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  ****** SSLSocket example, server code
  *
  * (Fedora version)
  *
 **/

#include <thread>
#include <cstdlib>	// atoi
#include <cstdio>	// printf
#include <cstring>	// strlen, strcmp


#include "SSLSocket.h"

#define PORT	4321


void Service( SSLSocket * client ) {
   char buf[ 1024 ] = { 0 };
   int sd, bytes;
   const char* ServerResponse="\n<Body>\n\
\t<Server>os.ecci.ucr.ac.cr</Server>\n\
\t<dir>ci0123</dir>\n\
\t<Name>Proyecto Integrador Redes y sistemas Operativos</Name>\n\
\t<NickName>PIRO</NickName>\n\
\t<Description>Consolidar e integrar los conocimientos de redes y sistemas operativos</Description>\n\
\t<Author>profesores PIRO</Author>\n\
</Body>\n";
    const char *validMessage = "\n<Body>\n\
\t<UserName>piro</UserName>\n\
\t<Password>ci0123</Password>\n\
</Body>\n";

   client->AcceptSSL();
   client->ShowCerts();

   bytes = client->Read( buf, sizeof( buf ) );
   buf[ bytes ] = '\0';
   printf( "Client msg: \"%s\"\n", buf );

   if ( ! strcmp( validMessage, buf ) ) {
      client->Write( ServerResponse, strlen( ServerResponse ) );
   } else {
      client->Write( "Invalid Message", strlen( "Invalid Message" ) );
   }

   client->Close();

}


int main( int cuantos, char ** argumentos ) {
   SSLSocket * server, * client;
   std::thread * worker;
   int port = PORT;
   
   if ( cuantos > 1 ) {
      port = atoi( argumentos[ 1 ] );
   }

   server = new SSLSocket( (const char *) "ci0123.pem", (const char *) "key0123.pem", false );
   server->Bind( port );
   server->MarkPassive( 10 );

   for( ; ; ) {
      client = (SSLSocket * ) server->AcceptConnection();
      client->Copy( server );
      worker = new std::thread( Service, client );	// service connection
   }

}


