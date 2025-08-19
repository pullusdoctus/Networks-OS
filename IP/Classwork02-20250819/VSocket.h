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

#ifndef VSocket_h
#define VSocket_h
 
class VSocket {
   public:
       void BuildSocket( char, bool = false );
      ~VSocket();

      void Close();
      int EstablishConnection( const char *, int );
      int EstablishConnection( const char *, const char * );
      virtual int MakeConnection( const char *, int ) = 0;
      virtual int MakeConnection( const char *, const char * ) = 0;

      virtual size_t Read( void *, size_t ) = 0;
      virtual size_t Write( const void *, size_t ) = 0;
      virtual size_t Write( const char * ) = 0;

      int Bind( int );

// UDP methods
      size_t sendTo( const void *, size_t, void * );
      size_t recvFrom( void *, size_t, void * );

   protected:
      int idSocket;   // Socket identifier
      bool IPv6;      // Is IPv6 socket?
      int port;       // Socket associated port
      char type;      // Socket type (datagram, stream, etc.)
        
};

#endif // VSocket_h
