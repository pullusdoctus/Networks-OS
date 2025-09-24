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
#include <cstdio>
#include <openssl/evp.h>
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
 *  SSLSocket
 *     use SSL_new with a defined context
 *
 *  Create a SSL object for server conections
 *
 *  @param	const char * certFileName, file containing certificate
 *  @param	const char * keyFileName, file containing keys
 **/
SSLSocket::SSLSocket(const char* certFilename,
                     const char* keyFilename,
                     bool IPv6) {
  this->BuildSocket('s', IPv6);
  this->SSLContext = nullptr;
  this->SSLStruct = nullptr;
  this->InitContext(true);  // init server
  this->LoadCertificates(certFilename, keyFilename);
  SSL* ssl = SSL_new(reinterpret_cast<SSL_CTX*>(this->SSLContext));
  if (!ssl)
    throw std::runtime_error("SSLSocket::SSLSocket - SSL_new failed");
  this->SSLStruct = ssl;
}


/**
  *  Class constructor
  *  @param     int id: socket descriptor
 **/
SSLSocket::SSLSocket(int fd) {
  this->idSocket = fd;
  this->type = 's';
  this->IPv6 = false;
  this->port = -1;
  this->SSLContext = nullptr;
  this->SSLStruct = nullptr;
}

/**
 *   Create constructs a new SSL * variable from a previous created context
 *
 *  @param	SSLSocket * original socket with a previous created context
 **/
void SSLSocket::Copy(SSLSocket* og) {
  if (!og || !og->SSLContext)
    throw std::runtime_error("SSLSocket::Copy - Invalid original socket");
  SSL* ssl = SSL_new(reinterpret_cast<SSL_CTX*>(og->SSLContext));
  if (!ssl)
    throw std::runtime_error("SSLSocket::Copy - SSL_new failed");
  this->SSLStruct = ssl;
  this->SSLContext = og->SSLContext;
  if (SSL_set_fd(ssl, this->idSocket) <= 0) {  // try to adopt og.idSocket
    SSL_free(ssl);
    throw std::runtime_error("SSLSocket::Copy - SSL_set_fd failed");
  }
}


/**
  * Class destructor
 **/
SSLSocket::~SSLSocket() {
  // SSL destroy
  if (this->SSLContext)
    SSL_CTX_free(reinterpret_cast<SSL_CTX*>(this->SSLContext));
  if (this->SSLStruct)
    SSL_free(reinterpret_cast<SSL*>(this->SSLStruct));
  this->Close();
}


/**
  *  SSLInit
  *     use SSL_new with a defined context
  *
  *  Create a SSL object
 **/
void SSLSocket::Init(bool serverContext) {
  this->InitContext(serverContext);
  SSL* ssl = SSL_new(reinterpret_cast<SSL_CTX*>(this->SSLContext));
  if (!ssl)
    throw std::runtime_error("SSLSocket::Init - SSL_new failed");
  this->SSLStruct = ssl;
}

/**
  *  InitContext
  *     use SSL_library_init, OpenSSL_add_all_algorithms, SSL_load_error_strings, TLS_server_method, SSL_CTX_new
  *
  *  Creates a new SSL server context to start encrypted comunications, this context is stored in class instance
 **/
void SSLSocket::InitContext(bool serverContext) {
  const SSL_METHOD* method = nullptr;
  SSL_CTX* context = nullptr;
  if (serverContext) {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    method = TLS_server_method();
  } else {
    method = TLS_client_method();
  }
  if (!method) {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("SSLSocket::InitContext - Method creation failed");
  }
  context = SSL_CTX_new(method);
  if (!context) {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("SSLSocket::InitContext - SSL_CTX_new failed");
  }
  this->SSLContext = context;
}

/**
 *  Load certificates
 *    verify and load certificates
 *
 *  @param	const char * certFileName, file containing certificate
 *  @param	const char * keyFileName, file containing keys
 *
 **/
