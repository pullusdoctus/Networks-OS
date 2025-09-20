#include <iostream>

#include "FileSystem.h"

int main() {
  FileSystem* fs= new FileSystem() ;

  fs->crearInodo("a.dat") ;
  fs->agregar("a.dat", 'a') ;
  fs->imprimir() ;

  fs->crearInodo("b.dat") ;
  fs->agregar("b.dat",'b') ;
  fs->imprimir() ;

  fs->crearInodo("c.dat") ;
  fs->agregar("c.dat",'c') ;
  fs->imprimir() ;

  fs->escribir("a.dat", "hola!");
  char* datos = fs->leer("a.dat", 5);
  std::cout << "Datos leidos: " << datos << std::endl;
  fs->imprimir();
  fs->renombrar("a.dat", "ballena.dat");
  fs->imprimir();

}