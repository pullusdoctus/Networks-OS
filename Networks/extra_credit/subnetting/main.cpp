#include "subnetter.h"

#include <iostream>

int main() {
  std::cout << "\n=== Subnetting Algorithm ===\n" << std::endl;
  std::cout << "\nExample 1: Low to high\n" << std::endl;
  IP ip("192.168.24.0");
  Subnetter subnetter(ip);
  subnetter.addSubnet("A", 16);
  subnetter.addSubnet("B", 127);
  subnetter.addSubnet("C", 30);
  subnetter.addSubnet("D", 15);
  subnetter.addSubnet("E", 63);
  subnetter.addSubnet("F", 7);
  subnetter.calc();
  subnetter.print();

  std::cout << "\n\nExample 2: High to low\n" << std::endl;
  IP ip2("172.16.64.0");
  Subnetter subnetter2(ip2, false);
  subnetter2.addSubnet("A", 16);
  subnetter2.addSubnet("B", 127);
  subnetter2.addSubnet("C", 30);
  subnetter2.addSubnet("D", 1024);
  subnetter2.addSubnet("E", 511);
  subnetter2.addSubnet("F", 128);
  subnetter2.calc();
  subnetter2.print();

  return 0;
}