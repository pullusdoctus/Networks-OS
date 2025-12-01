#pragma once

#include "subnet.h"

#include <vector>

class Subnetter {
  private:
    IP baseNetwork;
    std::vector<Subnet> subnets;
    bool low2high;

    static bool compareBySize(const Subnet& a, const Subnet& b);
    int calcMask(int size) const;
    void assignAddresses();

  public:
    Subnetter(const IP& base, bool low2high = true);

    void addSubnet(const std::string& name, int hostCount);
    void calc();
    void print() const;
};