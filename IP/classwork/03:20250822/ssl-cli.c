/**
 *
 *   Non-class version of SSL client program
 *
 *   CI0123 PIRO
 *
 **/
 
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <errno.h>
#include <unistd.h>		// close



int OpenConnection( const char * hostname, const char * port ) {
   struct hostent *host;
   int sfd, err, st;

   if ( NULL == (host = gethostbyname( hostname ) ) )     {
      perror( hostname );
      exit( 10 );
   }

   struct addrinfo hints = {0}, *addrs;
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;

   st = getaddrinfo( hostname, port, &hints, &addrs );
   if ( 0 != st ) {
      fprintf(stderr, "%s: %s\n", hostname, gai_strerror( st ));
      exit( 11 );
   }

   for ( struct addrinfo *addr = addrs; addr != NULL; addr = addr->ai_next) {
      sfd = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
      if ( -1 == sfd ) {
         err = errno;
         continue;
      }

      if ( 0 == connect(sfd, addr->ai_addr, addr->ai_addrlen) ) {
         break;
      }

      err = errno;
      sfd = -1;
      close( sfd );
   }

   freeaddrinfo(addrs);

   if ( -1 == sfd ) {
      fprintf(stderr, "%s: %s\n", hostname, strerror(err));
      exit(EXIT_FAILURE);
   }

   return sfd;

}

int main( int items, char const *values[] ) {
   SSL_CTX *ctx;
   int sfd, st;
   char * host = "163.178.104.187";
   char * serv = "https";
   const char * request = "GET /lego/list.php?figure=giraffe HTTP/1.1\r\nhost: redes.ecci\r\n\r\n";
   char a[ 512 ];

   const SSL_METHOD *method = TLS_client_method(); /* Create new client-method instance */
   ctx = SSL_CTX_new( method );

   if ( NULL == ctx ) {
      perror("SSL_CTX");
      exit( 2 );
   }

   SSL *ssl = SSL_new( ctx );
   if ( NULL == ssl ) {
      perror("SSL_new");
      exit( 3 );
   }

   if ( items > 1 ) {
      host = values[ 1 ];
   }
   if ( items > 2 ) {
      serv = values[ 2 ];
   }
   sfd = OpenConnection( host, serv );
   SSL_set_fd(ssl, sfd);

   st = SSL_connect(ssl);
   if ( 1 != st ) {
      SSL_get_error( ssl, st );
      fprintf( stderr, "SSL_connect failed with SSL_get_error code %d\n", st );
      exit( 4 );
   }

   printf( "Connected with %s encryption\n", SSL_get_cipher( ssl ) );

   st = SSL_write( ssl, request, strlen( request ) );
   if ( 0 == st ) {
      st = SSL_get_error( ssl, st );
      fprintf( stderr, "SSL_write failed with SSL_get_error code %d\n", st );
      exit( 5 );
   }
   memset( a, 0, 512 );
   st = SSL_read( ssl, a, 512 );
   if ( st < 0 ) {
      st = SSL_get_error( ssl, st );
      fprintf( stderr, "SSL_read failed with SSL_get_error code %d\n", st );
      exit( 6 );
   }
   printf( "%s\n", a );
   
   SSL_free( ssl );
   close( sfd );
   SSL_CTX_free( ctx );

   return 0;

}

