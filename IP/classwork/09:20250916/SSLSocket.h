/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  *   SSL Socket class interface
  *
  * (Fedora version)
 **/

#pragma once

#include "VSocket.h"

class SSLSocket : public VSocket {
  public:
    SSLSocket(bool IPv6 = false);				// Not possible to create with UDP, client constructor
    SSLSocket(const char* certFilename,
              const char* keyFilename,
              bool IPv6 = false);		// For server connections
    SSLSocket(int fd);
    void Copy(SSLSocket* og);
    ~SSLSocket();

    size_t Connect(const char* hostName, int port) override;
    int MakeConnection(const char* hostName, int port) override;
    int MakeConnection(const char* host, const char* service) override;
    VSocket* AcceptConnection() override;
    void AcceptSSL();

    size_t Write(const char*) override;
    size_t Write(const void*, size_t) override;
    size_t Read(void*, size_t) override;

    void ShowCerts();
    const char* GetCipher();

  private:
    void Init(bool = false);		// Defaults to create a client context, true if server context needed
    void InitContext(bool serverContext);
    void LoadCertificates(const char* certFilename, const char* keyFilename);

    // Instance variables
    void* SSLContext;				// SSL context
    void* SSLStruct;					// SSL BIO (Basic Input/Output)
};
