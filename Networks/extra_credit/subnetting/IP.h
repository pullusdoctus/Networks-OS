#pragma once

#include <string>
#include <cstdint>

class IP {
  private:
    uint32_t address;

  public:
    IP();
    IP(const std::string& ip);
    IP(uint32_t address);

    uint32_t toInt() const;
    std::string toString() const;

    IP operator+(uint32_t offset) const;
    IP operator-(uint32_t offset) const;
};