#include <algorithm>
#include <cctype>
#include <exception>
#include <iostream>
#include <regex>
#include <string>
#include <sstream>

#include "Socket.h"

#define MAX_FILE_SIZE 10240

class Fork {
 private:
  Socket* forkSocket;
  std::string serverHost;
  int serverPort;

  bool is_valid_filename(const std::string& filename) {
    if (filename.empty() || filename.length() > 255) return false;
    std::regex valid_chars(R"([a-zA-Z0-9._-]+)");
    return std::regex_match(filename, valid_chars);
  }

  bool is_ascii_content(const std::string& content) {
    return std::all_of(content.begin(), content.end(),
                       [](unsigned char c) { return c <= 127; });
  }

  bool is_valid_filesize(const std::string& content) {
    return content.length() <= MAX_FILE_SIZE;
  }

  std::string comm_w_server(const std::string& method) {
    try {
      std::cout << "[Fork to Server] " << method << std::endl;
      this->forkSocket->Write(method.c_str());
      this->forkSocket->Write("\n");

      char buffer[4096];
      size_t bytesRead = forkSocket->Read(buffer, sizeof(buffer) - 1);
      buffer[bytesRead] = '\0';

      std::string response(buffer);
      if (!response.empty() && response.back() == '\n')
        response.pop_back();
      return response;
    } catch (const std::exception& e) {
      return "err Connection error: " + std::string(e.what());
    }
  }

  void comm_w_client(const std::string& message) {
    std::cout << "[Fork to Client] " << message << std::endl;
  }

  std::string extract_server_content(const std::string& server_response) {
    if (server_response.length() > 3 && server_response.substr(0, 3) == "ok ") {
      return server_response.substr(3);
    } else if (
      server_response.length() > 4 && server_response.substr(0, 4) == "err ") {
      return "Error: " + server_response.substr(4);
    } else {
      return "Error: Invalid server response";
    }
  }

 public:
  Fork(const std::string& host, int port)
    : serverHost(host), serverPort(port), forkSocket(nullptr) {
  }

  ~Fork() { if (this->forkSocket) delete this->forkSocket; }

  bool connect_to_server() {
    try {
      this->forkSocket = new Socket('s', false);
      this->forkSocket->Connect(this->serverHost.c_str(), this->serverPort);
      std::cout << "Connected to server at " << this->serverHost << ":"
        << this->serverPort << std::endl;
      return true;
    } catch (const std::exception& e) {
      std::cerr << "Failed to connect to server: " << e.what() << std::endl;
      return false;
    }
  }

  std::string process_client_command(const std::string& command) {
    std::istringstream iss(command);
    std::string method;
    iss >> method;
    if (method == "request") {
      std::string filename;
      iss >> filename;
      if (!this->is_valid_filename(filename))
        return "Error: Invalid filename format";

      std::string server_command = "fetch " + filename;
      std::string server_response = this->comm_w_server(server_command);

      return this->extract_server_content(server_response);
    } else if (method == "submit") {
      std::string content;
      std::getline(iss, content);
      if (content.empty()) return "Error: No content provided";
      if (!this->is_ascii_content(content))
        return "Error: File must containt ASCII characters only";
      if (!this->is_valid_filesize(content))
        return "Error: Filesize exceeds maximum (" +
        std::to_string(MAX_FILE_SIZE) + " bytes)";

      std::string server_command = "store " + content;
      std::string server_response = this->comm_w_server(server_command);
      return this->extract_server_content(server_response);
    } else if (method == "list") {
      std::string server_response = this->comm_w_server("fetch-list");
      return this->extract_server_content(server_response);
    } else if (method == "info") {
      std::string filename;
      iss >> filename;
      if (!this->is_valid_filename(filename)) return "Error: Invalid filename";

      std::string server_command = "fetch-info " + filename;
      std::string server_response = this->comm_w_server(server_command);
      return this->extract_server_content(server_response);
    } else if (method == "quit") {
      this->comm_w_server("close client-socket");
      return "Connection closed. Goodbye!";
    } else {
      return "Error: Unknown method: " + method;
    }
  }

  void run() {
    if (!this->connect_to_server()) {
      std::cerr << "Cannot start fork without server connection" << std::endl;
      return;
    }
    std::cout << "=== ASCII ART FORK STARTED ===\n" << std::endl;
    std::cout << "Method: ";

    std::string input;
    while (std::getline(std::cin, input)) {
      if (!input.empty()) {
        std::string response = this->process_client_command(input);
        this->comm_w_client(response);
        if (input.substr(0, 4) == "quit") break;
      }
    }
    std::cout << "Fork shutting down..." << std::endl;
  }
};

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "USAGE: ./fork server_host server_port" << std::endl;
    return 1;
  }
  Fork fork(argv[1], std::stoi(argv[2]));
  fork.run();
  return 0;
}
