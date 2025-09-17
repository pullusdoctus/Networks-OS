#include <iostream>
#include <string>
#include <fstream>

using namespace std;

const int TDIRECTORIO = 3;
const int TUNIDAD = 16;
const int TBLOQUE = 4;

struct INodo {
  string nombre;
  string fecha;
  int bloque0;
  int bloque1;
  int bloque2;
  int bloque3;
  INodo *bloqueIndirecto0;
  INodo *bloqueIndirecto1;

  bool operator==(const string& name) const {
    return nombre == name;
  }
};

//journaling?
class FS {


private:
INodo *directorio ;
bool* bloquesLibres;
fstream unidad;
public:

FS(){
  unidad.open("unidad.bin");
  directorio = new INodo[TDIRECTORIO] ;
  bloquesLibres= new bool[TUNIDAD];
  int nFiles = 0;

  directorio= obtenerBloque(0) ; //DEFINIR DE DONDE SALE ESTE DATO
  //

  for (int i=0 ; i<TUNIDAD; i++) {
    bloquesLibres[i] = -2; // -2 means free block
  }
}

~FS(){
  unidad.close();
  //liberar directorio;
  //liberar bloquesLibres;
}

Bloque *obtenerBloque(int bloque){
  byte[TBLOQUE] b= unidad.seekg(bloque*TBLOQUE);
  return b;
}

void guardarBloque(Bloque bloque[]) {
  
}

bool nombreExiste (string nombre) {
  return buscar(nombre) != -1 ;
}

int buscar(string nombre) {
  //recorre el directorio para devolver el bloque donde empieza el archivo
  bool existe= false ;
  int indice = 0;
  while (!existe && indice < TDIRECTORIO) {
    if (directorio[indice].nombre == nombre) {
      existe = true;
    } else {
      indice++;
    }
  }
  if (!existe) {
    indice = -1;
  }
  return indice ;
}

void crear(string nombre){
  bool existe = false;
  existe = nombreExiste(nombre);
  if (!existe && nFiles < TDIRECTORIO) {
    int bloqueLibre = buscarBloqueLibre();
    if (bloqueLibre != -1) {
      directorio[nFiles].nombre = nombre;
      directorio[nFiles].fecha = "2025-09-09";
      directorio[nFiles].bloque = bloqueLibre;
      //marcar el bloque como ocupado
      nFiles++;
    }
  }
}

int buscarBloqueLibre() {
  int bloque = 0;
  int indice = 0;
  while (indice < TUNIDAD) {
    if (fat[indice] == -2) {
      bloque = indice;
      indice = TUNIDAD;
    }
    indice++;
  }
  return indice==TUNIDAD?-1:bloque;
}

void agregar(string nombre, char byte) {
  int posicionDirectorio = buscar(nombre);
  if (posicionDirectorio != -1) {
    //Buscar el bloque donde esta el archivo
    //busco el final del archivo
    //agrego al final
      //si tiene campo en el bloque agrego ahi
      //si no tiene campo en el bloque, pido bloque libre y lo agrego
      //si no hay bloque libre, no se puede agregar
  }
  //si existe
   /// recorrer hasta el -1
   ////   pedir bloque libre
   ///    guardar en el fat el bloque libre
   //     guardar en el bloque el dato nuevo
   //     guardar en el fat del bloque nuevo un -1
   ///    
}

//abrir (lectura/escritura)
  //buscar
  //marcar como abierto
  //guardar el permiso

  //UGA
  //USER GROUP ALL
  //111 001 000
  //chmod 755 x.dat
  //111 101 101
//borrar
//renombrar
//leer(nombre, posicion, tamano)
  //abrir
  //ubicarse en la posicion
  //leer esa cantidad de datos a partir de esa posicion

//escribir?


void imprimirDirectorio() {
  cout << endl ;

  for (int i=0 ; i<tDirectorio; i++) {
    cout << directorio[i].nombre << " - " << directorio [i].fecha << " - " << directorio[i].bloque << endl ;
  }

  cout << endl ;
}

//imprimir una matriz de los datos
void imprimirUnidad() {
  cout << endl ;
  for(int i=0; i<16; i++){
    cout << unidad[i] << " ";
    if(i == 3 || i == 7 || i == 11){
      cout << endl;
    }
  }
  cout << endl ;
}

void imprimir() {
  cout << "=== Directory ===" << endl;
  imprimirDirectorio();
  cout << "=== Storage Unit ===" << endl;
  imprimirUnidad();
}

};

int main() {
  FS *fs= new FS() ;

  fs->crear("a.dat") ;
  fs->agregar("a.dat", 'a') ;
  fs->imprimir() ;

  fs->crear("b.dat") ;
  fs->agregar("b.dat",'b') ;
  fs->imprimir() ;
  fs->agregar("b.dat",'b') ;
  fs->imprimir() ;
}

