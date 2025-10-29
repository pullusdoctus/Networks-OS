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
int VSocket::BuildSocket( char t, bool IPv6 ){
   int st = -1;
   int type = -1;
   if (t == 's') {
      type = SOCK_STREAM;
   } else if (t == 'd') {
         type = SOCK_DGRAM;
   } else {
      throw std::runtime_error("VSocket::BuildSocket: Socket type not supported");
   }
   int domain = -1;
   this->IPv6 = IPv6;
   if (IPv6 == true) {
      domain = AF_INET6;
   } else {
      domain = AF_INET;
   }

   st = socket(domain, type, 0);
   if ( -1 == st ) {
      throw std::runtime_error(
        std::string("VSocket::BuildSocket failed: ") + strerror(errno));
   }
   
   this->idSocket = st;
   return st;
}

int VSocket::BuildSocket( int fd ){
   this->idSocket = fd;
   return fd;
}

/**
  * Class destructor
  *
 **/
VSocket::~VSocket() {

}


/**
  * Close method
  *    use Unix close system call (once opened a socket is managed like a file in Unix)
  *
 **/
void VSocket::Close(){
   int st = -1;
   if (this->idSocket < 0) {
      return;
   }
   printf("File descriptor to close %i\n", this->idSocket);
   st = close(this->idSocket);
   if ( -1 == st ) {
      throw std::runtime_error( std::string("VSocket::Close failed: ") + strerror(errno) );
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
   
   if (this->IPv6 == true) {
      sockaddr_in6 * addr6 = new sockaddr_in6;
      memset( addr6, 0, sizeof(sockaddr_in6) );
      addr6->sin6_family = AF_INET6;
      addr6->sin6_port = htons(port);
      if (inet_pton(AF_INET6, hostip, &addr6->sin6_addr) != 1) {
         delete(addr6);
         throw std::runtime_error( "VSocket::Invalid ipv6 network address." );
      }
      st = connect(this->idSocket, (const sockaddr*) addr6, sizeof(sockaddr_in6));
      delete(addr6);
   } else {
      sockaddr_in * addr = new sockaddr_in;
      
      memset( addr, 0, sizeof(sockaddr_in) );
      addr->sin_family = AF_INET;
      
      addr->sin_port = htons(port);
      
      if (inet_pton(AF_INET, hostip, &addr->sin_addr) != 1) {
         delete(addr);
         throw std::runtime_error( "VSocket::Invalid ipv4 network address." );
      }
      st = connect(this->idSocket, (const sockaddr*) addr, sizeof(sockaddr_in));
      delete(addr);
   }
   
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

   struct addrinfo hints, *result, *rp;
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
   hints.ai_socktype = SOCK_STREAM; /* Stream socket */
   hints.ai_flags = 0;
   hints.ai_protocol = 0;          /* Any protocol */

   st = getaddrinfo( host, service, &hints, &result );

   for ( rp = result; rp; rp = rp->ai_next ) {
      st = connect( this->idSocket, rp->ai_addr, rp->ai_addrlen );
      if ( 0 == st )
         break;
   }

   freeaddrinfo( result );

   if ( -1 == st ) {
      throw std::runtime_error( "VSocket::EstablishConnection" );
   }
   return st;
}

/**
  * Bind method
  *    use "bind" Unix system call (man 3 bind) (server mode)
  *
  * @param      int port: bind a unamed socket to a port defined in sockaddr structure
  *
  *  Links the calling process to a service at port
  *
 **/
int VSocket::Bind( int port ) {
   int st = -1;
   if (this->IPv6 == true) {
      sockaddr_in6 * addr6 = new sockaddr_in6;
      memset( addr6, 0, sizeof(sockaddr_in6) );
      addr6->sin6_family = AF_INET6;
      addr6->sin6_port = htons(port);
      st = bind(this->idSocket, (const sockaddr*) addr6, sizeof(sockaddr_in6));
      delete(addr6);
   } else {
      sockaddr_in * addr = new sockaddr_in;
      memset( addr, 0, sizeof(sockaddr_in) );
      addr->sin_family = AF_INET;
      addr->sin_addr.s_addr = htonl(INADDR_ANY);
      addr->sin_port = htons(port);
      memset(addr->sin_zero, '\0', sizeof (addr->sin_zero));
      st = bind(this->idSocket, (const sockaddr*) addr, sizeof(sockaddr_in));
      delete(addr);
   }
   
   if ( -1 == st ) {
      throw std::runtime_error(
         std::string("VSocket::Bind failed: ") + strerror(errno)
      );
   }

   return st;
}

/**
  * MarkPassive method
  *    use "listen" Unix system call (man listen) (server mode)
  *
  * @param      int backlog: defines the maximum length to which the queue of pending connections for this socket may grow
  *
  *  Establish socket queue length
  *
 **/
int VSocket::MarkPassive( int backlog ) {
   int st = -1;

   st = listen(this->idSocket, backlog);

   if ( -1 == st ) {
      throw std::runtime_error( "VSocket::MarkPassive" );
   }

   return st;

}

/**
  * WaitForConnection method
  *    use "accept" Unix system call (man 3 accept) (server mode)
  *
  *
  *  Waits for a peer connections, return a sockfd of the connecting peer
  *
 **/
int VSocket::WaitForConnection( void ) {
   int st = -1;
   socklen_t addrlen = 0;
   if (this->IPv6 == true) {
      sockaddr_in6 * addr6 = new sockaddr_in6;
      addrlen = sizeof(sockaddr_in6);
      memset( addr6, 0, sizeof(sockaddr_in6) );
      st = accept(this->idSocket, (sockaddr*) addr6, &addrlen);
      delete(addr6);
   } else {
      sockaddr_in * addr = new sockaddr_in;
      addrlen = sizeof(sockaddr_in);
      memset( addr, 0, sizeof(sockaddr_in) );
      st = accept(this->idSocket, (sockaddr*) addr, &addrlen);
      delete(addr);
   }

   if ( -1 == st ) {
      throw std::runtime_error( "VSocket::WaitForConnection" );
   }

   return st;

}

/**
  * Shutdown method
  *    use "shutdown" Unix system call (man 3 shutdown) (server mode)
  *
  *
  *  cause all or part of a full-duplex connection on the socket associated with the file descriptor socket to be shut down
  *
 **/
int VSocket::Shutdown( int mode ) {
   int st = -1;

   st = shutdown(this->idSocket, mode);

   if ( -1 == st ) {
      throw std::runtime_error( "VSocket::Shutdown" );
   }
   
   return st;

}

// UDP methods 2025

/**
  *  sendTo method
  *
  *  @param	const void * buffer: data to send
  *  @param	size_t size data size to send
  *  @param	void * addr address to send data
  *
  *  Send data to another network point (addr) without connection (Datagram)
  *
 **/
size_t VSocket::sendTo( const void * buffer, size_t size, void * addr ) {
   int st = -1;

   return st;

}

/**
  *  recvFrom method
  *
  *  @param	const void * buffer: data to send
  *  @param	size_t size data size to send
  *  @param	void * addr address to receive from data
  *
  *  @return	size_t bytes received
  *
  *  Receive data from another network point (addr) without connection (Datagram)
  *
 **/
size_t VSocket::recvFrom( void * buffer, size_t size, void * addr ) {
   int st = -1;
   socklen_t addrlen = 0;
   if (this->IPv6 == true) {
      addrlen = sizeof(sockaddr_in6);
      st = recvfrom(this->idSocket, buffer, size, 0, (sockaddr*) addr,
            &addrlen);
   } else {
      addrlen = sizeof(sockaddr_in);
      st = recvfrom(this->idSocket, buffer, size, 0, (sockaddr*) addr,
            &addrlen);
   }
   
   if ( -1 == st ) {
      throw std::runtime_error( "VSocket::RecvFrom" );
   }

   return st;
}

