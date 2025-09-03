#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class Server {
 private:
  std::string storage_dir;
  std::map<std::string, std::string>file_metadata;
  int file_counter;

  std::string curr_time() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
  }

  std::string gen_filename() {
    return "ascii_" + std::to_string(++this->file_counter) + ".txt";
  }

  bool file_exists(const std::string& filename) {
    return std::filesystem::exists(this->storage_dir + "/" + filename);
  }

  std::string read_file(const std::string& filename) {
    std::ifstream file(this->storage_dir + "/" + filename);
    if (!file.is_open()) return "";

    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
  }

  bool write_file(
    const std::string& filename, const std::string& content) {
    std::ofstream file(this->storage_dir + "/" + filename);
    if (!file.is_open()) return false;

    file << content;
    this->file_metadata[filename] = this->curr_time();
    return true;
  }

  std::vector<std::string> get_filelist() {
    std::vector<std::string> files;
    try {
      for (const auto& file :
        std::filesystem::directory_iterator(this->storage_dir)) {
        if (file.is_regular_file()) {
          files.push_back(file.path().filename().string());
        }
      }
    } catch (const std::exception& e) {
      std::cerr << "Error listing files: " << e.what() << std::endl;
    }
    return files;
  }

  std::string get_fileinfo(const std::string filename) {
    if (!file_exists(filename)) return "";

    std::ostringstream info;
    info << "Filename: " << filename << "\n";

    try {
      auto filesize =
        std::filesystem::file_size(this->storage_dir + "/" + filename);
      info << "Size: " << filesize << " bytes\n";

      auto ftime =
        std::filesystem::last_write_time(this->storage_dir + "/" + filename);
      auto cftime = std::chrono::duration_cast<std::chrono::seconds>
        (ftime.time_since_epoch()).count();
      auto ltime = *std::localtime(&cftime);
      info << "Last modified: " << std::put_time(&ltime, "%Y-%m-%d %H:%M:%S")
        << "\n";

      if (this->file_metadata.find(filename) != this->file_metadata.end()) {
        info << "Created: " << this->file_metadata[filename] << "\n";
      }
    } catch (const std::exception& e) {
      info << "Error retrieving file information: "<< e.what() << "\n";
    }
    return info.str();
  }

 public:
  Server(const std::string& storage_dir = ".")
    : storage_dir(storage_dir), file_counter(0) {
    std::filesystem::create_directories(this->storage_dir);

    auto files = this->get_filelist();
    for (const auto& file : files) {
      if (file.find("ascii_") == 0) {
        try {
          int num = std::stoi(file.substr(6, file.find(".txt") - 6));
          this->file_counter = std::max(this->file_counter, num);
        } catch (const std::exception& e) {
        }
      }
    }
  }

  std::string process_command(const std::string& command) {
    std::istringstream iss(command);
    std::string method;
    iss >> method;

    if (method == "fetch") {
      std::string filename;
      iss >> filename;
      if (filename.empty()) return "err No filename";
      if (!file_exists(filename)) return "err File not found: " + filename;

      std::string content = this->read_file(filename);
      if (content.empty()) return "err Could not read file: " + filename;

      return "ok " + content;
    } else if (method == "store") {
      std::string content;
      std::getline(iss, content);
      if (content.empty()) return "err No content provided";

      std::string filename = this->gen_filename();
      if (!this->write_file(filename, content))
        return "err Could not store file";

      return "ok " + filename;
    } else if (method == "fetch-list") {
      auto files = this->get_filelist();
      std::ostringstream filelist;
      for (size_t i = 0; i < files.size(); ++i) {
        if (i > 0) filelist << ", ";
        filelist << files[i];
      }

      return "ok " + filelist.str();
    } else if (method == "fetch-info") {
      std::string filename;
      iss >> filename;
      if (filename.empty()) return "err No filename";
      if (!file_exists(filename)) return "File not found: " + filename;

      std::string info = this->get_fileinfo(filename);
      if (info.empty()) return "err Could not retrieve file information";

      return "ok " + info;
    } else if (method == "close") {
      return "ok close";
    } else {
      return "err Unknown method: " + method;
    }
  }

  void run() {
    std::cout << "=== ASCII ART SERVER STARTED ===\n" << std::endl;
    std::cout << "Storage directory: " << this->storage_dir << std::endl;
    std::cout << "Method ('exit' to quit): ";
    std::string input;
    while (std::getline(std::cin, input)) {
      if (input == "exit") break;
      if (!input.empty()) {
        std::string response = this->process_command(input);
        std::cout << response << std::endl;
      }
    }
    std::cout << "Server shutting down..." << std::endl;
  }
};

int main() {
  Server server = Server("./ascii-art");
  server.run();
  return 0;
}
