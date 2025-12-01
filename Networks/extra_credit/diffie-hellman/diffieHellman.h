#pragma once

class DiffieHellman {
private:
  long long primeModulus;
  long long generator;
  long long privateKey;
  long long publicKey;

  long long modPow(long long base, long long exp, long long mod);
    
public:
  DiffieHellman(long long prime, long long generator);
    
  long long getPublicKey() const { return publicKey; }
  long long getPrivateKey() const { return privateKey; }
    
  long long computeSharedSecret(long long otherPublicKey);
    
  void printKeys() const;
};