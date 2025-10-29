/**
 *   UCR-ECCI
 *
 *   IPv4 TCP client normal or SSL according to parameters
 **/

#include <iostream>
#include <iterator>
#include <fstream>

#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "VSocket.h"
#include "Socket.h"
#include "SSLSocket.h"

#define	MAXBUF	256

/**
 * @brief Reads all the contents in a binary file.
 *
 * @param nArchivo The file's name.
 *
 * @return A pointer to a dynamic buffer that has the file's contents. Nullptr if the file could not be opened.
 */
char* leerFigura(const std::string& nArchivo) {
    std::fstream figura(nArchivo, std::ios::in | std::ios::binary);
    if (!figura.is_open()) {
        std::cerr << "No se pudo abrir la figura" << std::endl;
        return nullptr;
    }

    // Calcular el tamaño del archivo especificado por el usuario
    figura.seekg(0, std::ios::end);
    std::streamsize tArchivo = figura.tellg();
    figura.seekg(0, std::ios::beg);

    // Crear buffer dinámico para el contenido del archivo
    char* buffer = new char[tArchivo];
    figura.read(buffer, tArchivo);
    
    figura.close();
    return buffer;
}

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
bool processRequest(char* clientRequest, VSocket* client) {
  // construir el request HTTP
  std::string requestStr(clientRequest);
  std::string request = 
      "POST / HTTP/1.1\r\n"
      "Host: 127.0.0.1\r\n"
      "Content-Type: application/xml\r\n"
      "Content-Length: " + std::to_string(requestStr.size()) + "\r\n"
      "\r\n" +
      requestStr;

  client->Write(request.c_str(), request.size());

  std::string response;
  char buf[MAXBUF];
  int bytes;

  // Ciclo para leer la respuesta completa del server
  while ((bytes = client->Read(buf, sizeof(buf) - 1)) > 0) {
      buf[bytes] = '\0';
      response += buf;
  }

  size_t start = response.find("<pre>");
  size_t end   = response.find("</pre>");
  std::string art;
  if (start != std::string::npos && end != std::string::npos) {
      art = response.substr(start + 5, end - (start + 5));
  } else {
      art = response;
  }

  // Se muestra el resultado de la consulta al server
  printf("Bytes read total: %zu\n%s\n", response.size(), art.c_str());

  return true;
}


/**
 * @brief Creates a connection to the remote server
 * It creates an HTTP or HTTPS socket depending on arguments, and establishes
 * a connection to the remote server.
 *
 * @param argc Argument count
 *  - If argc > 1: HTTPS connection
 *  - If argc == 1: HTTP connection
 * @param os Remote server IP
 *
 * @return VSocket* Pointer to the created socket
**/
VSocket* connectToServer(int argc, char* hostname, int port) {
  VSocket* client;
	#ifdef USE_IPV6
		// IPv6
		client = new Socket('s', true);
	#else
		// IPv4
		client = new Socket('s', false);
	#endif
	client->MakeConnection( hostname, port );
  return client;
}

/**
 * @brief Main program execution - ASCII Art Client
 * Implements a client application that connects to a remote server
 * to retrieve ASCII art. Supports both HTTP and HTTPS (port 80 and port 443).
 *
 * Summary:
 * 1. Determines connection type based on argc
 * 2. Establishes connection to remote server
 * 3. Presents menu of available ASCII art
 * 4. Sends HTTP GET request for selected figure
 * 5. Receives and processes the response
 * 6. Removes HTML tags and formats output
 * 7. Displays ASCII art to user
 *
 * @param argc Argument count
 *  - If argc > 1: Uses HTTPS connection
 *  - If argc == 1: Uses HTTP connection
 * @param argv Argument values
 *
 * @return int Exit status
 *  - 0: running
 *  - Non-zero: error
**/
int main( int argc, char * argv[] ) {
	char * hostname = argv[1];  // server IP
  int port = atoi(argv[2]);
	int selectedOption;
	bool running = true;
	VSocket* client = nullptr;
	// Parameters to ask and receive from server
	char action[16] = { 0 };
	char figure[16] = { 0 };
	const char * requestMessage = "\n<Body>\n\
			\t<Action>%s</Action>\n\
			\t<Figure>%s</Figure>\n\
      \t<Content>%s</Content>\n\
			</Body>\n";
	char clientRequest[ 1024 ] = { 0 };
	// main loop
	while (running) {
		// menu
		printf("Welcome to Animal Figures, please select an option:"
				"\n0. Quit\n1. List Available Figures\n2. Request a Figure\n3. Submit a Figure\n4. Delete a Figure\n");
		std::cin >> selectedOption;
		// get the correct ASCII art based on input
		switch (selectedOption)
		{
      case 0:  // exit
        printf("Closing the client. Goodbye!\n");
        running = false;
        break;
      case 1:  // list figures
        client = connectToServer(argc, hostname, port);
        strcpy(action, "List");
        strcpy(figure, "Figures");
        sprintf( clientRequest, requestMessage, action, figure );
        running = processRequest(clientRequest, client);
        client->Close();
        client = nullptr;
        break;
      case 2:  // request figure
        client = connectToServer(argc, hostname, port);
        strcpy(action, "Request");
        printf("Type the name of the animal that you want:\n");
        std::cin >> figure;
        sprintf( clientRequest, requestMessage, action, figure );
        running = processRequest(clientRequest, client);
        client->Close();
        client = nullptr;
        break;
      case 3: { // submit figure
        client = connectToServer(argc, hostname, port);
        strcpy(action, "Submit");
        printf("Type the filename of the animal that you want to upload:\n");
        std::cin >> figure;
        char* content = leerFigura(figure);
        if (content != nullptr) {
          sprintf( clientRequest, requestMessage, action, figure, content);
          running = processRequest(clientRequest, client);
        }
        client->Close();
        client = nullptr;
        break;
      }
      case 4:  // delete figure
        client = connectToServer(argc, hostname, port);
        strcpy(action, "Delete");
        printf("Type the name of the animal that you want to delete:\n");
        std::cin >> figure;
        sprintf( clientRequest, requestMessage, action, figure );
        running = processRequest(clientRequest, client);
        client->Close();
        client = nullptr;
        break;
      default:
        printf( "Error, please enter a valid option\n" );
        break;
		}
	}

	if (client) {
		client->Close();
	}

	return 0;
}