void SSLSocket::LoadCertificates(const char* certFileName,
                                 const char* keyFileName) {
  SSL_CTX* context = reinterpret_cast<SSL_CTX*>(this->SSLContext);
  if (  // try to load certificate file
    SSL_CTX_use_certificate_file(context, certFileName,
                                 SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("SSLSocket::LoadCertificates - "
                             "Certificate file load failed");
  }
  if (  // try to load keys
    SSL_CTX_use_PrivateKey_file(context, keyFileName,
                                SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("SSLSocket::LoadCertificates - "
                             "Private keys load failed");
  }
  if (!SSL_CTX_check_private_key(context)) {  // if key invalid
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("SSLSocket::LoadCertificates - "
                             "Private keys verification failed");
  }
}

/**
 * Connect
 *  Base class override. Calls SSLSocket::MakeConnection inside
**/
size_t SSLSocket::Connect(const char* hostName, int port) {
  return this->MakeConnection(hostName, port);
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
  int st = this->EstablishConnection(hostName, port);		// Establish a non ssl connection first
  if (st != 0) return st;
  SSL_set_fd(reinterpret_cast<SSL*>(this->SSLStruct), this->idSocket);
  int ssl_st = SSL_connect(reinterpret_cast<SSL*>(this->SSLStruct));
  if (ssl_st <= 0) return SSL_get_error(reinterpret_cast<SSL*>(this->SSLStruct),
                                        ssl_st);
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
  int st = this->EstablishConnection(host, service);
  if (st != 0) return st;
  SSL_set_fd(reinterpret_cast<SSL*>(this->SSLStruct), this->idSocket);
  int ssl_st = SSL_connect(reinterpret_cast<SSL*>(this->SSLStruct));
  if (ssl_st <= 0) return SSL_get_error(reinterpret_cast<SSL*>(this->SSLStruct),
                                        ssl_st);
  return st;
}

/**
 * AcceptConnection
 *  Base class override. Accept TCP connection with SSLSocket
 */
VSocket* SSLSocket::AcceptConnection() {
  int clientID = this->WaitForConnection();
  return new SSLSocket(clientID);
}

/**
 *   SSLSocket::Accept
 *
 *  waits for a TLS/SSL client to initiate the TLS/SSL handshake
 **/
void SSLSocket::AcceptSSL() {
  if (!this->SSLStruct)
    throw std::runtime_error("SSLSocket::AcceptSSL - SSL obj not init");
  if (SSL_set_fd(reinterpret_cast<SSL*>(this->SSLStruct),
                 this->idSocket) <= 0) {  // bind socket to SSL
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("SSLSocket::AcceptSSL - SSL_set_fd failed");
  }
  int st = SSL_accept(reinterpret_cast<SSL*>(this->SSLStruct));
  if (st <= 0) {
    int error = SSL_get_error(reinterpret_cast<SSL*>(this->SSLStruct), st);
    ERR_print_errors_fp(stderr);
    char error_msg[256];
    snprintf(error_msg, sizeof(error_msg), "SSLSocket::Accept SSL - "
             "SSL_accept failed: ERR %d", error);
    throw std::runtime_error(error_msg);
  }
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
  int st = SSL_read(reinterpret_cast<SSL*>(this->SSLStruct), buffer, size);
  if (st <= 0) {
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
  int st = SSL_write(reinterpret_cast<SSL*>(this->SSLStruct), string, strlen(string));
  if (st <= 0) {
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
  int st = SSL_write(reinterpret_cast<SSL*>(this->SSLStruct), buffer, size);
  if (st <= 0) {
    throw std::runtime_error("SSLSocket::Write(void*, size_t)");
  }
  return st;
}

/**
 *   Show SSL certificates
 **/
void SSLSocket::ShowCerts() {
  // Get certificates (if available)
  X509* cert =
    SSL_get_peer_certificate(reinterpret_cast<SSL*>(this->SSLStruct));
  char* line = nullptr;
  if (cert) {
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
