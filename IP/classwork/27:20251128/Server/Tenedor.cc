/**
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2025-i
 *  Grupo 6
 *
 *  ForkServer.cc - Servidor coordinador entre múltiples servidores y clientes
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <regex>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Socket.h"

#define MAXBUF 256
#define PORT_CLIENT 8080    // Puerto para clientes
#define PORT_SERVER 8081    // Puerto para servidores
#define PORT_UDP 4321       // Puerto UDP para señalización

/**
 * Estructura para almacenar información de servidores
 */
struct ServerInfo {
  std::string ip;
  int port;
  bool active;
  std::vector<std::string> figures;
};

/**
 * Clase Tenedor - Coordinador del sistema distribuido
 */
class Tenedor {
private:
  std::map<std::string, ServerInfo> servers;  // IP -> ServerInfo
  std::map<std::string, std::string> figureLocation;  // figura -> IP servidor
  std::mutex serversMutex;
  VSocket* clientListener;
  VSocket* udpSocket;
  int currentServerIndex;

public:
  Tenedor() : currentServerIndex(0) {
    clientListener = nullptr;
    udpSocket = nullptr;
  }

  ~Tenedor() {
    if (clientListener) {
      clientListener->Close();
      delete clientListener;
    }
    if (udpSocket) {
      udpSocket->Close();
      delete udpSocket;
    }
  }

  /**
   * Parsea un mensaje con formato: COMANDO /BEGIN/ [params] /END/
   */
  bool parseMessage(const std::string& message, std::string& command, 
                    std::vector<std::string>& params, std::string& postscript) {
    std::regex cmdRegex("([A-Z]+)\\s*/BEGIN/\\s*(.*)\\s*/END/\\s*(.*)");
    std::smatch matches;
    
    if (std::regex_search(message, matches, cmdRegex)) {
      command = matches[1].str();
      std::string paramsStr = matches[2].str();
      postscript = matches[3].str();

      postscript.erase(0, postscript.find_first_not_of(" \t\r\n"));
      postscript.erase(postscript.find_last_not_of(" \t\r\n") + 1);
      
      // Separar parámetros por '/'
      size_t pos = 0;
      while (pos < paramsStr.length()) {
        size_t nextSlash = paramsStr.find('/', pos);
        if (nextSlash == std::string::npos) {
          std::string param = paramsStr.substr(pos);
          param.erase(0, param.find_first_not_of(" \t\r\n"));
          param.erase(param.find_last_not_of(" \t\r\n") + 1);
          if (!param.empty() && param != " ") {
            params.push_back(param);
          }
          break;
        }
        std::string param = paramsStr.substr(pos, nextSlash - pos);
        param.erase(0, param.find_first_not_of(" \t\r\n"));
        param.erase(param.find_last_not_of(" \t\r\n") + 1);
        if (!param.empty() && param != " ") {
          params.push_back(param);
        }
        pos = nextSlash + 1;
      }
      return true;
    }
    return false;
  }

  /**
   * Construye un mensaje de respuesta
   */
  std::string buildResponse(int code, const std::string& data) {
    std::string status;
    switch(code) {
      case 200: status = "200 OK"; break;
      case 400: status = "400 Bad Request"; break;
      case 500: status = "500 Internal Server Error"; break;
      default: status = "500 Internal Server Error";
    }
    
    std::string response = status + " /BEGIN/ " + data + " /END/\r\n";
    return response;
  }

  /**
   * Comando LIST - Retorna lista de todas las figuras
   */
  std::string handleList() {
    std::lock_guard<std::mutex> lock(serversMutex);
    std::string result;
    
    for (const auto& entry : figureLocation) {
      result += entry.first + "\n";
    }
    
    if (result.empty()) {
      result = "No hay figuras disponibles";
    }
    
    return buildResponse(200, result);
  }

