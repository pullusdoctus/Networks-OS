/**
  *  Universidad de Costa Rica
  *  ECCI
  *  CI0123 Proyecto integrador de redes y sistemas operativos
  *  2025-i
  *  Grupos: 1 y 3
  *
  ****** SSLSocket base class interface
  *
  * (Fedora version)
  *
 **/

#ifndef SSLSocket_h
#define SSLSocket_h
 
#include "Socket.h"
#include <openssl/ssl.h>
#include <openssl/err.h>


class SSLSocket : public VSocket {
   public:
      // int BuildSocket( char, bool = false );
      // int BuildSocket( int );
      ~SSLSocket();

      // void Close();
      // int EstablishConnection( const char *, int );
      // int EstablishConnection( const char *, const char * );
      int MakeConnection( const char *, int );
      int MakeConnection( const char *, const char * );

      size_t Read( void *, size_t );
      size_t Write( const void *, size_t );
      size_t Write( const char * );

      // int Bind( int );			// Assign a socket address to a socket descriptor
      // int MarkPassive( int );		// Mark a socket passive: will be used to accept connections
      // int WaitForConnection( void );	// Wait for a peer connection
      VSocket * AcceptConnection();
      // int Shutdown( int );		// cause all or part of a full-duplex connection on the socket
      //                                   // associated with the file descriptor socket to be shut down

      // // UDP methods
      // size_t sendTo( const void *, size_t, void * );
      // size_t recvFrom( void *, size_t, void * );
      
      SSLSocket( const char * certFileName, const char * keyFileNamei, bool IPv6 = false,
            bool server = false);

      SSLSocket(bool IPv6 = false, bool server = false);

      SSLSocket( int id );

      void InitContext( bool serverContext );

      void Init( bool );

      void LoadCertificates( const char * certFileName, const char * keyFileName );

      void ShowCerts();

      void Copy( SSLSocket * original );

      void AcceptSSL();

      const char * GetCipher();
      void * SSLContext;
   protected:
      int idSocket;   // Socket identifier
      bool IPv6;      // Is IPv6 socket?
      int port;       // Socket associated port
      char type;      // Socket type (datagram, stream, etc.)
      
      void * SSLStruct;
};

#endif // SSLSocket_h
