/**
*  Universidad de Costa Rica
*  ECCI
*  CI0123 Proyecto integrador de redes y sistemas operativos
*  2025-i
*  Grupo 6
*
* StorageServer: almacena los archivos
**/

#include <cstdio>	// printf
#include <cstdlib>	// atoi
#include <cstring>	// strlen, strcmp
#include <iostream>
#include <regex>
#include <thread>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <atomic>

#include "SSLSocket.h"
#include "FileSystem.h"
#define	MAXBUF	256

#define PORTCLIENT 8080 // Cliente
#define PORTFORK 8081
#define PORTCACHE 8082
#define PORTUDP 4321

#define MAX_FILESIZE 132096  // 4 * 256 (bloques directos) + 256 * 256 * 2 (bloques indirectos)

std::atomic<bool> stopRequested(false);
const char* g_ForkIP;
int g_UDPPort;
const char* g_myIP;

/**
 * @brief Processes an HTTP request and displays the formatted response
 *
 * Sends a HTTP request to the remote server through the client socket,
 * and formats the response received by removing its HTML tags.
 * It then displays the cleaned ASCII art to the user.
 *
 * @param clientRequest  String containing the full HTTP request body
 * @param client         Socket object used for communication
 * @return bool          True if the request was successfully processed
**/
std::string processRequest(char* clientRequest, VSocket* client, FileSystem * fs) {
  // convierte el request recibido a string
  std::string requestStr(clientRequest);
  // construir el request HTTP
  std::string request = 
      "POST / HTTP/1.1\r\n"
      "Host: 127.0.0.1\r\n"
      "Content-Type: application/xml\r\n"
      "Content-Length: " + std::to_string(requestStr.size()) + "\r\n"
      "\r\n" +
      requestStr;

  // envío del request al servidor a través del socket
  client->Write(request.c_str(), request.size());

  std::string response;
  char buf[MAXBUF];
  int bytes;

  // Ciclo para leer la respuesta completa del server
  while ((bytes = client->Read(buf, sizeof(buf))) > 0) {
    response.append(buf, bytes);
    if (response.find("\r\n\r\n") != std::string::npos) break;
  }

  size_t contentLengthPos = response.find("Content-Length:");
  int contentLength = 0;
  if (contentLengthPos != std::string::npos) {
    size_t start = contentLengthPos + 15;
    size_t end = response.find("\r\n", start);
    std::string lengthStr = response.substr(start, end - start);
    contentLength = std::stoi(lengthStr);
    std::cout << "Content-Length: " << contentLength << std::endl;
  }
  size_t headerEnd = response.find("\r\n\r\n");
  size_t bodyStart = headerEnd + 4;
  int bodyReceived = response.size() - bodyStart;
  std::cout << "Body received: " << bodyReceived << " / " << contentLength << std::endl;
  int remaining = contentLength - bodyReceived;
  while (remaining > 0) {
    int toRead = std::min(remaining, (int)sizeof(buf));
    bytes = client->Read(buf, toRead);
    if (bytes <= 0) break;
    response.append(buf, bytes);
    remaining -= bytes;
    std::cout << "Read " << bytes << ". Remaining: " << remaining << std::endl;
  }

  std::cout << "Respuesta del servidor: " << response.c_str() << std::endl;

  size_t pos = response.find("\r\n\r\n"); // Fin de los headers HTTP
  std::string body;
  if (pos != std::string::npos) {
      body = response.substr(pos + 4); // Contenido que viene después de los headers
  } else {
      body = response; // Si no tiene headers, se utiliza todo el contenido
  }

  std::regex r("</?[a-zA-Z][a-zA-Z0-9]*[ \".=/]*>");
  std::string result = std::regex_replace(body, r, " ");

	std::string action, figure, contentLine, content;

	std::istringstream stream(result);
  stream >> action >> figure;

  std::cout << "Action: '" << action << "' Figure: '" << figure << "'" << std::endl;

  std::string lineCleaner;
  std::getline(stream, lineCleaner); // Descartamos lo que sobre de la línea

  while (std::getline(stream, contentLine)) {
      content += contentLine + "\n";
  }

  std::string fsData;
  if (action == "MISS") {
    char* temp = fs->leer(figure, 256);
    fsData = temp;
    delete[] temp;
  } else {
    return response;
  }

  std::string serverResponse;
  if (action == "MISS") {
      std::string bodyContent = "<html><body><pre>\n" + fsData + "\n</pre></body></html>\n";
      serverResponse =
          "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/html\r\n"
          "Content-Length: " + std::to_string(bodyContent.size()) + "\r\n"
          "Connection: close\r\n"
          "\r\n" +
          bodyContent;
  } else {
      serverResponse =
          "HTTP/1.1 404 Not Found\r\n"
          "Content-Type: text/html\r\n"
          "Connection: close\r\n"
          "\r\n"
          "<html><body><pre>Not Found</pre></body></html>";
  }

  std::cout << serverResponse << std::endl;

  client->Write(serverResponse.c_str(), serverResponse.size());

  return "MISS";
}


