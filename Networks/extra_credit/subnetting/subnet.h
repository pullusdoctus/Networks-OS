#pragma once

#include "IP.h"

#include <string>

class Subnet {
  private:
    std::string name;
    int ogSize;
    int adjSize;
    IP ip;
    int mask;

  public:
    Subnet(const std::string& name, int size);

    void setAddress(const IP& newIp, int newMask);

    std::string getName() const { return this->name; }
    int getOgSize() const { return this->ogSize; }
    int getAdjSize() const { return this->adjSize; }
    IP getAddress() const { return this->ip; }
    int getMask() const { return this->mask; }

    IP getBroadcastAddress() const;
    IP getFirstHost() const;
    IP getLastHost() const;

    void print() const;
};