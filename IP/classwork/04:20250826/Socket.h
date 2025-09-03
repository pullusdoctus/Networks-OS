/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  ****** Socket class interface
  *
  * (Fedora version)
 **/

#pragma once
#include "VSocket.h"

class Socket : public VSocket {
   public:
      Socket(char type, bool IPv6 = false);
      ~Socket();
      size_t Connect(const char* host, int port) override;
      int MakeConnection(const char* host, int port) override;
      int MakeConnection(const char* host, const char* service) override;
      size_t Read(void* buffer, size_t bufferSize) override;
      size_t Write(const void* buffer, size_t bufferSize) override;
      size_t Write(const char* text) override;
      int Listen(int connections = 5);
      Socket* Accept();
};

