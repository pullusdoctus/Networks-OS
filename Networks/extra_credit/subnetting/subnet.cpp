#include "subnet.h"

#include <iostream>
#include <iomanip>
#include <cmath>

Subnet::Subnet(const std::string& name, int size)
  : name(name), ogSize(size), mask(0) {
  int needed = size + 2;
  this->adjSize = 1;
  while (this->adjSize < needed) this->adjSize *= 2;
}

// =============================================================================

void Subnet::setAddress(const IP& newIp, int newMask) {
  this->ip = newIp;
  this->mask = newMask;
}

// =============================================================================

IP Subnet::getBroadcastAddress() const {
  uint32_t hostBits = 32 - mask;
  uint32_t offset = (1 << hostBits) - 1;
  return this->ip + offset;
}

IP Subnet::getFirstHost() const { return this->ip + 1; }

IP Subnet::getLastHost() const { return this->getBroadcastAddress() - 1; }

// =============================================================================

void Subnet::print() const {
  std::cout << std::left
            << std::setw(6)   << this->name
            << std::setw(8)   << this->adjSize
            << std::setw(16)  << this->ip.toString()
            << std::setw(6)   << this->mask
            << std::setw(18)  << this->getBroadcastAddress().toString()
            << std::setw(18)  << this->getFirstHost().toString()
            << std::setw(18)  << this->getLastHost().toString()
            << std::endl;
}