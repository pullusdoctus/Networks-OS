/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  *  Socket class implementation
  *
  * (Fedora version)
 **/
 
// SSL includes
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <stdexcept>

#include "SSLSocket.h"
#include "Socket.h"

/**
  *  Class constructor
  *     use base class
  *
  *  @param     char t: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param     bool ipv6: if we need a IPv6 socket
 **/
SSLSocket::SSLSocket(bool IPv6) {
  this->BuildSocket('s', IPv6);
  this->SSLContext = nullptr;
  this->SSLStruct = nullptr;
  this->Init();					// Initializes to client context
}


/**
  *  Class constructor
  *     use base class
  *  @param     char t: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param     bool IPv6: if we need a IPv6 socket
 **/
SSLSocket::SSLSocket(char* certFileName, char* keyFileName, bool IPv6) {
}


/**
  *  Class constructor
  *  @param     int id: socket descriptor
 **/
SSLSocket::SSLSocket(int id) {
  this->BuildSocket(id);
}


/**
  * Class destructor
 **/
SSLSocket::~SSLSocket() {
  // SSL destroy
  if (nullptr != this->SSLContext) {
    SSL_CTX_free(reinterpret_cast<SSL_CTX*>(this->SSLContext));
  }
  if ( nullptr != this->SSLStruct ) {
    SSL_free(reinterpret_cast<SSL*>(this->SSLStruct));
  }
  this->Close();
}


/**
  *  SSLInit
  *     use SSL_new with a defined context
  *
  *  Create a SSL object
 **/
void SSLSocket::Init(bool serverContext) {
  SSL* ssl = nullptr;
  this->InitContext(serverContext);
}

/**
  *  InitContext
  *     use SSL_library_init, OpenSSL_add_all_algorithms, SSL_load_error_strings, TLS_server_method, SSL_CTX_new
  *
  *  Creates a new SSL server context to start encrypted comunications, this context is stored in class instance
 **/
void SSLSocket::InitContext(bool serverContext) {
  const SSL_METHOD* method;
  SSL_CTX* context;
  if (serverContext) {
  } else {
  }
  if (nullptr == method) {
    throw std::runtime_error("SSLSocket::InitContext(bool)");
  }
}

/**
 *  Load certificates
 *    verify and load certificates
 *
 *  @param	const char * certFileName, file containing certificate
 *  @param	const char * keyFileName, file containing keys
 *
 **/
 void SSLSocket::LoadCertificates(const char* certFileName, const char* keyFileName) {
}

/**
 *  Connect
 *     use SSL_connect to establish a secure conection
 *
 *  Create a SSL connection
 *
 *  @param	char * hostName, host name
 *  @param	int port, service number
 *
 **/
int SSLSocket::MakeConnection(const char* hostName, int port) {
  int st;
  st = this->MakeConnection(hostName, port);		// Establish a non ssl connection first
  return st;
}


/**
 *  Connect
 *     use SSL_connect to establish a secure conection
 *
 *  Create a SSL connection
 *
 *  @param	char * hostName, host name
 *  @param	char * service, service name
 **/
int SSLSocket::MakeConnection(const char* host, const char* service) {
  int st;
  st = this->MakeConnection(host, service);
  return st;
}


/**
  *  Read
  *     use SSL_read to read data from an encrypted channel
  *
  *  @param	void * buffer to store data read
  *  @param	size_t size, buffer's capacity
  *
  *  @return	size_t byte quantity read
  *
  *  Reads data from secure channel
 **/
size_t SSLSocket::Read(void* buffer, size_t size) {
  int st = -1;
  if (-1 == st) {
    throw std::runtime_error("SSLSocket::Read(void*, size_t)");
  }
  return st;
}


/**
  *  Write
  *     use SSL_write to write data to an encrypted channel
  *
  *  @param	void * buffer to store data read
  *  @param	size_t size, buffer's capacity
  *
  *  @return	size_t byte quantity written
  *
  *  Writes data to a secure channel
 **/
size_t SSLSocket::Write(const char* string) {
  int st = -1;
  if ( -1 == st ) {
    throw std::runtime_error("SSLSocket::Write(const char*)");
  }
  return st;
}


/**
  *  Write
  *     use SSL_write to write data to an encrypted channel
  *
  *  @param	void * buffer to store data read
  *  @param	size_t size, buffer's capacity
  *
  *  @return	size_t byte quantity written
  *
  *  Reads data from secure channel
 **/
size_t SSLSocket::Write(const void* buffer, size_t size) {
  int st = -1;
  if ( -1 == st ) {
    throw std::runtime_error("SSLSocket::Write(void*, size_t)");
  }
  return st;
}

/**
 *   Show SSL certificates
 **/
void SSLSocket::ShowCerts() {
  X509 *cert;
  char *line;
  cert = SSL_get_peer_certificate((SSL*)this->SSLStruct);		 // Get certificates (if available)
  if (nullptr != cert) {
    printf("Server certificates:\n");
    line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    printf("Subject: %s\n", line);
    free(line);
    line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
    printf("Issuer: %s\n", line);
    free(line);
    X509_free(cert);
  } else {
    printf("No certificates.\n");
  }
}

/**
 *   Return the name of the currently used cipher
 **/
const char * SSLSocket::GetCipher() {
  return SSL_get_cipher(reinterpret_cast<SSL*>(this->SSLStruct));
}
