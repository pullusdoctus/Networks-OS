/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  ****** VSocket base class implementation
  *
  * (Fedora version)
  *
 **/

#include <sys/socket.h>
#include <arpa/inet.h>		// ntohs, htons
#include <stdexcept>            // runtime_error
#include <cstring>		// memset
#include <netdb.h>			// getaddrinfo, freeaddrinfo
#include <unistd.h>			// close
/*
#include <cstddef>
#include <cstdio>

//#include <sys/types.h>
*/
#include "VSocket.h"

VSocket::VSocket(char type, bool IPv6)
   : type(type), IPv6(IPv6), port(-1), idSocket(-1) {
   this->BuildSocket(type, IPv6);
}

/**
  *  Class creator (constructor)
  *     use Unix socket system call
  *
  *  @param     char t: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param     bool ipv6: if we need a IPv6 socket
  *
 **/
void VSocket::BuildSocket( char t, bool IPv6 ){

   int st = -1;

   int domain = -1;
   int type = -1;

   if (IPv6) { 
      domain = AF_INET6;
   } else {  // IPv4
      domain = AF_INET;
   }

   if (t == 's') {
      type = SOCK_STREAM;
   } else if (t == 'd') {
      type = SOCK_DGRAM;
   } else {
      // TODO: error handling
      type = SOCK_STREAM;
   }
   
   if (domain != -1 && type != -1) st = socket(domain, type, 0);

   if ( -1 == st ) {
      throw std::runtime_error( "VSocket::BuildSocket, (reason)" );
   }

   this->idSocket = st;
}


/**
  * Class destructor
  *
 **/
VSocket::~VSocket() {

   this->Close();

}


/**
  * Close method
  *    use Unix close system call (once opened a socket is managed like a file in Unix)
  *
 **/
void VSocket::Close(){

   int st = close(this->idSocket);

   if ( -1 == st ) {
      throw std::runtime_error( "VSocket::Close()" );
   }

   this->idSocket = -1;

}


/**
  * EstablishConnection method
  *   use "connect" Unix system call
  *
  * @param      char * host: host address in dot notation, example "10.84.166.62"
  * @param      int port: process address, example 80
  *
 **/
int VSocket::EstablishConnection( const char * hostip, int port ) {

   int st = -1;

   if ( -1 == st ) {
      throw std::runtime_error( "VSocket::EstablishConnection" );
   }

   return st;

}


/**
  * EstablishConnection method
  *   use "connect" Unix system call
  *
  * @param      char * host: host address in dns notation, example "os.ecci.ucr.ac.cr"
  * @param      char * service: process address, example "http"
  *
 **/
int VSocket::EstablishConnection( const char *host, const char *service ) {
   int st = -1;

   throw std::runtime_error( "VSocket::EstablishConnection" );

   return st;

}

