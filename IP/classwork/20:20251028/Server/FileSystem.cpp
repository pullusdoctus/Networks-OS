#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>  // para std::hex y std::setw

#include "FileSystem.h"

INodo::INodo()
  :  bloque0(-1), bloque1(-1), bloque2(-1), bloque3(-1),
    bloqueIndirecto0(nullptr), bloqueIndirecto1(nullptr),
    nombre(""), fecha("") {
    }

INodo::~INodo() {
  this->fecha = "";
  this->nombre = "";
  this->bloque0 = -1;
  this->bloque1 = -1;
  this->bloque2 = -1;
  this->bloque3 = -1;
  if (this->bloqueIndirecto0) delete this->bloqueIndirecto0;
  this->bloqueIndirecto0 = nullptr;
  if (this->bloqueIndirecto1) delete this->bloqueIndirecto1;
  this->bloqueIndirecto1 = nullptr;
}

bool INodo::asignarBloque(int bloque) {
  if (this->bloque0 == -1) {
    this->bloque0 = bloque;
    return true;
  } 
  if (this->bloque1 == -1) {
    this->bloque1 = bloque;
    return true;
  } 
  if (this->bloque2 == -1) {
    this->bloque2 = bloque;
    return true;
  } 
  if (this->bloque3 == -1) {
    this->bloque3 = bloque;
    return true;
  }
  return false;
}

int INodo::obtenerUltimoBloque() {
  if (this->bloque3 != -1) return this->bloque3;
  if (this->bloque2 != -1) return this->bloque2;
  if (this->bloque1 != -1) return this->bloque1;
  if (this->bloque0 != -1) return this->bloque0;
  return -1;  // no hay bloques asignados
}

// FileSystem::FileSystem(bool crearNuevoDisco) {
//     if (crearNuevoDisco) {
//         crearDisco(TAMANIOBYTES); // Crear disco de 1MB
//         // Inicializar bitmap en ceros
//         inicializarBitmap();
//     }
//     // Cargar el bitmap en memoria
//     cargarBitmap();
// }

FileSystem::FileSystem(bool crearNuevoDisco) : nFiles(0) {
  // Intentar abrir
  this->unidad.open("./unidad.bin", std::ios::in | std::ios::out | std::ios::binary);
  if (!this->unidad.is_open()) {
    crearNuevoDisco = true;
  } else {  // unidad existe
    this->unidad.seekg(0, std::ios::end);
    std::streamsize tUnidad = this->unidad.tellg();
    std::streamsize tEsperado = TUNIDAD * TBLOQUE;

    if (tUnidad != tEsperado) {
      // si la unidad no tiene el tamanio correcto, se recrea
      this->unidad.close();
      crearNuevoDisco = true;
    } else {
      this->unidad.seekg(0, std::ios::beg);  // volver al inicio
    }
  }

  if (crearNuevoDisco) {
    std::cout << "Creando unidad" << std::endl;
    std::ofstream nuevaUnidad("./unidad.bin", std::ios::binary);
    if (nuevaUnidad.is_open()) {
      // inicializar
      std::vector<char> buffer(TUNIDAD * TBLOQUE, 0);
      nuevaUnidad.write(buffer.data(), buffer.size());
      nuevaUnidad.close();
      this->unidad.open("./unidad.bin", std::ios::in | std::ios::out | std::ios::binary);
    }
  }

  // ya si no se pudo, estalle
  if (!this->unidad.is_open()) {
    throw std::runtime_error("No se pudo abrir o crear la unidad\n");
  }
  
  this->directorio = new INodo[TDIRECTORIO];  
  this->bitmap = new bool[TUNIDAD];
  // Falta cargar el directorio con el inodo del directorio
  for (int i = 0; i < TUNIDAD; i++) this->bitmap[i] = false;

  // Inicializar bloques con '\0'
  Bloque b;
  for (int i = 0; i < TBLOQUE; i++) b.datos[i] = '\0';
  for (int i = 0; i < TUNIDAD; i++) {
    unidad.seekp(i * TBLOQUE, std::ios::beg);
    unidad.write(reinterpret_cast<char*>(&b), sizeof(Bloque));
  }
  unidad.flush();
}