/**
 * @brief Creates a connection to the remote server
 * It creates an HTTP or HTTPS socket depending on arguments, and establishes
 * a connection to the remote server.
 *
 * @param hostname Pointer to a character array containing the hostname or IP address
 *                 of the remote server.
 * @param port Integer value representing the port number to connect to.
 *
 * @return SSLSocket* Pointer to the created and connected SSL socket instance.
 *
 * @pre Network connectivity and server availability are required.
 * @post A secure connection to the specified server is established.
**/
VSocket* connectToServer(char* hostname, int port) {
  VSocket* client;
	#ifdef USE_IPV6
		// IPv6
		client = new SSLSocket(/*cert*/false, /*isServer*/false);
	#else
		// IPv4
		client = new SSLSocket(/*cert*/false, /*isServer*/false);
	#endif
	client->MakeConnection( hostname, port );
  return client;
}

std::string processCacheResponse(VSocket* client, FileSystem * fs, std::string request) {
  // cuerpo
  size_t pos = request.find("\r\n\r\n");
  std::string body;
  if (pos != std::string::npos) {
    body = request.substr(pos + 4);
  } else {
    body = request;
  }
  std::cout << "Body size: " << body.size() << std::endl;

  std::regex r("</?[a-zA-Z][a-zA-Z0-9]*[ \".=/]*>");
  std::string result = std::regex_replace(body, r, " ");

	std::string action, figure, contentLine, content;

	std::istringstream stream(result);
  stream >> action >> figure;

  std::cout << "Action: '" << action.c_str() << "' Figure: '" << figure.c_str() << "'" << std::endl;

  std::string lineCleaner;
  std::getline(stream, lineCleaner); // Descartamos lo que sobre de la línea

  while (std::getline(stream, contentLine)) {
      content += contentLine + "\n";
  }

  // quitar espacios y saltos de línea al inicio y al final del contenido
  if (!content.empty() && content.back() == '\n') {
    content.pop_back();
  }
  if (content.size() >= 8) {  // el regex introduce 8 espacios al inicio
    content = content.substr(8);
  }
  while (!content.empty() && std::isspace((unsigned char)content.back())) {
    content.pop_back();
  }

  std::cout << "Contenido procesado: " << content.c_str() << std::endl;

  return content.c_str();
}

/**
 * @brief Processes an HTTP request and generates the corresponding response.
 *
 * This function receives an HTTP request from a connected client through a socket,
 * extracts the action and parameters contained in the message, and performs
 * the corresponding operation on the file system. The supported operations
 * include requesting, submitting, deleting and interacting
 * with a cache server when necessary.
 *
 * @param client Pointer to the VSocket object representing the connected client.
 * @param fs Pointer to the FileSystem object that handles file operations.
 * @param IPCache Pointer to a character array containing the cache server IP address.
 * @param portCache Integer value representing the cache server port number.
 *
 * @return void
 *
 * @pre The client socket must be connected and readable.
 * @pre The FileSystem instance must be properly initialized.
 *
 * @post The client request is processed, and an HTTP-formatted response is sent back.
 */
