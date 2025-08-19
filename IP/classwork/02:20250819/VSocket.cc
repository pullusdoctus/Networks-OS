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
 **/

#include <sys/socket.h>
#include <arpa/inet.h>		// ntohs, htons
#include <stdexcept>            // runtime_error
#include <cstring>		// memset
#include <netdb.h>			// getaddrinfo, freeaddrinfo
#include <unistd.h>			// close
// #include <cstddef>
// #include <cstdio>
// #include <sys/types.h>
#include "VSocket.h"

/**
  *  Class constructor
  *     use Unix socket system call
  *
  *  @param     char type: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param     bool ipv6: if we need a IPv6 socket
 **/
void VSocket::BuildSocket(char type, bool IPv6) {
  this->type = type;
  this->IPv6 = IPv6;
  int domain = -1;
  int typeCode = -1;
  if (IPv6) {
    domain = AF_INET6;
  } else {  // IPv4
    domain = AF_INET;
  }
  if (type == 's') {
    typeCode = SOCK_STREAM;
  } else if (type == 'd') {
    typeCode = SOCK_DGRAM;
  } else {
    throw std::runtime_error("VSocket::BuildSocket - Invalid socket type");
  }
  int st = -1;
  if (domain != -1 && typeCode != -1) {
    st = socket(domain, typeCode, 0);
  }
  if (st == -1) {
    throw std::runtime_error(
      "VSocket::BuildSocket - Socket building unsuccesful");
  }
  this->idSocket = st;
}

/**
  * Class destructor
 **/
VSocket::~VSocket() { this->Close(); }

/**
  * Close method
  *    use Unix close system call (once opened a socket is managed like a file in Unix)
 **/
void VSocket::Close() {
  if (this->idSocket != -1) {
    int st = close(this->idSocket);
    if (-1 == st) {
      throw std::runtime_error("VSocket::Close");
    }
    this->idSocket = -1;
  }
  this->IPv6 = false;
  this->port = -1;
  this->type = '\0';
}

/**
  * EstablishConnection method
  *   use "connect" Unix system call
  *
  * @param      char* host: host address in dot notation, example "10.84.166.62"
  * @param      int port: process address, example 80
 **/
int VSocket::EstablishConnection(const char* host, int port) {
  int st = -1;
  if (this->IPv6) st = this->EstablishIPv6(st, host, port);
  else st = this->EstablishIPv4(st, host, port);
  if (st == -1) {
    throw std::runtime_error(
      "VSocket::EstablishConnection - Connection failed");
  }
  this->port = port;
  return st;
}

int VSocket::EstablishIPv4(int st, const char* host, int port) {
  struct sockaddr_in host4;
  memset((char*)&host4, 0, sizeof(host4));
  host4.sin_family = AF_INET;
  st = inet_pton(AF_INET, host, &host4.sin_addr);
  if (st == -1 || st == 0) {
    throw std::runtime_error("VSocket::EstablishIPv4 - inet_pton IPV4 failed");
  }
  host4.sin_port = htons(port);
  st = connect(this->idSocket, (sockaddr*)&host4, sizeof(host4));
  return st;
}

int VSocket::EstablishIPv6(int st, const char* host, int port) {
  struct sockaddr_in6 host6;
  memset((char*)&host6, 0, sizeof(host6));
  host6.sin6_family = AF_INET6;
  st = inet_pton(AF_INET6, host, &host6.sin6_addr);
  if (st == -1 || st == 0) {
    throw std::runtime_error("VSocket::EstablishIPv6 - inet_pton IPv6 failed");
  }
  host6.sin6_port = htons(port);
  st = connect(this->idSocket, (sockaddr*)&host6, sizeof(host6));
  return st;
}

/**
  * EstablishConnection method
  *   use "connect" Unix system call
  *
  * @param      char * host: host address in dns notation, example "os.ecci.ucr.ac.cr"
  * @param      char * service: process address, example "http"
 **/
int VSocket::EstablishConnection(const char* host, const char* service) {
  struct addrinfo hints, *result;
  int st = -1;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = this->IPv6 ? AF_INET6 : AF_INET;
  hints.ai_socktype = (this->type == 's') ? SOCK_STREAM : SOCK_DGRAM;
  st = getaddrinfo(host, service, &hints, &result);
  if (st != 0) {
    throw std::runtime_error(
      "VSocket::EstablishConnection - getaddrinfo failed");
  }
  st = connect(this->idSocket, result->ai_addr, result->ai_addrlen);
  freeaddrinfo(result);
  if (st == -1) {
    throw std::runtime_error("VSocket::EstablishConnection - connect failed");
  }
  return st;
}

int VSocket::Bind(int) {
  int st = -1;
  struct sockaddr_in host4;
  host4.sin_family = AF_INET;
  host4.sin_addr.s_addr = htonl(INADDR_ANY);
  host4.sin_port = this->port;
  memset(host4.sin_zero, '\0', sizeof(host4.sin_zero));
  return st;
}

size_t VSocket::SendTo(const void*, size_t, void*) {
  int st = -1;
  return;
}

size_t VSocket::ReceiveFrom(void*, size_t, void*) {
  int st = -1;
  return;
}