// FileSystem::~FileSystem() {
//     // Guardar el bitmap en disco al cerrar
//     guardarBitmap();
// }

FileSystem::~FileSystem() {
  this->unidad.close();
  //liberar directorio;
  //liberar bloquesLibres;
}


void FileSystem::crearDisco(uint64_t size_in_bytes) {
  std::ofstream disk(this->nombreDisco, std::ios::binary);
  if (!disk) {
    throw std::runtime_error("Error: No se pudo crear el archivo del disco.\n");
  }

  std::vector<char> buffer(TAMANIOBLOQUE, 0);
  std::uint64_t written = 0;

  while (written < size_in_bytes) {
    std::uint64_t to_write = std::min<std::uint64_t>(TAMANIOBLOQUE, size_in_bytes - written);
    disk.write(buffer.data(), to_write);
    written += to_write;
  }

  disk.close();
  std::cout << "Disco creado: " << this->nombreDisco << " (" << size_in_bytes << " bytes)\n";
}

bool FileSystem::nombreExiste(std::string nombre) {
  return this->buscarArchivoPorNombre(nombre) != -1 ;
}

std::vector<Entrada> FileSystem::leerDirectorio(){
  // Función para leer el directorio desde disco
  std::ifstream disk(this->nombreDisco, std::ios::binary);
  if (!disk) {
    throw std::runtime_error("Error: No se pudo abrir el disco.\n");
  }

  // Ir al bloque reservado para el directorio
  disk.seekg(BLOQUEDIRECTORIO * TAMANIOBLOQUE, std::ios::beg);

  // Calcular cuántas entradas caben en un bloque
  size_t maxEntradas = TAMANIOBLOQUE / sizeof(Entrada);

  this->entradas.clear();
  for (size_t i = 0; i < maxEntradas; i++) {
      Entrada e;
    disk.read(reinterpret_cast<char*>(&e), sizeof(Entrada));

    // Si el nombre está vacío, ya no hay más entradas
    if (e.nombre[0] == '\0') {
      break;
    }

    this->entradas.push_back(e);
  }

  disk.close();
  std::cout << "Directorio leído. Entradas: " << this->entradas.size() << "\n";
  return this->entradas;
}


// +--------+
// | BITMAP |
// +--------+

Bloque* FileSystem::obtenerBloque(int bloque) {
  Bloque* b = new Bloque();
  this->unidad.seekg(bloque*TBLOQUE, std::ios::beg);
  this->unidad.read(reinterpret_cast<char*>(b), sizeof(Bloque)); // leerlo
  return b;
}

void FileSystem::guardarBloque(Bloque bloque[]) {
  // TODO(any)
  (void)bloque;
}

// // Función para buscar una entrada por nombre en el directorio
// // Retorna true si la encuentra y coloca el índice en "indiceEncontrado"
// Entrada FileSystem::buscarEntradaPorNombre(const std::string& nombreBuscado) {
//     std::ifstream disk(this->nombreDisco, std::ios::binary);
//     if (!disk) {
//         throw std::runtime_error("Error: No se pudo abrir el disco.\n");

//     }

//     disk.seekg(BLOQUEDIRECTORIO * TAMANIOBLOQUE, std::ios::beg);
//     size_t maxEntradas = TAMANIOBLOQUE / sizeof(Entrada);
//     for (size_t i = 0; i < maxEntradas; i++) {
//         Entrada e;
//         disk.read(reinterpret_cast<char*>(&e), sizeof(Entrada));

//         if (e.nombre[0] == '\0') continue; // entrada vacía

//         if (std::string(e.nombre) == nombreBuscado) {
//             disk.close();
//             std::cout << "Archivo encontrado" << "\n" ;
//             return e; // encontrada
//         }
//     }

//     disk.close();
//     std::cout << "Archivo no existe" << "\n";
//     return Entrada{"", -1}; // no encontrada
//   };