void processResponse(VSocket* client, FileSystem * fs, char* IPCache, int portCache) {
	std::string request;
  char buf[MAXBUF];   // ahora 256
  int bytes;

  // header
  while ((bytes = client->Read(buf, sizeof(buf))) > 0) {
      request.append(buf, bytes);
      if (request.find("\r\n\r\n") != std::string::npos) break;
  }

  // cuerpo
  size_t contentLengthPos = request.find("Content-Length:");
  int contentLength = 0;
  if (contentLengthPos != std::string::npos) {
    size_t start = contentLengthPos + 15;
    size_t end = request.find("\r\n", start);
    std::string lengthStr = request.substr(start, end - start);
    contentLength = std::stoi(lengthStr);
    std::cout << "Content-Length: " << contentLength << std::endl;
  }
  size_t headerEnd = request.find("\r\n\r\n");
  size_t bodyStart = headerEnd + 4;
  int bodyReceived = request.size() - bodyStart;
  std::cout << "Body received: " << bodyReceived << " / " << contentLength << std::endl;
  int remaining = contentLength - bodyReceived;
  while (remaining > 0) {
    int toRead = std::min(remaining, (int)sizeof(buf));
    bytes = client->Read(buf, toRead);
    if (bytes <= 0) break;
    request.append(buf, bytes);
    remaining -= bytes;
    std::cout << "Read " << bytes << ". Remaining: " << remaining << std::endl;
  }
  size_t pos = request.find("\r\n\r\n");
  std::string body;
  if (pos != std::string::npos) {
    body = request.substr(pos + 4);
  } else {
    body = request;
  }
  std::cout << "Body size: " << body.size() << std::endl;

  std::regex r("</?[a-zA-Z][a-zA-Z0-9]*[ \".=/]*>");
  std::string result = std::regex_replace(body, r, " ");

	std::string action, figure, contentLine, content;

	std::istringstream stream(result);
  stream >> action >> figure;

  std::cout << "Action: '" << action << "' Figure: '" << figure << "'" << std::endl;

  std::string lineCleaner;
  std::getline(stream, lineCleaner); // Descartamos lo que sobre de la línea

  while (std::getline(stream, contentLine)) {
      content += contentLine + "\n";
  }

  // quitar espacio en blanco al principio del contenido
  if (!content.empty() && content.back() == '\n') {
    content.pop_back();
  }
  if (content.size() >= 8) {  // el regex introduce 8 espacios al inicio
    content = content.substr(8);
  }

  std::string fsData;
  if (action == "List") {
    fsData = fs->leerUnidad();
  } else if (action == "Request") {
    // Caché
    char * hostname = IPCache;  // server caché IP
    int cachePort = portCache;
    VSocket* clientServer = nullptr;
    const char * requestMessage = "\n<Body>\n\
        \t<Action>%s</Action>\n\
        \t<Figure>%s</Figure>\n\
        \t<Content>%s</Content>\n\
        </Body>\n";
    char clientRequest[ 1024 ] = { 0 };
    std::cout << "DEBUG: connecting to cache" << std::endl;
    clientServer = connectToServer(hostname, cachePort); // Conexión con el caché
    std::cout << "DEBUG: connected to cache" << std::endl;
    sprintf( clientRequest, requestMessage, "RequestCache", figure.c_str(), "");
    std::string processedRequestResult = processRequest(clientRequest, clientServer, fs);
    std::cout << "Respuesta del servidor: " << processedRequestResult.c_str() << std::endl;
    if (processedRequestResult != "MISS") { // Hubo hit
      std::cout << "Cache hit" << std::endl;
      fsData = processCacheResponse(clientServer, fs, processedRequestResult);
    } else { // Hubo miss
      // Cliente
      char* temp = fs->leer(figure, MAX_FILESIZE);
      if (temp) {
        fsData = temp;
        delete[] temp;
      } else {
        fsData = "Error: File not found";
      }
    }
    
    clientServer->Close();
    delete clientServer;
    clientServer = nullptr;
    // processResponse(clientServer, fs, IPCache, portCache);
  } else if (action == "Submit") {
    fs->crearInodo(figure);
    fs->escribir(figure, content.c_str());
    fsData = fs->leerUnidad();
  } else if (action == "Delete") {
    fs->eliminar(figure);
    fsData = fs->leerUnidad();
  }

  std::string response;
  if (action == "List" || action == "Request" || action == "Submit" || action == "Delete") {
      std::string bodyContent = "<html><body><pre>\n" + fsData + "\n</pre></body></html>\n";
      response =
          "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/html\r\n"
          "Content-Length: " + std::to_string(bodyContent.size()) + "\r\n"
          "Connection: close\r\n"
          "\r\n" +
          bodyContent;
  } else {
      response =
          "HTTP/1.1 404 Not Found\r\n"
          "Content-Type: text/html\r\n"
          "Connection: close\r\n"
          "\r\n"
          "<html><body><pre>Not Found</pre></body></html>";
  }

  client->Write(response.c_str(), response.size());
}

