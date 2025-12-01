#include "diffieHellman.h"

#include <iostream>
#include <random>

DiffieHellman::DiffieHellman(long long prime, long long generator)
 : primeModulus(prime), generator(generator) {
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<long long> dist(2, this->primeModulus - 2);
  this->privateKey = dist(gen);
  
  this->publicKey
   = this->modPow(this->generator, this->privateKey, this->primeModulus);
}

long long DiffieHellman::modPow(long long base, long long exp, long long mod) {
  long long result = 1;
  base = base % mod;
  while (exp > 0) {
    if (exp % 2 == 1) result = (result * base) % mod;
    exp = exp >> 1;
    base = (base * base) % mod;
  }
  return result;
}

long long DiffieHellman::computeSharedSecret(long long otherPublicKey) {
  return this->modPow(otherPublicKey, this->privateKey, this->primeModulus);
}

void DiffieHellman::printKeys() const {
  std::cout << "Private Key: " << this->privateKey << std::endl;
  std::cout << "Public Key: " << this->publicKey << std::endl;
}