  /**
   * Comando GET - Obtiene una figura específica
   */
  std::string handleGet(const std::string& figureName) {
    std::lock_guard<std::mutex> lock(serversMutex);
    
    auto it = figureLocation.find(figureName);
    if (it == figureLocation.end()) {
      return buildResponse(400, "Figura no encontrada");
    }
    
    std::string serverIP = it->second;
    auto serverIt = servers.find(serverIP);
    
    if (serverIt == servers.end() || !serverIt->second.active) {
      return buildResponse(500, "Servidor no disponible");
    }
    
    try {
      VSocket* serverConn = new Socket('s', false);
      serverConn->MakeConnection(serverIP.c_str(), PORT_SERVER);
      
      std::string request = "GET /BEGIN/ " + figureName + " /END/\r\n";
      serverConn->Write(request.c_str(), request.length());
      
      char buffer[MAXBUF];
      std::string response;
      int bytes;
      
      while ((bytes = serverConn->Read(buffer, sizeof(buffer))) > 0) {
        response.append(buffer, bytes);
        if (response.find("/END/") != std::string::npos) break;
      }
      
      serverConn->Close();
      delete serverConn;
      
      return response;
    } catch (...) {
      return buildResponse(500, "Error de comunicación con servidor");
    }
  }

  /**
   * Comando ADD - Agrega una nueva figura al sistema
   */
  std::string handleAdd(const std::string& figureName, const std::string& size, 
                        const std::string& content) {
    std::lock_guard<std::mutex> lock(serversMutex);
    if (figureLocation.find(figureName) != figureLocation.end()) {
      return buildResponse(400, "Figura ya existe");
    }
    
    std::string selectedServer;
    int count = 0;
    int activeServers = 0;
    for (const auto& server : servers) {
      if (server.second.active) {
        activeServers++;
      }
    }

    if (activeServers == 0)
      return buildResponse(500, "No hay servidores disponibles");

    for (const auto& server : servers) {
      if (server.second.active) {
        if (count == currentServerIndex % activeServers) {
          selectedServer = server.first;
          break;
        }
        count++;
      }
    }
    
    currentServerIndex++;
    
    try {
      VSocket* serverConn = new Socket('s', false);
      serverConn->MakeConnection(selectedServer.c_str(), PORT_SERVER);
      
      std::string request = "ADD /BEGIN/ " + figureName + " / " + 
                          size  + " /END/" + content + "\r\n";
      serverConn->Write(request.c_str(), request.length());
      
      char buffer[MAXBUF];
      std::string response;
      int bytes;
      
      while ((bytes = serverConn->Read(buffer, sizeof(buffer))) > 0) {
        response.append(buffer, bytes);
        if (response.find("/END/") != std::string::npos) break;
      }
      
      serverConn->Close();
      delete serverConn;
      
      figureLocation[figureName] = selectedServer;
      servers[selectedServer].figures.push_back(figureName);
      
      return buildResponse(200, "Figura agregada exitosamente");
    } catch (...) {
      return buildResponse(500, "Error al agregar figura");
    }
  }

  /**
   * Comando DELETE - Elimina una figura del sistema
   */
  std::string handleDelete(const std::string& figureName) {
    std::lock_guard<std::mutex> lock(serversMutex);
    auto it = figureLocation.find(figureName);
    if (it == figureLocation.end()) {
      return buildResponse(400, "Figura no encontrada");
    }
    
    std::string serverIP = it->second;
    
    try {
      VSocket* serverConn = new Socket('s', false);
      serverConn->MakeConnection(serverIP.c_str(), PORT_SERVER);
      
      std::string request = "DELETE /BEGIN/ " + figureName + " /END/\r\n";
      serverConn->Write(request.c_str(), request.length());
      
      char buffer[MAXBUF];
      std::string response;
      int bytes;
      
      while ((bytes = serverConn->Read(buffer, sizeof(buffer))) > 0) {
        response.append(buffer, bytes);
        if (response.find("/END/") != std::string::npos) break;
      }
      
      serverConn->Close();
      delete serverConn;
      
      figureLocation.erase(figureName);
      auto& figures = servers[serverIP].figures;
      figures.erase(std::remove(figures.begin(), figures.end(), figureName), 
                    figures.end());
      
      return buildResponse(200, "Figura eliminada exitosamente");
    } catch (...) {
      return buildResponse(500, "Error al eliminar figura");
    }
  }