/**
 * @brief Función de servicio para manejar una conexión de cliente
 *
 * Procesa la solicitud del cliente y cierra la conexión al finalizar.
 *
 * @param client Socket del cliente conectado
 * @param fs Puntero al sistema de archivos
 */
void Service( VSocket * client, FileSystem * fs, char* IPCache, int portCache) {
	// client->AcceptConnection();
	// client->ShowCerts();
	processResponse(client, fs, IPCache, portCache);
	client->Close();
}

/**
 * @brief Lee el contenido completo de un archivo de texto
 *
 * Abre el archivo "cat.txt", determina su tamaño, y lee todo su contenido
 * en un buffer dinámico.
 *
 * @param[out] tArchivo Puntero donde se almacenará el tamaño del archivo leído
 *
 * @return Puntero a un buffer dinámico con el contenido del archivo.
 *         Retorna nullptr si el archivo no pudo abrirse.
 */
char* leerFigura(std::streamsize* tArchivo) {
  std::fstream figura("./cat.txt", std::ios::in | std::ios::binary);
  if (!figura.is_open()) {
    std::cerr << "No se pudo abrir la figura" << std::endl;
    return nullptr;
  }

  // Recorremos la figura completa para obtener su tamaño
  figura.seekg(0, std::ios::end);
  *tArchivo = figura.tellg();
  figura.seekg(0, std::ios::beg);

  // Creamos un buffer dinámico para el contenido de la figura
  char* buffer = new char[*tArchivo];
  figura.read(buffer, *tArchivo);
  std::cout << std::endl;
  figura.close();
  return buffer;
}

/**
 * Notifica al Tenedor vía UDP que el servidor está iniciando
 */
void notifyFork(const char* tenedorIP, int tenedorPort, const char* myIP, 
                   bool connecting) {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    std::cerr << "Error creando socket UDP" << std::endl;
    return;
  }
  
  struct sockaddr_in tenedorAddr;
  memset(&tenedorAddr, 0, sizeof(tenedorAddr));
  tenedorAddr.sin_family = AF_INET;
  tenedorAddr.sin_port = htons(tenedorPort);
  inet_pton(AF_INET, tenedorIP, &tenedorAddr.sin_addr);
  
  std::string command = connecting ? "CONNECT" : "QUIT";
  std::string message = command + " /BEGIN/ " + std::string(myIP) + " /END/\r\n";
  
  sendto(sockfd, message.c_str(), message.length(), 0,
        (struct sockaddr*)&tenedorAddr, sizeof(tenedorAddr));
  close(sockfd);
  
  std::cout << "Notificación " << command << " enviada al Tenedor" << std::endl;
}

/**
 * Procesa comandos del protocolo intragrupal
 * Agrega esto a tu función processResponse existente
 */
