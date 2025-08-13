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

class VSocket {
 public:
  VSocket(char type, bool IPv6);
  ~VSocket();

  void BuildSocket(char type, bool IPv6 = false );
  void Close();
  int EstablishConnection(const char* host, int port);
  int EstablishConnection(const char* host, const char* service);
  virtual int MakeConnection(const char*, int) = 0;
  virtual int MakeConnection(const char*, const char*) = 0;

  virtual size_t Connect(const char*, int) = 0;
  virtual size_t Read(void*, size_t) = 0;
  virtual size_t Write(const void*, size_t) = 0;
  virtual size_t Write(const char*) = 0;

 protected:
  bool IPv6;      // Is IPv6 socket?
  int idSocket;   // Socket identifier
  int port;       // Socket associated port
  char type;      // Socket type (datagram, stream, etc.)
};
