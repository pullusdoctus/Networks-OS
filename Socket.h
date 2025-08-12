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
  *
 **/

#ifndef Socket_h
#define Socket_h
#include "VSocket.h"

class Socket : public VSocket {

   public:
      Socket( char, bool = false );
      ~Socket();
      int MakeConnection( const char *, int );
      int MakeConnection( const char *, const char * );
      size_t Read( void *, size_t );
      size_t Write( const void *, size_t );
      size_t Write( const char * );

   protected:

};

#endif