void processIntraGroupCommand(VSocket* client, FileSystem* fs, 
                              const std::string& command,
                              const std::vector<std::string>& params) {
  std::string response;
  
  if (command == "GET" && params.size() >= 1) {
    // Obtener figura
    std::string figureName = params[0];
    char* temp = fs->leer(figureName, MAX_FILESIZE);
    
    if (temp) {
      std::string content(temp);
      delete[] temp;
      response = "200 OK /BEGIN/ " + content + " /END/\r\n";
    } else {
      response = "400 Bad Request /BEGIN/ Figura no encontrada /END/\r\n";
    }
  } else if (command == "ADD" && params.size() >= 3) {
    // Agregar figura
    std::string figureName = params[0];
    std::string content = params[2];
    
    fs->crearInodo(figureName);
    fs->escribir(figureName, content.c_str());
    
    response = "200 OK /BEGIN/ Figura agregada /END/\r\n";
  } else if (command == "DELETE" && params.size() >= 1) {
    // Eliminar figura
    std::string figureName = params[0];
    fs->eliminar(figureName);
    
    response = "200 OK /BEGIN/ Figura eliminada /END/\r\n";
  } else if (command == "LIST") {
    // Listar figuras
    std::string list = fs->leerUnidad();
    response = "200 OK /BEGIN/ " + list + " /END/\r\n";
  } else {
    response = "400 Bad Request /BEGIN/ Comando no reconocido /END/\r\n";
  }
  
  client->Write(response.c_str(), response.length());
}

void handleSigint(int sig) {
  (void)sig;
  stopRequested = true;
}

void setupSigHandler() {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handleSigint;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, nullptr);
}

/**
 * @brief Función principal del servidor
 *
 * Inicializa el sistema de archivos, carga un archivo de ejemplo,
 * crea un socket servidor, y entra en un ciclo infinito aceptando
 * conexiones de clientes. Cada cliente es atendido en un thread separado.
 *
 * @param cuantos Número de argumentos de línea de comandos
 * @param argumentos Array de argumentos. Si se proporciona, argumentos[1]
 *                   especifica el puerto a usar.
 *
 * @return int Código de retorno (nunca retorna en operación normal)
 */
int main( int cuantos, char ** argumentos ) {
  setupSigHandler();
  if (cuantos < 7) {
    std::cerr << "Uso: " << argumentos[0] 
              << " <puerto_cliente> <ip_cache> <puerto_cache> "
              << " <ip_tenedor> <puerto_tenedor_udp> <mi_ip>" << std::endl;
    return 1;
  }

	FileSystem* fs= new FileSystem() ;
  fs->crearInodo("gato.dat") ;

  std::streamsize tArchivo;
  char* buffer = leerFigura(&tArchivo);
  fs->escribir("gato.dat", buffer);

  // Fork server
  const char* tenedorIP = argumentos[4];
  g_myIP = tenedorIP;
  int tenedorUDPPort = atoi(argumentos[5]);
  g_UDPPort = tenedorUDPPort;
  const char* myIP = argumentos[6];
  g_myIP = myIP;
  notifyFork(tenedorIP, tenedorUDPPort, myIP, true);

  // Cliente
	VSocket * server, * client;
	std::thread * worker;
	int port = PORTCLIENT;
	if ( cuantos > 1 ) port = atoi( argumentos[ 1 ] );

	#ifdef USE_IPV6
		// IPv6
		server = new Socket(/*"ci0123.pem", "key0123.pem",*/ 's', true);
	#else
		// IPv4
		server = new Socket(/*"ci0123.pem", "key0123.pem", false, */ 's');
	#endif

	server->Bind( port );
	server->MarkPassive( 10 );
  std::cout << "Servidor iniciado en puerto " << port << std::endl;

  // Loop principal - ahora escucha en dos puertos:
  // 1. Puerto para clientes (8080)
  // 2. Puerto para Tenedor (8081)
	for( ; ; ) {
    if (stopRequested) break;
		client = (Socket * ) server->AcceptConnection();
    if (!client) {
      if (errno == EINTR) {
        std::cout << "SIGINT. Notifying fork..." << std::endl;
        notifyFork(g_ForkIP, g_UDPPort, g_myIP, false);
        break;
      }
      continue;
    }
		worker = new std::thread( Service, client, fs, argumentos[2], atoi(argumentos[3]) );	// service connection
    worker->detach();
	}
  
	return 0;
}