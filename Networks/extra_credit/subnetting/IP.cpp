#include "IP.h"

#include <sstream>
#include <stdexcept>

IP::IP() : address(0) {}

IP::IP(const std::string& ip) {
  std::stringstream ss(ip);
  std::string octet;
  int octets[4] = {0, 0, 0, 0};
  int i = 0;
  while (std::getline(ss, octet, '.') && i < 4) octets[i++] = std::stoi(octet);
  if (i != 4) throw std::invalid_argument("Invalid IP address format.");
  this->address = (octets[0] << 24) | (octets[1] << 16) |
                  (octets[2] << 8)  | (octets[3]);
}

IP::IP(uint32_t address) : address(address) {}

// =============================================================================

uint32_t IP::toInt() const { return this->address; }

std::string IP::toString() const {
  std::stringstream ss;
  ss << ((this->address >> 24) & 0xFF) << "."
     << ((this->address >> 16) & 0xFF) << "."
     << ((this->address >> 8) & 0xFF)  << "."
     << (this->address & 0xFF);
  return ss.str();
}

// =============================================================================

IP IP::operator+(uint32_t offset) const { return IP(this->address + offset); }
IP IP::operator-(uint32_t offset) const { return IP(this->address - offset); }