int FileSystem::buscarArchivoPorNombre(const std::string& nombre) {
  //recorre el directorio para devolver el bloque donde empieza el archivo
  bool existe= false ;
  int indice = 0;
  while (!existe && indice < TDIRECTORIO) {
    if (this->directorio[indice].nombre == nombre) {
      existe = true;
    } else {
      indice++;
    }
  }
  if (!existe) {
    indice = -1;
  }
  // TODO(any): valor de retorno: int o Entrada?
  return indice;
}

// Se crea un inodo nuevo
void FileSystem::crearInodo(std::string nombre){
  bool existe = false;
  existe = this->nombreExiste(nombre);
  if (!existe && this->nFiles < TDIRECTORIO) {
    int bloqueLibre = this->buscarBloqueLibre();
    if (bloqueLibre != -1) {
      this->directorio[this->nFiles].nombre = nombre;
      this->directorio[this->nFiles].fecha = "2025-09-09";
      this->directorio[this->nFiles].asignarBloque(bloqueLibre);
      //marcar el bloque como ocupado
      this->bitmap[bloqueLibre] = true;
      this->nFiles++;
    }
  }
}

int FileSystem::buscarBloqueLibre() {
  int indice = 0;
  while (indice < TUNIDAD) {
    if (this->bitmap[indice] == false) {
      return indice;
    }
    indice++;
  }
  return -1;
}

bool FileSystem::agregar(std::string nombre, char byte) {
  int posicionDirectorio = this->buscarArchivoPorNombre(nombre);
  if (posicionDirectorio != -1) {  // no tiene bloques asignados
    // Primero se identifica cuál es el último bloque
    int bloqueArchivo = this->directorio[posicionDirectorio].obtenerUltimoBloque();

    if (bloqueArchivo == -1) {  // si el INodo no tiene bloques asignados
      int bloque = this->buscarBloqueLibre();
      if (bloque == -1) { return false ;}
      bool seAsigno = this->directorio[posicionDirectorio].asignarBloque(bloque);
      if (!seAsigno) {
        std::cerr << "Error: INodo lleno" << std::endl;
        return false;
      }
      this->bitmap[bloque] = true;
      bloqueArchivo = bloque;
    }
    // se escribe en el ultimo bloque
    // busco el final del archivo
    Bloque* bloqueActual = this->obtenerBloque(bloqueArchivo);
    int writePosition = -1; 

    for (int i = 0; i < TBLOQUE; i++) {
      if (bloqueActual->datos[i] == '\0') {
        writePosition = i;
        break;
      }
    }

    // En caso de que el bloque esté lleno, se pide otro
    if (writePosition == -1) {
      int bloque = this->buscarBloqueLibre();
      if (bloque == -1) { return false ;}
      bool seAsigno = this->directorio[posicionDirectorio].asignarBloque(bloque);
      if (!seAsigno) {
        std::cerr << "Error: INodo lleno" << std::endl;
        return false;
      }
      this->bitmap[bloque] = true;

      delete bloqueActual;  // sacar el bloque anterior de memoria
      bloqueActual = this->obtenerBloque(bloque);
      bloqueArchivo = bloque;
      writePosition = 0;  // al principio del bloque nuevo
    }
    
    bloqueActual->datos[writePosition] = byte;
    this->unidad.seekp(bloqueArchivo * TBLOQUE, std::ios::beg);
    this->unidad.write(reinterpret_cast<char*>(bloqueActual), sizeof(Bloque));
    this->unidad.flush();  // opcional, asegura que se escriba inmediatamente

    delete bloqueActual;
    return true;
  }
  return false;  // archivo no encontrado
}

char* FileSystem::leer(std::string nombre, int nbytes) {
  int posicionDir = this->buscarArchivoPorNombre(nombre);
  int bloqueArchivo = -1;

  if (posicionDir != -1) bloqueArchivo = this->directorio[posicionDir].obtenerBloque0();
  else return nullptr;

  if (bloqueArchivo == -1) return nullptr;  // archivo no existe
  
  char* bytesLeidos = new char[nbytes + 1];  // nbytes chars + '\0"
  this->unidad.seekg(bloqueArchivo * TBLOQUE, std::ios::beg);
  this->unidad.read(bytesLeidos, nbytes);
  bytesLeidos[nbytes] = '\0';
  return bytesLeidos;
}


