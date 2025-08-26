/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  *  ******   Socket class implementation
  *
  * (Fedora version)
 **/

#include <sys/socket.h>         // sockaddr_in
#include <arpa/inet.h>          // ntohs
#include <unistd.h>		// write, read
#include <cstring>
#include <stdexcept>
#include <stdio.h>		// printf

#include "Socket.h"		// Derived class

/**
  *  Class constructor
  *     use Unix socket system call
  *
  *  @param     char t: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param     bool ipv6: if we need a IPv6 socket
 **/
Socket::Socket(char type, bool IPv6) {
  this->BuildSocket(type, IPv6);  // Call base class constructor
}

/**
  *  Class destructor
  *
  *  @param     int id: socket descriptor
 **/
Socket::~Socket() { this->Close(); }

size_t Socket::Connect(const char* host, int port) {
  return this->MakeConnection(host, port);
}

/**
  * MakeConnection method
  *   use "EstablishConnection" in base class
  *
  * @param      char * host: host address in dot notation, example "10.1.166.62"
  * @param      int port: process address, example 80
 **/
int Socket::MakeConnection(const char* host, int port) {
  return this->EstablishConnection(host, port);
}

/**
  * MakeConnection method
  *   use "EstablishConnection" in base class
  *
  * @param      char * host: host address in dns notation, example "os.ecci.ucr.ac.cr"
  * @param      char * service: process address, example "http"
 **/
int Socket::MakeConnection(const char* host, const char* service) {
   return this->EstablishConnection(host, service);
}

/**
  * Read method
  *   use "read" Unix system call (man 3 read)
  *
  * @param      void * buffer: buffer to store data read from socket
  * @param      int size: buffer capacity, read will stop if buffer is full
 **/
size_t Socket::Read(void* buffer, size_t bufferSize) {
  int st = read(this->idSocket, buffer, bufferSize);
  if  (st == -1) {
    throw std::runtime_error("Socket::Read - could not read from socket");
  }
  return st;
}

/**
  * Write method
  *   use "write" Unix system call (man 3 write)
  *
  * @param      void * buffer: buffer to store data write to socket
  * @param      size_t size: buffer capacity, number of bytes to write
 **/
size_t Socket::Write(const void* buffer, size_t bufferSize) {
  int st = write(this->idSocket, buffer, bufferSize);
  if (st == -1) {
    throw std::runtime_error("Socket::Write - could not write to buffer");
  }
  return st;
}

/**
  * Write method
  *   use "write" Unix system call (man 3 write)
  *
  * @param      char * text: text to write to socket
 **/
size_t Socket::Write(const char* text) {
  int st = write(this->idSocket, text, strlen(text));
  if (st == -1) {
    throw std::runtime_error("Socket::Write - could not write text to buffer");
  }
  return st;
}