  /**
   * Procesa solicitudes de clientes
   */
  void processClientRequest(VSocket* client) {
    char buffer[MAXBUF];
    std::string request;
    int bytes;
    
    try {
      while ((bytes = client->Read(buffer, sizeof(buffer))) > 0) {
        request.append(buffer, bytes);
        if (request.find("/END/") != std::string::npos) break;
        else std::cerr << "/END/ no encontrado." << std::endl; break;
      }
      
      std::cout << "Solicitud recibida: " << request << std::endl;
      
      std::string command;
      std::vector<std::string> params;
      std::string postscript;
      if (!parseMessage(request, command, params, postscript)) {
        std::string response = buildResponse(400, "Formato de comando incorrecto");
        client->Write(response.c_str(), response.length());
        client->Close();
        delete client;
        return;
      }
      
      std::string response;
      if (command == "LIST") {
        response = handleList();
      } else if (command == "GET" && params.size() >= 1) {
        response = handleGet(params[0]);
      } else if (command == "ADD" && params.size() >= 2) {
        response = handleAdd(params[0], params[1], postscript);
      } else if (command == "DELETE" && params.size() >= 1) {
        response = handleDelete(params[0]);
      } else {
        response = buildResponse(400, "Comando no reconocido o parámetros faltantes");
      }
      
      client->Write(response.c_str(), response.length());
    } catch (const std::exception& e) {
      std::cerr << "Error procesando solicitud: " << e.what() << std::endl;
    }
    client->Close();
    delete client;
  }

  /**
   * Maneja señalización UDP de servidores
   */
  void handleUDPSignaling() {
    try {
      udpSocket = new Socket('d', false);
      udpSocket->Bind(PORT_UDP);
      
      char buffer[MAXBUF];
      struct sockaddr_in clientAddr;
      
      std::cout << "Escuchando señalización UDP en puerto " << PORT_UDP << std::endl;
      
      while (true) {
        memset(buffer, 0, sizeof(buffer));
        memset(&clientAddr, 0, sizeof(clientAddr));
        
        size_t bytes = udpSocket->recvFrom(buffer, sizeof(buffer) - 1, &clientAddr);
        
        if (bytes > 0) {
          buffer[bytes] = '\0';
          std::string message(buffer);
          
          char senderIP[INET_ADDRSTRLEN];
          inet_ntop(AF_INET, &(clientAddr.sin_addr), senderIP, INET_ADDRSTRLEN);
          
          std::cout << "UDP recibido de " << senderIP << ": " << message << std::endl;
          
          std::string command;
          std::vector<std::string> params;
          std::string postscript;
          
          if (parseMessage(message, command, params, postscript) && params.size() > 0) {
            std::string ip = params[0];
            
            std::lock_guard<std::mutex> lock(serversMutex);
            
            if (command == "CONNECT") {
              std::cout << "Servidor conectado: " << ip << std::endl;
              servers[ip] = {ip, PORT_SERVER, true, {}};
            } else if (command == "QUIT") {
              std::cout << "Servidor desconectado: " << ip << std::endl;
              if (servers.find(ip) != servers.end()) {
                servers[ip].active = false;
                
                for (const auto& fig : servers[ip].figures) {
                  figureLocation.erase(fig);
                }
              }
            }
          }
        }
      }
    } catch (const std::exception& e) {
      std::cerr << "Error en señalización UDP: " << e.what() << std::endl;
    }
  }

  /**
   * Inicia el servidor tenedor
   */
  void start() {
    std::thread udpThread(&Tenedor::handleUDPSignaling, this);
    udpThread.detach();
    
    clientListener = new Socket('s', false);
    clientListener->Bind(PORT_CLIENT);
    clientListener->MarkPassive(10);
    
    std::cout << "========================================" << std::endl;
    std::cout << "                TENEDOR                 " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Puerto clientes (TCP): " << PORT_CLIENT << std::endl;
    std::cout << "Puerto servidores (TCP): " << PORT_SERVER << std::endl;
    std::cout << "Puerto señalización (UDP): " << PORT_UDP << std::endl;
    std::cout << "Esperando conexiones..." << std::endl;
    std::cout << "========================================" << std::endl;
    
    while (true) {
      try {
        VSocket* client = clientListener->AcceptConnection();
        std::thread* worker = new std::thread(&Tenedor::processClientRequest, 
                                              this, client);
        worker->detach();
      } catch (const std::exception& e) {
        std::cerr << "Error aceptando conexión: " << e.what() << std::endl;
      }
    }
  }
};

int main(int argc, char** argv) {
  Tenedor tenedor;
  tenedor.start();
  return 0;
}