#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>
#include <string>
#include <sstream>

#define MAX_FILE_SIZE 10240

class Fork {
 private:
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
    std::cout << "[Fork to Server] " << method << std::endl;
    // TODO: connect them so server can return response
    std::cout << "Enter server response: ";
    std::string response;
    std::getline(std::cin, response);
    return response;
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

int main() {
  Fork fork;
  fork.run();
  return 0;
}
