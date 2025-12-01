#include "subnetter.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cmath>

Subnetter::Subnetter(const IP& base, bool low2high)
  : baseNetwork(base), low2high(low2high) {}

// =============================================================================

bool Subnetter::compareBySize(const Subnet& a, const Subnet& b) {
  return a.getAdjSize() > b.getAdjSize();
}

int Subnetter::calcMask(int size) const {
  int bits = 0;
  int temp = size;
  while (temp > 1) {
    temp /= 2;
    bits++;
  }
  return 32 - bits;
}

void Subnetter::assignAddresses() {
  uint32_t current = this->baseNetwork.toInt();
  if (this->low2high) {
    for (size_t i = 0; i < this->subnets.size(); i++) {
      int size = this->subnets[i].getAdjSize();
      int mask = this->calcMask(size);
      this->subnets[i].setAddress(IP(current), mask);
      current += size;
    }
  } else {
    uint32_t total = 0;
    for (const auto& subnet : this->subnets) {
      total += subnet.getAdjSize();
    }
    current = this->baseNetwork.toInt() + total;
    for (size_t i = 0; i < this->subnets.size(); i++) {
      int size = this->subnets[i].getAdjSize();
      int mask = this->calcMask(size);
      current -= size;
      this->subnets[i].setAddress(IP(current), mask);
    }
  }
}

// =============================================================================

void Subnetter::addSubnet(const std::string& name, int hostCount) {
  this->subnets.push_back(Subnet(name, hostCount));
}

void Subnetter::calc() {
  std::sort(this->subnets.begin(), this->subnets.end(), this->compareBySize);
  this->assignAddresses();
}

void Subnetter::print() const {
  std::cout << "\n=== SUBNETTING RESULTS ===\n" << std::endl;
  std::cout << "Base network: " << this->baseNetwork.toString() << std::endl;
  std::cout << "Assignment order: "
            << (this->low2high ? "Lower to higher IP" : "Higher to lower IP")
            << "\n" << std::endl;
  std::cout << std::left
            << std::setw(6)   << "Set"
            << std::setw(8)   << "N"
            << std::setw(18)  << "Net"
            << std::setw(6)   << "Mask"
            << std::setw(18)  << "BC"
            << std::setw(18)  << "First"
            << std::setw(18)  << "Last"
            << std::endl;
  std::cout << std::string(92, '-') << std::endl;
  for (const auto& subnet : this->subnets) subnet.print();
  std::cout << std::endl;
}