bool FileSystem::escribir(std::string nombre, const char* datos) {
  for (char c : std::string(datos)) {
    if (!this->agregar(nombre, c)) return false;
  }
  return true;
}

bool FileSystem::renombrar(std::string nombre, std::string nombreNuevo) {
  int posicionDir = this->buscarArchivoPorNombre(nombre);
  if (posicionDir == -1) return false;
  this->directorio[posicionDir].nombre = nombreNuevo;
  return true;
}

void FileSystem::imprimirDirectorio() {
  std::cout << std::endl;

  for (int i=0 ; i<TDIRECTORIO; i++) {
    std::cout << directorio[i].nombre << " - "
              << directorio [i].fecha << " - "
              << directorio[i].obtenerBloque0() << std::endl;
  }

  std::cout << std::endl;
}

bool FileSystem::eliminar(std::string nombre) {
  int posicionDir = this->buscarArchivoPorNombre(nombre);
  if (posicionDir == -1) return false;
  int bloque0 = this->directorio[posicionDir].obtenerBloque0();
  if (bloque0 != -1) {
    int bloque1 = this->directorio[posicionDir].obtenerBloque1();
    int bloque2 = this->directorio[posicionDir].obtenerBloque2();
    int bloque3 = this->directorio[posicionDir].obtenerBloque3();
    int bloques[] = {bloque0, bloque1, bloque2, bloque3};
    for (int i = 0; i < 4; i++) {  // este 4 no deberia estar hard codeado
      // si hay algo en el bloque, se limpia
      if (bloques[i] != -1) {  
        this->bitmap[bloques[i]] = false;
        Bloque* bloqueActual = this->obtenerBloque(bloques[i]);
        for (int j = 0; j < TBLOQUE; j++) {
          bloqueActual->datos[j] = '\0';
        }
        this->unidad.seekp(bloques[i] * TBLOQUE, std::ios::beg);
        this->unidad.write(reinterpret_cast<char*>(bloqueActual), sizeof(Bloque));
        delete bloqueActual;
      }
    }
  }
  // pasar el ultimo archivo a esta posicion
  if (posicionDir < this->nFiles - 1) {
    this->directorio[posicionDir] = this->directorio[this->nFiles - 1];
  }
  // limpiar el ultimo inodo
  this->directorio[this->nFiles - 1].~INodo();
  this->nFiles--;
  this->unidad.flush();
  return true;
}

bool FileSystem::modificar(std::string nombre, const char* datos) {
  // TODO(any): esto tiene sentido que sea exactamente lo mismo
  // que un escribir?
  return this->escribir(nombre, datos);
}

bool FileSystem::reemplazar(std::string nombre, const char* datosNuevos) {
  if (!this->eliminar(nombre)) return false;
  this->crearInodo(nombre);
  return this->escribir(nombre, datosNuevos);
}

std::string FileSystem::leerUnidad() {
  std::string bytesLeidos;

  for (int i=0 ; i<TDIRECTORIO; i++) {
    bytesLeidos += directorio[i].nombre;
    bytesLeidos += ",";
  }

  return bytesLeidos;
}

//imprimir una matriz de los datos
void FileSystem::imprimirUnidad() {
  std::cout << std::endl;

  char byte;
  for (int i = 0; i < TUNIDAD * TBLOQUE; i++) {
    this->unidad.seekg(i, std::ios::beg);
    this->unidad.read(&byte, 1);
    if (byte != '\0') {
      std::cout << byte << "";
    } else {
      std::cout << "";
    }
    if (i % TBLOQUE == TBLOQUE-1) std::cout << std::endl; // salto de línea cada bloque
  }

  std::cout << std::endl;
}

// void FileSystem::imprimirBitmap

void FileSystem::imprimir() {
  std::cout << "=== Directory ===" << std::endl;
  this->imprimirDirectorio();
  std::cout << "=== Storage Unit ===" << std::endl;
  this->imprimirUnidad();
  // std::cout << "=== Bitmap ===" << std::endl;
  // this->imprimirBitmap();
}

