/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  ****** SSLSocket base class implementation
  *
  * (Fedora version)
  *
 **/
#include <iostream>
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
#include "SSLSocket.h"


/**
 *  SSLSocket
 *     use SSL_new with a defined context
 *
 *  Create a SSL object for server conections
 *
 *  @param	const char * certFileName, file containing certificate
 *  @param	const char * keyFileName, file containing keys
 *
 **/
SSLSocket::SSLSocket( const char * certFileName, const char * keyFileNamei, bool IPv6,
      bool server ) {
   // Create a new VSocket stream socket instance
   this->idSocket = this->BuildSocket('s', IPv6);

   this->SSLContext = nullptr;
   this->SSLStruct = nullptr;

   // Create a new server context calling InitContext
   this->Init(server);
   if (server == true)
      this->LoadCertificates(certFileName, keyFileNamei);
}

/**
  *  Class constructor
  *     use base class
  *
  *  @param     char t: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param     bool ipv6: if we need a IPv6 socket
  *
 **/
SSLSocket::SSLSocket( bool IPv6, bool server) {
   this->idSocket = this->BuildSocket( 's', IPv6 );
   
   this->SSLContext = nullptr;
   this->SSLStruct = nullptr;

   this->Init(server);					// Initializes to client context

}

SSLSocket::SSLSocket(int existingSocket) {
    this->idSocket = this->BuildSocket( existingSocket );
    this->SSLContext = nullptr;
    this->SSLStruct = nullptr;
}


/**
  * Class destructor
  *
 **/
SSLSocket::~SSLSocket() {

// SSL destroy
   if ( nullptr != this->SSLContext ) {
      SSL_CTX_free( reinterpret_cast<SSL_CTX *>( this->SSLContext ) );
   }
   if ( nullptr != this->SSLStruct ) {
      SSL_free( reinterpret_cast<SSL *>( this->SSLStruct ) );
   }

   this->Close();

}

/**
  * MakeConnection method
  *   use "EstablishConnection" in base class
  *
  * @param      char * host: host address in dot notation, example "10.1.166.62"
  * @param      int port: process address, example 80
  *
 **/
int SSLSocket::MakeConnection( const char * hostip, int port ) {
   int st = 0;
   this->EstablishConnection( hostip, port );
   if (SSL_set_fd((SSL*) this->SSLStruct, this->idSocket) != 1) {
      throw std::runtime_error( "SSLSocket::Invalid file descriptor assigment." );
   }
   st = SSL_connect((SSL*) this->SSLStruct);
   if (st != 1) {
      int err = SSL_get_error((SSL*) this->SSLStruct, st);
      std::cerr << "SSL_connect failed, code=" << err << std::endl;
      ERR_print_errors_fp(stderr); // imprime la pila de errores de OpenSSL
      throw std::runtime_error("SSLSocket::MakeConnection");
   }

   return st;
}


/**
  * MakeConnection method
  *   use "EstablishConnection" in base class
  *
  * @param      char * host: host address in dns notation, example "os.ecci.ucr.ac.cr"
  * @param      char * service: process address, example "http"
  *
 **/
