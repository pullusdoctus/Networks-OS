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

void VSocket::setIDSocket(int newId) {
  int st = -1;
  if (this->idSocket != -1) {
    st = close(this->idSocket);
    if (st == -1) throw std::runtime_error("VSocket::Close");
    this->idSocket = -1;
  }
  this->idSocket = newId;
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

/**
  *  Uses "bind" Unix system call (man 2 bind) (server mode)
  *  @param      int port: bind a unnamed socket to a port defined in sockaddr structure
  *  Links the calling process to a service at port
 **/
int VSocket::Bind(int port) {
  struct sockaddr_in host4;
  host4.sin_family = AF_INET;
  host4.sin_addr.s_addr = htonl(INADDR_ANY);
  host4.sin_port = htons(port);
  memset(host4.sin_zero, '\0', sizeof(host4.sin_zero));
  int st = bind(this->idSocket, (struct sockaddr*)&host4, sizeof(host4));
  if (st == -1) {
    throw std::runtime_error("VSocket::Bind - bind failed");
  }
  this->port = port;
  return st;
}

/**
  *  @param	const void * buffer: data to send
  *  @param	size_t size data size to send
  *  @param	void * addr address to send data
  *  Send data to another network point (addr) without connection (Datagram)
 **/
size_t VSocket::SendTo(const void* buffer, size_t bufferSize, void* destiny) {
  int st = -1;
  if (this->type == 'd') {
    if (this->IPv6) {
      st = sendto(this->idSocket, buffer, bufferSize, 0,
                  (sockaddr*)destiny, sizeof(struct sockaddr_in6));
    } else {
      st = sendto(this->idSocket, buffer, bufferSize, 0,
                  (sockaddr*)destiny, sizeof(struct sockaddr_in));
    }
  } else {
    st = sendto(this->idSocket, buffer, bufferSize, 0, NULL, 0);
  }
  if (st == -1) {
    throw std::runtime_error("VSocket::SendTo - send failed");
  }
  return st;
}

/**
  *  @param	const void * buffer: data to send
  *  @param	size_t size data size to send
  *  @param	void * addr address to receive from data
  *  @return	size_t bytes received
  *  Receive data from another network point (addr) without connection (Datagram)
 **/
size_t VSocket::ReceiveFrom(void* buffer, size_t bufferSize, void* origin) {
  socklen_t originLen = 0;
  if (this->IPv6) {
    originLen = sizeof(struct sockaddr_in6);
  } else {
    originLen = sizeof(struct sockaddr_in);
  }
  int st = recvfrom(this->idSocket, buffer, bufferSize, 0,
                    (sockaddr*)origin, &originLen);
  if (st == -1) {
    throw std::runtime_error("VSocket::ReceiveFrom - receive failed");
  }
  return st;
}

