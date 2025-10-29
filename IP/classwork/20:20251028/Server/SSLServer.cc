/**
*  Universidad de Costa Rica
*  ECCI
*  CI0123 Proyecto integrador de redes y sistemas operativos
*  2025-i
*  Grupos: 1 y 3
*
****** SSLSocket example, server code
*
* (Fedora version)
*
**/

#include <cstdio>	// printf
#include <cstdlib>	// atoi
#include <cstring>	// strlen, strcmp
#include <iostream>
#include <regex>
#include <thread>

#include "SSLSocket.h"
#include "FileSystem.h"
#define	MAXBUF	256

#define PORT	6752

/**
 * @brief Processes an HTTP request and displays the formatted response
 *
 * Sends a HTTP request to the remote server through the client socket,
 * and formats the resonse received by removing its HTML tags.
 * It then displays the cleaned ASCII art to the user.
 *
 * @param figure String with the full HTTP request
 * @param client Client socket through which the connection to the server is established
 *
 * @return bool True on succesful request connection, false otherwise
**/
void processResponse(VSocket* client, FileSystem * fs) {
	std::string request;
  char buf[MAXBUF];   // ahora 256
  int bytes;

  while ((bytes = client->Read(buf, sizeof(buf))) > 0) {
      request.append(buf, bytes);
      if (request.find("\r\n\r\n") != std::string::npos) break;
  }

  size_t pos = request.find("\r\n\r\n"); // Fin de los headers HTTP
  std::string body;
  if (pos != std::string::npos) {
      body = request.substr(pos + 4); // Contenido que viene después de los headers
  } else {
      body = request; // Si no tiene headers, se utiliza todo el contenido
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
  if (action == "List") {
      fsData = fs->leerUnidad();
  } else if (action == "Request") {
      char* temp = fs->leer(figure, 256);
      fsData = temp;
      delete[] temp;
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
void Service( VSocket * client, FileSystem * fs ) {
	// client->AcceptConnection();
	// client->ShowCerts();
	processResponse(client, fs);
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
	FileSystem* fs= new FileSystem() ;
  fs->crearInodo("gato.dat") ;

  std::streamsize tArchivo;
  char* buffer = leerFigura(&tArchivo);
  fs->escribir("gato.dat", buffer);

	VSocket * server, * client;
	std::thread * worker;
	int port = PORT;
	
	if ( cuantos > 1 ) {
		port = atoi( argumentos[ 1 ] );
	}

	#ifdef USE_IPV6
		// IPv6
		server = new Socket(/*"ci0123.pem", "key0123.pem",*/ 's', true);
	#else
		// IPv4
		server = new Socket(/*"ci0123.pem", "key0123.pem", false, */ 's');
	#endif

	server->Bind( port );
	server->MarkPassive( 10 );

	for( ; ; ) {
		client = (Socket * ) server->AcceptConnection();
		worker = new std::thread( Service, client, fs );	// service connection
	}

}
