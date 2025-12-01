#include "diffieHellman.h"

#include <iostream>

int main() {
  long long primeMod = 23;
  long long gen = 5;
  
  std::cout << "=== Diffie-Hellman Key Exchange Demo ===" << std::endl;
  std::cout << "\nPublic Parameters:" << std::endl;
  std::cout << "Prime (p): " << primeMod << std::endl;
  std::cout << "Generator (g): " << gen << std::endl;
  
  std::cout << "\n--- Alice ---" << std::endl;
  DiffieHellman alice(primeMod, gen);
  alice.printKeys();
  
  std::cout << "\n--- Bob ---" << std::endl;
  DiffieHellman bob(primeMod, gen);
  bob.printKeys();
  
  std::cout << "\n--- Key Exchange ---" << std::endl;
  long long aliceSharedSecret = alice.computeSharedSecret(bob.getPublicKey());
  long long bobSharedSecret = bob.computeSharedSecret(alice.getPublicKey());
  
  std::cout << "Alice's computed shared secret: " << aliceSharedSecret << std::endl;
  std::cout << "Bob's computed shared secret: " << bobSharedSecret << std::endl;
  
  if (aliceSharedSecret == bobSharedSecret) {
    std::cout << "\nSuccess! Both parties have the same shared secret." << std::endl;
  } else {
    std::cout << "\nError! Shared secrets do not match." << std::endl;
  }
  
  std::cout << "\n\n=== Example with Larger Prime ===" << std::endl;
  long long largePrimeMod = 467;
  long long largeGen = 2;
  
  std::cout << "Prime (p): " << largePrimeMod << std::endl;
  std::cout << "Generator (g): " << largeGen << std::endl;
  
  DiffieHellman alice2(largePrimeMod, largeGen);
  DiffieHellman bob2(largePrimeMod, largeGen);
  
  long long sharedSecret = alice2.computeSharedSecret(bob2.getPublicKey());
  
  std::cout << "\nAlice's public key: " << alice2.getPublicKey() << std::endl;
  std::cout << "Bob's public key: " << bob2.getPublicKey() << std::endl;
  std::cout << "Shared secret: " << sharedSecret << std::endl;
  
  return 0;
}