int SSLSocket::MakeConnection( const char *host, const char *service ) {
   int st = 0;
   this->EstablishConnection( host, service );
   if (SSL_set_fd((SSL*) this->SSLStruct, this->idSocket) != 1) {
      throw std::runtime_error( "SSLSocket::Invalid file descriptor assigment." );
   }
   st = SSL_connect((SSL*) this->SSLStruct);
   if ( 1 != st ) {
      throw std::runtime_error( "SSLSocket::MakeConnection" );
   }
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
  *
 **/
size_t SSLSocket::Read( void * buffer, size_t size ) {
   int st = 0;

   st = SSL_read((SSL*) this->SSLStruct, buffer, size);

   if (st <= 0) {
      int err = SSL_get_error((SSL*)this->SSLStruct, st);
      ERR_print_errors_fp(stderr);
      throw std::runtime_error("SSLSocket::Read failed");
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
  *
 **/
size_t SSLSocket::Write( const char * string ) {
   int st = 0;
   
   st = SSL_write((SSL*) this->SSLStruct, string, (size_t) strlen(string));

   if (st <= 0) {
      int err = SSL_get_error((SSL*)this->SSLStruct, st);
      ERR_print_errors_fp(stderr);
      throw std::runtime_error("SSLSocket::Write failed");
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
  *
 **/
size_t SSLSocket::Write( const void * buffer, size_t size ) {
   int st = 0;
   
   st = SSL_write((SSL*) this->SSLStruct, buffer, size);

   if (st <= 0) {
      int err = SSL_get_error((SSL*)this->SSLStruct, st);
      ERR_print_errors_fp(stderr);
      throw std::runtime_error("SSLSocket::Write failed");
   }

   return st;

}

/**
  * AcceptiConnection method
  *    use base class to accept connections
  *
  *  @returns   a new class instance
  *
  *  Waits for a new connection to service (TCP mode: stream)
  *
 **/
VSocket * SSLSocket::AcceptConnection() {
    int id = this->WaitForConnection();
    printf("This is the id: %i\n", id);

    // Crear peer pasando el socket aceptado
    SSLSocket * peer = new SSLSocket(id);

    // asignar el contexto del servidor al peer
    peer->SSLContext = this->SSLContext;

    // crear la estructura SSL para este socket
    SSL * ssl = SSL_new((SSL_CTX*)peer->SSLContext);
    if (!ssl) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    peer->SSLStruct = ssl;

    // vincular descriptor
    if (SSL_set_fd((SSL*)peer->SSLStruct, peer->idSocket) != 1) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    return peer;
}


/**
  *  InitContext
  *     For clients:
  *        use TLS_client_method and SSL_CTX_new
  *
  *     For servers:
  *     use SSL_library_init, OpenSSL_add_all_algorithms, SSL_load_error_strings, TLS_server_method, SSL_CTX_new
  *
  *  Creates a new SSL server context to start encrypted comunications, this context is stored in class instance
  *     Create a context for client/server according to parameter
  *
  *  Creates a new SSL context to start encrypted comunications, this context is stored in class instance
  *
 **/
void SSLSocket::InitContext( bool serverContext ) {
   if ( serverContext ) {	// Create a server context
      SSL_METHOD const * method;
      SSL_CTX *ctx;
      // Call SSL_library_init() to register the available SSL/TLS ciphers and digests
      SSL_library_init();
      // Call OpenSSL_add_all_algorithms() to load and register all cryptos, etc.
      OpenSSL_add_all_algorithms();
      // Call SSL_load_error_strings() to load all error messages
      SSL_load_error_strings();
      // Call TLS_server_method() to create a method
      method = TLS_server_method();
      ctx = SSL_CTX_new(method);
      // Check for errors
      if ( ctx == NULL )
      {
         ERR_print_errors_fp(stderr);
         abort();
      }
      this->SSLContext = ctx;
   } else {	// Create a client context
      // Create a SSL_METHOD * variable using TLS_client_method() call
      SSL_METHOD const * method;
      method = TLS_client_method();
      // Check for errors
      if ( method == NULL ) {
         ERR_print_errors_fp(stderr);
         abort();
      }
      // Create a new context SSL_CTX * variable using SSL_CTX_new( ... )
      SSL_CTX *ctx;
      ctx = SSL_CTX_new(method);
      // Check for errors
      if ( ctx == NULL )
      {
         ERR_print_errors_fp(stderr);
         abort();
      }
      // Assign context to an instance variable
      this->SSLContext = ctx;
   }
}

void SSLSocket::Init( bool serverContext ) {
   this->InitContext( serverContext );
   SSL * ssl = SSL_new( (SSL_CTX *) this->SSLContext );
   if ( nullptr == ssl ) {
      throw std::runtime_error( "SSLSocket::Init, error at creating SSL structure" );
   }
   this->SSLStruct = (void *) ssl;
}

/**
 *  Load certificates
 *    verify and load certificates
 *
 *  @param	const char * certFileName, file containing certificate
 *  @param	const char * keyFileName, file containing keys
 *
 **/
void SSLSocket::LoadCertificates( const char * certFileName, const char * keyFileName ) {
   SSL_CTX * context = (SSL_CTX*) this->SSLContext;

   if ( SSL_CTX_use_certificate_file( context, certFileName, SSL_FILETYPE_PEM ) <= 0 ) {	 // set the local certificate from CertFile
      ERR_print_errors_fp( stderr );
      abort();
   }

   if ( SSL_CTX_use_PrivateKey_file( context, keyFileName, SSL_FILETYPE_PEM ) <= 0 ) {	// set the private key from KeyFile (may be the same as CertFile)
      ERR_print_errors_fp( stderr );
      abort();
   }

   if ( ! SSL_CTX_check_private_key( context ) ) {	// verify private key
      fprintf( stderr, "Private key does not match the public certificate\n" );
      abort();
   }

}

/**
 *   Show SSL certificates
 *
 **/
void SSLSocket::ShowCerts() {
   X509 *cert;
   char *line;

   cert = SSL_get_peer_certificate( (SSL *) this->SSLStruct );		 // Get certificates (if available)
   if ( nullptr != cert ) {
      printf("Server certificates:\n");
      line = X509_NAME_oneline( X509_get_subject_name( cert ), 0, 0 );
      printf( "Subject: %s\n", line );
      free( line );
      line = X509_NAME_oneline( X509_get_issuer_name( cert ), 0, 0 );
      printf( "Issuer: %s\n", line );
      free( line );
      X509_free( cert );
   } else {
      printf( "No certificates.\n" );
   }

}

/**
 *   Create constructs a new SSL * variable from a previous created context
 *
 *  @param	Socket * original socket with a previous created context
 *
 **/
void SSLSocket::Copy( SSLSocket * original ) {
   SSL * ssl;
   int st;

   // Constructs a new SSL * variable using SSL_new() function
   ssl = SSL_new((SSL_CTX *) this->SSLContext);
   // Check for errors
   if ( ssl == NULL )
   {
      ERR_print_errors_fp(stderr);
      abort();
   }
   // Assign new variable to instance variable
   original->SSLStruct = ssl;

   // change conection status to SSL using SSL_set_fd() function
   st = SSL_set_fd((SSL*) original->SSLStruct, original->idSocket);
   // Check for errors 
   if (st != 1) {
      ERR_print_errors_fp(stderr);
      abort();
   }
}



/**
 *   SSLSocket::Accept
 *
 *  waits for a TLS/SSL client to initiate the TLS/SSL handshake
 *
 **/
void SSLSocket::AcceptSSL(){
   int st = -1;

   // Call SSL_accept() to initiate TLS/SSL handshake
   st = SSL_accept((SSL*) this->SSLStruct);
   // Check for errors
   if (st != 1) {
      ERR_print_errors_fp(stderr);
      abort();
   }
}

/**
 *   Get SSL ciphers
 *
 **/
const char * SSLSocket::GetCipher() {

   // Call SSL_get_cipher() and return the name
   return SSL_get_cipher((SSL*) this->SSLStruct);
}
