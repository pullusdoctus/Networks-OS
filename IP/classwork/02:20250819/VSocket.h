/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  ****** VSocket base class interface
  *
  * (Fedora version)
  *
 **/

#pragma once
#include <stddef.h>

class VSocket {
 public:
  ~VSocket();

  void BuildSocket(char type, bool IPv6 = false );
  void Close();

  int EstablishConnection(const char* host, int port);
  int EstablishConnection(const char* host, const char* service);
  virtual int MakeConnection(const char* host, int port) = 0;
  virtual int MakeConnection(const char* host, const char* service) = 0;
  virtual size_t Connect(const char* host, int port) = 0;

  virtual size_t Read(void* buffer, size_t bufferSize) = 0;
  virtual size_t Write(const void* buffer, size_t bufferSize) = 0;
  virtual size_t Write(const char* text) = 0;

  int Bind(int);
  size_t SendTo(const void*, size_t, void*);
  size_t ReceiveFrom(void*, size_t, void*);

 protected:
  bool IPv6;      // Is IPv6 socket?
  int idSocket;   // Socket identifier
  int port;       // Socket associated port
  char type;      // Socket type (datagram, stream, etc.)

  int EstablishIPv4(int st, const char* host, int port);
  int EstablishIPv6(int st, const char* host, int port);
};
