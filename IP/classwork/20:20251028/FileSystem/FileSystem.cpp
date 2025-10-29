#include "FileSystem.h"

// Copyright 2025 pullusdoctus

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

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

  const std::size_t maxBlocksPerList = TBLOQUE / sizeof(int);

  if (!this->indirecto0) this->indirecto0 = new BloqueIndirecto();
  if (this->indirecto0->bloques.size() < maxBlocksPerList) {
    this->indirecto0->bloques.push_back(bloque);
    return true;
  }

  if (!this->indirecto1) this->indirecto1 = new BloqueIndirecto();
  if (this->indirecto1->bloques.size() < maxBlocksPerList) {
    this->indirecto1->bloques.push_back(bloque);
    return true;
  }

  return false;
}

std::vector<int> INodo::getIndirecto0() const {
  if (this->indirecto0) return this->indirecto0->bloques;
  return std::vector<int>();
}

std::vector<int> INodo::getIndirecto1() const {
  if (this->indirecto1) return this->indirecto1->bloques;
  return std::vector<int>();
}

int INodo::obtenerBloque(int bloque) {
  if (bloque == 0) return this->bloque0;
  if (bloque == 1) return this->bloque1;
  if (bloque == 2) return this->bloque2;
  if (bloque == 3) return this->bloque3;

  bloque -= 4;

  if (this->indirecto0) {
    if (bloque >= 0 &&
        bloque < static_cast<int>(this->indirecto0->bloques.size())) {
      return indirecto0->bloques[bloque];
    }
    bloque -= static_cast<int>(this->indirecto0->bloques.size());
  }

  if (this->indirecto1) {
    if (bloque >= 0 &&
        bloque < static_cast<int>(this->indirecto1->bloques.size())) {
      return indirecto1->bloques[bloque];
    }
  }

  return -1;
}

int INodo::obtenerUltimoBloque() {
  if (this->indirecto1 && !this->indirecto1->bloques.empty())
    return this->indirecto1->bloques.back();
  if (this->indirecto0 && !this->indirecto0->bloques.empty())
    return this->indirecto0->bloques.back();
  if (this->bloque3 != -1) return this->bloque3;
  if (this->bloque2 != -1) return this->bloque2;
  if (this->bloque1 != -1) return this->bloque1;
  if (this->bloque0 != -1) return this->bloque0;
  return -1;
}

FileSystem::FileSystem(bool crearNuevoDisco) : nFiles(0) {
  this->unidad.open("./unidad.bin",
                    std::ios::in | std::ios::out | std::ios::binary);

  if (!this->unidad.is_open()) {
    crearNuevoDisco = true;
  } else {
    this->unidad.seekg(0, std::ios::end);
    std::streamsize tUnidad = this->unidad.tellg();
    if (tUnidad != TUNIDAD) {
      this->unidad.close();
      crearNuevoDisco = true;
    } else {
      this->unidad.seekg(0, std::ios::beg);
    }
  }
  if (crearNuevoDisco) {
    std::cout << "Creating disk unit" << std::endl;
    std::ofstream nuevaUnidad("./unidad.bin", std::ios::binary);
    if (nuevaUnidad.is_open()) {
      std::vector<char> buffer(TUNIDAD, 0);
      nuevaUnidad.write(buffer.data(), buffer.size());
      nuevaUnidad.close();

      if (this->unidad.is_open()) this->unidad.close();
      this->unidad.open("./unidad.bin",
                        std::ios::in | std::ios::out | std::ios::binary);
    }
  }

  if (!this->unidad.is_open()) {
    throw std::runtime_error("Could not open or create disk unit");
  }

  this->directorio = new INodo[TDIRECTORIO];
  this->bitmap = new bool[CANTIDADBLOQUES];
  std::memset(this->bitmap, 0, CANTIDADBLOQUES * sizeof(bool));

  const int bitmapStart = BLOQUEDIRECTORIO + 1;
  const int bitsPerBlock = TBLOQUE * 8;
  const int bitmapBlocksNeeded =
      (CANTIDADBLOQUES + bitsPerBlock - 1) / bitsPerBlock;
  const int firstDataBlock = bitmapStart + bitmapBlocksNeeded;

  if (crearNuevoDisco) {
    for (int i = 0; i <= firstDataBlock && i < CANTIDADBLOQUES; i++) {
      this->bitmap[i] = true;
    }

    for (int i = 0; i < TDIRECTORIO; i++) {
      this->directorio[i] = INodo();
    }

    this->guardarBitmap();
    this->guardarDirectorio();

  } else {
    for (int i = 0; i <= firstDataBlock && i < CANTIDADBLOQUES; i++) {
      this->bitmap[i] = true;
    }

    this->unidad.clear();
    this->unidad.seekg(BLOQUEDIRECTORIO * TBLOQUE, std::ios::beg);

    for (int i = 0; i < TDIRECTORIO; i++) {
      Entrada e;
      this->unidad.read(reinterpret_cast<char*>(&e), sizeof(Entrada));
      if (e.nombre[0] != '\0') {
        this->directorio[i].nombre = std::string(e.nombre);
        this->directorio[i].fecha = "2025-09-09";
        this->nFiles++;
      }
    }

    // Read file block metadata
    int metaStart = bitmapStart + bitmapBlocksNeeded;
    this->unidad.seekg(metaStart * TBLOQUE, std::ios::beg);

    for (int i = 0; i < TDIRECTORIO; i++) {
      if (!this->directorio[i].nombre.empty()) {
        // Read direct blocks
        for (int d = 0; d < 4; d++) {
          int32_t blk;
          this->unidad.read(reinterpret_cast<char*>(&blk), sizeof(int32_t));
          if (blk >= 0 && blk < CANTIDADBLOQUES) {
            this->directorio[i].asignarBloque(blk);
            this->bitmap[blk] = true;
          }
        }

        // Read indirect blocks
        for (int level = 0; level < 2; level++) {
          int32_t count;
          this->unidad.read(reinterpret_cast<char*>(&count), sizeof(int32_t));

          for (int k = 0; k < count; k++) {
            int32_t blk = -1;
            this->unidad.read(reinterpret_cast<char*>(&blk), sizeof(int32_t));
            if (blk >= 0 && blk < CANTIDADBLOQUES) {
              this->directorio[i].asignarBloque(blk);
              this->bitmap[blk] = true;
            }
          }
        }
      }
    }

    try {
      this->cargarBitmap();
    } catch (...) {
      Bloque tmp;
      // Scan data blocks
      for (int b = firstDataBlock; b < CANTIDADBLOQUES; b++) {
        this->unidad.seekg(b * TBLOQUE, std::ios::beg);
        this->unidad.read(reinterpret_cast<char*>(&tmp), sizeof(Bloque));

        bool occupied = false;
        for (int j = 0; j < TBLOQUE && !occupied; j++) {
          if (tmp.datos[j] != '\0') occupied = true;
        }
        this->bitmap[b] = occupied;
      }
    }
  }
}

void FileSystem::crearDisco(uint64_t size_in_bytes) {
  std::ofstream disk(this->nombreDisco, std::ios::binary);
  if (!disk) {
    throw std::runtime_error("Error: No se pudo crear el archivo del disco.\n");
  }

  std::vector<char> buffer(TBLOQUE, 0);
  std::uint64_t written = 0;

  while (written < size_in_bytes) {
    std::uint64_t to_write =
        std::min<std::uint64_t>(TBLOQUE, size_in_bytes - written);
    disk.write(buffer.data(), to_write);
    written += to_write;
  }

  disk.close();
  std::cout << "Disco creado: " << this->nombreDisco << " (" << size_in_bytes
            << " bytes)\n";
}

bool FileSystem::nombreExiste(std::string nombre) {
  return this->buscarArchivoPorNombre(nombre) != -1;
}

std::vector<Entrada> FileSystem::leerDirectorio() {
  std::ifstream disk(this->nombreDisco, std::ios::binary);
  if (!disk) {
    throw std::runtime_error("Error: No se pudo abrir el disco.\n");
  }

  // Ir al bloque reservado para el directorio
  disk.seekg(BLOQUEDIRECTORIO * TBLOQUE, std::ios::beg);

  size_t maxEntradas = TBLOQUE / sizeof(Entrada);

  this->entradas.clear();
  for (size_t i = 0; i < maxEntradas; i++) {
    Entrada e;
    disk.read(reinterpret_cast<char*>(&e), sizeof(Entrada));

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
  this->unidad.seekg(bloque * TBLOQUE, std::ios::beg);
  this->unidad.read(reinterpret_cast<char*>(b), sizeof(Bloque));  // leerlo
  return b;
}

void FileSystem::guardarBloque(Bloque bloque[]) {
  // TODO(any)
  (void)bloque;
}

void FileSystem::inicializarBitmap() {
  for (int i = 0; i < CANTIDADBLOQUES; i++) this->bitmap[i] = false;
  if (BLOQUEDIRECTORIO >= 0 && BLOQUEDIRECTORIO < CANTIDADBLOQUES)
    this->bitmap[BLOQUEDIRECTORIO] = true;
  // number of bits per block when storing bitmap as raw bits
  int bitsPerBlock = TBLOQUE * 8;
  int bitmapBlocksNeeded = (CANTIDADBLOQUES + bitsPerBlock - 1) / bitsPerBlock;
  int bitmapStart = BLOQUEDIRECTORIO + 1;  // normalmente 1
  for (int i = 0; i < bitmapBlocksNeeded; i++) {
    int b = bitmapStart + i;
    if (b >= 0 && b < CANTIDADBLOQUES) this->bitmap[b] = true;
  }
  int bytesPerInodoBase =
      4 * sizeof(int32_t) + sizeof(int32_t) + sizeof(int32_t);
  int metaBytesNeeded = TDIRECTORIO * bytesPerInodoBase;
  int bytesPerBlock = TBLOQUE;
  int metaBlocksNeeded = (metaBytesNeeded + bytesPerBlock - 1) / bytesPerBlock;
  int metaStart = BLOQUEDIRECTORIO + 1 + bitmapBlocksNeeded;
  for (int i = 0; i < metaBlocksNeeded; i++) {
    int b = metaStart + i;
    if (b >= 0 && b < CANTIDADBLOQUES) this->bitmap[b] = true;
  }
}

void FileSystem::guardarBitmap() {
  if (!this->unidad.is_open()) return;

  std::vector<char> buffer(CANTIDADBLOQUES);
  for (int i = 0; i < CANTIDADBLOQUES; i++) buffer[i] = this->bitmap[i] ? 1 : 0;

  int bitsPerBlock = TBLOQUE * 8;
  int bitmapBlocksNeeded = (CANTIDADBLOQUES + bitsPerBlock - 1) / bitsPerBlock;
  int blocksAvailable = std::min(bitmapBlocksNeeded, CANTBLOQUESBITMAP);
  int bitmapStart = BLOQUEDIRECTORIO + 1;
  int bytesPerBlock = TBLOQUE;

  for (int blk = 0; blk < blocksAvailable; blk++) {
    std::vector<char> blkbuf(TBLOQUE, 0);
    int offset = blk * bytesPerBlock;
    int toCopy = std::min(bytesPerBlock, CANTIDADBLOQUES - offset);
    if (toCopy > 0) std::memcpy(blkbuf.data(), buffer.data() + offset, toCopy);
    int targetBlock = bitmapStart + blk;
    if (targetBlock >= 0 && targetBlock < CANTIDADBLOQUES) {
      this->unidad.clear();
      this->unidad.seekp(targetBlock * TBLOQUE, std::ios::beg);
      this->unidad.write(blkbuf.data(), blkbuf.size());
    }
  }
  this->unidad.flush();
}

void FileSystem::cargarBitmap() {
  if (!this->unidad.is_open()) throw std::runtime_error("Unidad no abierta");

  int bytesPerBlock = TBLOQUE;
  int bitsPerBlock = TBLOQUE * 8;
  int bitmapBlocksNeeded = (CANTIDADBLOQUES + bitsPerBlock - 1) / bitsPerBlock;
  int blocksAvailable = std::min(bitmapBlocksNeeded, CANTBLOQUESBITMAP);
  int bitmapStart = BLOQUEDIRECTORIO + 1;

  std::vector<char> buffer(CANTIDADBLOQUES, 0);
  for (int blk = 0; blk < blocksAvailable; blk++) {
    int targetBlock = bitmapStart + blk;
    if (targetBlock < 0 || targetBlock >= CANTIDADBLOQUES) break;
    this->unidad.seekg(targetBlock * TBLOQUE, std::ios::beg);
    std::vector<char> blkbuf(TBLOQUE);
    this->unidad.read(blkbuf.data(), blkbuf.size());
    int bytesPerBlock = TBLOQUE;
    int offset = blk * bytesPerBlock;
    int toCopy = std::min(bytesPerBlock, CANTIDADBLOQUES - offset);
    if (toCopy > 0) std::memcpy(buffer.data() + offset, blkbuf.data(), toCopy);
  }

  for (int i = 0; i < CANTIDADBLOQUES; i++) this->bitmap[i] = (buffer[i] != 0);
}

int FileSystem::buscarArchivoPorNombre(const std::string& nombre) {
  bool existe = false;
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
  return indice;
}

void FileSystem::crearInodo(std::string nombre) {
  if (nombre.empty() || nombre.length() > 15) {
    std::cerr << "Error: Invalid filename" << std::endl;
    return;
  }

  int slot = -1;
  for (int i = 0; i < TDIRECTORIO; i++) {
    if (this->directorio[i].nombre == nombre) {
      std::cerr << "Error: File already exists" << std::endl;
      return;
    }
    if (slot == -1 && this->directorio[i].nombre.empty()) {
      slot = i;
    }
  }

  if (slot == -1) {
    std::cerr << "Error: Directory full" << std::endl;
    return;
  }

  try {
    this->directorio[slot] = INodo();
    this->directorio[slot].nombre = nombre;
    this->directorio[slot].fecha = "2025-09-09";
    this->nFiles++;

    this->guardarDirectorio();
    this->guardarBitmap();
  } catch (const std::exception& e) {
    std::cerr << "Error creating file: " << e.what() << std::endl;
    this->directorio[slot] = INodo();  // Reset on failure
    if (this->nFiles > 0) this->nFiles--;
  }
}

int FileSystem::buscarBloqueLibre() {
  const int bitmapStart = BLOQUEDIRECTORIO + 1;
  const int bitmapBits = CANTIDADBLOQUES;
  const int bitsPerBlock = TBLOQUE * 8;
  const int bitmapBlocksNeeded = (bitmapBits + bitsPerBlock - 1) / bitsPerBlock;
  const int firstDataBlock = bitmapStart + bitmapBlocksNeeded;

  for (int i = firstDataBlock; i < CANTIDADBLOQUES; i++) {
    if (!this->bitmap[i]) {
      return i;
    }
  }

  return -1;
}

void FileSystem::marcarBloquesOcupados(const std::vector<int>& bloques) {
  for (int bloque : bloques) {
    if (bloque >= 0 && bloque < CANTIDADBLOQUES) {
      this->bitmap[bloque] = true;
    }
  }
  this->guardarBitmap();
}

void FileSystem::marcarBloquesLibres(const std::vector<int>& bloques) {
  for (int bloque : bloques) {
    if (bloque >= 0 && bloque < CANTIDADBLOQUES) {
      this->bitmap[bloque] = false;
    }
  }
  this->guardarBitmap();
}

bool FileSystem::agregar(std::string nombre, char byte) {
  int posicionDirectorio = this->buscarArchivoPorNombre(nombre);
  if (posicionDirectorio == -1) {
    std::cerr << "Error: File not found: " << nombre << std::endl;
    return false;
  }

  int bloqueArchivo =
      this->directorio[posicionDirectorio].obtenerUltimoBloque();
  int writePosition = 0;

  if (bloqueArchivo == -1) {
    bloqueArchivo = this->buscarBloqueLibre();
    if (bloqueArchivo == -1) {
      std::cerr << "Error: No free blocks available" << std::endl;
      return false;
    }

    if (!this->directorio[posicionDirectorio].asignarBloque(bloqueArchivo)) {
      std::cerr << "Error: Could not assign block to file" << std::endl;
      return false;
    }

    this->bitmap[bloqueArchivo] = true;
    Bloque* newBlock = new Bloque();
    std::memset(newBlock->datos, '\0', TBLOQUE);
    newBlock->datos[0] = byte;

    this->unidad.clear();
    this->unidad.seekp(bloqueArchivo * TBLOQUE, std::ios::beg);
    this->unidad.write(reinterpret_cast<char*>(newBlock), sizeof(Bloque));
    this->unidad.flush();
    delete newBlock;

    this->guardarBitmap();
    this->guardarDirectorio();
    return true;
  } else {
    Bloque* currentBlock = this->obtenerBloque(bloqueArchivo);
    bool needNewBlock = true;

    for (int i = 0; i < TBLOQUE; i++) {
      if (currentBlock->datos[i] == '\0') {
        writePosition = i;
        needNewBlock = false;
        break;
      }
    }

    if (needNewBlock) {
      int newBlock = this->buscarBloqueLibre();
      if (newBlock == -1) {
        std::cerr << "Error: No free blocks available" << std::endl;
        delete currentBlock;
        return false;
      }

      if (!this->directorio[posicionDirectorio].asignarBloque(newBlock)) {
        std::cerr << "Error: Could not assign new block to file" << std::endl;
        delete currentBlock;
        return false;
      }

      this->bitmap[newBlock] = true;
      delete currentBlock;

      currentBlock = new Bloque();
      std::memset(currentBlock->datos, '\0', TBLOQUE);
      bloqueArchivo = newBlock;
      writePosition = 0;

      this->guardarBitmap();
      this->guardarDirectorio();
    }

    currentBlock->datos[writePosition] = byte;

    this->unidad.clear();
    this->unidad.seekp(bloqueArchivo * TBLOQUE, std::ios::beg);
    this->unidad.write(reinterpret_cast<char*>(currentBlock), sizeof(Bloque));

    if (!this->unidad.good()) {
      std::cerr << "Error: Failed to write to block" << std::endl;
      delete currentBlock;
      return false;
    }

    this->unidad.flush();
    delete currentBlock;
  }

  return true;
}

char* FileSystem::leer(std::string nombre, int nbytes) {
  if (nombre.empty() || nbytes <= 0) {
    std::cerr << "Error: Invalid parameters for read operation" << std::endl;
    return nullptr;
  }

  int posicionDir = this->buscarArchivoPorNombre(nombre);
  if (posicionDir == -1) {
    std::cerr << "Error: File not found: " << nombre << std::endl;
    return nullptr;
  }

  char* bytesLeidos = new char[nbytes + 1];
  std::memset(bytesLeidos, '\0', nbytes + 1);
  int bytesCopiados = 0;

  int blockIdx = 0;
  bool foundEOF = false;

  while (bytesCopiados < nbytes && !foundEOF) {
    int numBloque = this->directorio[posicionDir].obtenerBloque(blockIdx++);
    if (numBloque == -1) break;

    this->unidad.clear();
    Bloque* bloque = this->obtenerBloque(numBloque);
    (void)numBloque;
    (void)bloque;
    if (!bloque) {
      std::cerr << "Error: Failed to read block " << numBloque << std::endl;
      continue;
    }

    for (int i = 0; i < TBLOQUE && bytesCopiados < nbytes; i++) {
      char c = bloque->datos[i];
      if (c == '\0') {
        foundEOF = true;
        break;
      }
      bytesLeidos[bytesCopiados++] = c;
    }

    delete bloque;
  }

  if (bytesCopiados == 0) {
    delete[] bytesLeidos;
    return nullptr;
  }

  bytesLeidos[bytesCopiados] = '\0';
  return bytesLeidos;
}

bool FileSystem::escribir(std::string nombre, const char* datos) {
  if (!datos) {
    std::cerr << "Error: Null data pointer" << std::endl;
    return false;
  }

  int posicionDir = this->buscarArchivoPorNombre(nombre);
  if (posicionDir == -1) {
    for (int i = 0; i < TDIRECTORIO; i++) {
      if (this->directorio[i].nombre.empty()) {
        posicionDir = i;
        break;
      }
    }
    if (posicionDir == -1) {
      std::cerr << "Error: No free directory entries" << std::endl;
      return false;
    }
  }

  try {
    size_t len = std::strlen(datos);
    size_t bytesWritten = 0;

    size_t blocksNeeded = (len + TBLOQUE - 1) / TBLOQUE;
    std::vector<int> newBlocks;

    for (size_t i = 0; i < blocksNeeded; i++) {
      int blk = this->buscarBloqueLibre();
      if (blk == -1) {
        std::cerr << "Error: No free blocks available for " << nombre
                  << std::endl;
        for (int allocated : newBlocks) {
          this->bitmap[allocated] = false;
        }
        return false;
      }
      newBlocks.push_back(blk);
      this->bitmap[blk] = true;
    }

    if (!this->directorio[posicionDir].nombre.empty()) {
      for (int i = 0; i < 4; i++) {
        int blk = this->directorio[posicionDir].obtenerBloque(i);
        if (blk >= 0) this->bitmap[blk] = false;
      }
      auto ind0 = this->directorio[posicionDir].getIndirecto0();
      auto ind1 = this->directorio[posicionDir].getIndirecto1();
      for (int blk : ind0)
        if (blk >= 0) this->bitmap[blk] = false;
      for (int blk : ind1)
        if (blk >= 0) this->bitmap[blk] = false;
    }

    this->directorio[posicionDir] = INodo();
    this->directorio[posicionDir].nombre = nombre;
    this->directorio[posicionDir].fecha = "2025-09-09";

    for (size_t i = 0; i < newBlocks.size(); i++) {
      int bloqueActual = newBlocks[i];

      if (!this->directorio[posicionDir].asignarBloque(bloqueActual)) {
        std::cerr << "Error: Failed to assign block " << bloqueActual << " to "
                  << nombre << std::endl;
        for (int blk : newBlocks) this->bitmap[blk] = false;
        return false;
      }

      Bloque* bloque = new Bloque();
      std::memset(bloque->datos, '\0', TBLOQUE);

      size_t offset = i * TBLOQUE;
      size_t bytesToWrite =
          std::min(len - offset, static_cast<size_t>(TBLOQUE));
      std::memcpy(bloque->datos, datos + offset, bytesToWrite);

      this->unidad.clear();
      this->unidad.seekp(bloqueActual * TBLOQUE, std::ios::beg);
      this->unidad.write(reinterpret_cast<const char*>(bloque), sizeof(Bloque));

      // Debug: report written block and first byte
      (void)bloqueActual;
      (void)bytesToWrite;

      if (!this->unidad.good()) {
        std::cerr << "Error: Failed to write block " << bloqueActual << " for "
                  << nombre << std::endl;
        delete bloque;
        return false;
      }

      delete bloque;
      bytesWritten += bytesToWrite;
    }

    if (bytesWritten != len) {
      std::cerr << "Error: Only wrote " << bytesWritten << " of " << len
                << " bytes to " << nombre << std::endl;
      return false;
    }

    this->guardarBitmap();
    this->guardarDirectorio();
    this->unidad.flush();
    return true;
  } catch (const std::exception& e) {
    std::cerr << "Error writing to " << nombre << ": " << e.what() << std::endl;
    return false;
  }
}

void FileSystem::guardarDirectorio() {
  if (!this->unidad.is_open()) {
    std::cerr << "Error: Storage unit not open" << std::endl;
    return;
  }

  try {
    this->unidad.clear();
    this->unidad.seekp(BLOQUEDIRECTORIO * TBLOQUE, std::ios::beg);

    for (int i = 0; i < TDIRECTORIO; i++) {
      Entrada e;
      std::memset(&e, 0, sizeof(Entrada));

      if (!this->directorio[i].nombre.empty()) {
        std::strncpy(e.nombre, this->directorio[i].nombre.c_str(),
                     sizeof(e.nombre) - 1);
        int firstBlock = this->directorio[i].obtenerBloque(0);
        e.indice = static_cast<int16_t>(firstBlock);
      }

      this->unidad.write(reinterpret_cast<const char*>(&e), sizeof(Entrada));

      if (!this->unidad.good()) {
        throw std::runtime_error("Failed to write directory entry " +
                                 std::to_string(i));
      }
    }

    std::streamoff written = TDIRECTORIO * sizeof(Entrada);
    if (written < TBLOQUE) {
      std::vector<char> pad(TBLOQUE - written, 0);
      this->unidad.write(pad.data(), pad.size());
      if (!this->unidad.good()) {
        throw std::runtime_error("Failed to write directory padding");
      }
    }

    int bitsPerBlock = TBLOQUE * 8;
    int bitmapBlocksNeeded =
        (CANTIDADBLOQUES + bitsPerBlock - 1) / bitsPerBlock;
    int metaStart = BLOQUEDIRECTORIO + 1 + bitmapBlocksNeeded;

    this->unidad.seekp(metaStart * TBLOQUE, std::ios::beg);
    for (int i = 0; i < TDIRECTORIO; i++) {
      if (!this->directorio[i].nombre.empty()) {
        for (int d = 0; d < 4; d++) {
          int32_t blk = this->directorio[i].obtenerBloque(d);
          this->unidad.write(reinterpret_cast<const char*>(&blk),
                             sizeof(int32_t));
        }

        auto writeIndirectList = [this](const std::vector<int>& list) {
          int32_t count = static_cast<int32_t>(list.size());
          this->unidad.write(reinterpret_cast<const char*>(&count),
                             sizeof(int32_t));
          for (int blk : list) {
            int32_t b = blk;
            this->unidad.write(reinterpret_cast<const char*>(&b),
                               sizeof(int32_t));
          }
        };

        writeIndirectList(this->directorio[i].getIndirecto0());
        writeIndirectList(this->directorio[i].getIndirecto1());

        if (!this->unidad.good()) {
          throw std::runtime_error("Failed to write inode metadata for entry " +
                                   std::to_string(i));
        }
      }
    }

    this->unidad.flush();
    if (!this->unidad.good()) {
      throw std::runtime_error("Failed to flush all directory data");
    }
  } catch (const std::exception& e) {
    std::cerr << "Error in guardarDirectorio: " << e.what() << std::endl;
  }
}

bool FileSystem::renombrar(std::string nombre, std::string nombreNuevo) {
  int posicionDir = this->buscarArchivoPorNombre(nombre);
  if (posicionDir == -1) return false;
  this->directorio[posicionDir].nombre = nombreNuevo;
  this->guardarDirectorio();
  return true;
}

void FileSystem::imprimirDirectorio() {
  std::cout << std::endl;

  for (int i = 0; i < TDIRECTORIO; i++) {
    std::cout << directorio[i].nombre << " - " << directorio[i].fecha << " - "
              << directorio[i].obtenerBloque(0) << std::endl;
  }

  std::cout << std::endl;
}

bool FileSystem::eliminar(std::string nombre) {
  int posicionDir = this->buscarArchivoPorNombre(nombre);
  if (posicionDir == -1) {
    std::cerr << "Error: File not found: " << nombre << std::endl;
    return false;
  }

  auto cleanBlock = [this](int bloque) {
    if (bloque != -1) {
      this->bitmap[bloque] = false;

      Bloque* bloqueActual = new Bloque();
      std::memset(bloqueActual->datos, '\0', TBLOQUE);

      this->unidad.clear();
      this->unidad.seekp(bloque * TBLOQUE, std::ios::beg);
      this->unidad.write(reinterpret_cast<char*>(bloqueActual), sizeof(Bloque));

      delete bloqueActual;
    }
  };

  for (int i = 0; i < 4; i++) {
    cleanBlock(this->directorio[posicionDir].obtenerBloque(i));
  }

  std::vector<int> ind0 = this->directorio[posicionDir].getIndirecto0();
  for (int blk : ind0) {
    cleanBlock(blk);
  }

  std::vector<int> ind1 = this->directorio[posicionDir].getIndirecto1();
  for (int blk : ind1) {
    cleanBlock(blk);
  }

  if (posicionDir < this->nFiles - 1) {
    this->directorio[posicionDir] = this->directorio[this->nFiles - 1];
  }

  this->directorio[this->nFiles - 1] = INodo();
  this->nFiles--;

  this->unidad.flush();
  this->guardarBitmap();
  this->guardarDirectorio();
  return true;
}

bool FileSystem::modificar(std::string nombre, const char* datos) {
  // TODO(any): esto tiene sentido que sea exactamente lo mismo
  // que un reemplazar?
  return this->reemplazar(nombre, datos);
}

bool FileSystem::reemplazar(std::string nombre, const char* datosNuevos) {
  if (!this->eliminar(nombre)) return false;
  this->crearInodo(nombre);
  return this->escribir(nombre, datosNuevos);
}

std::string FileSystem::leerUnidad() {
  std::string bytesLeidos;

  for (int i = 0; i < TDIRECTORIO; i++) {
    bytesLeidos += directorio[i].nombre;
    bytesLeidos += ",";
  }

  return bytesLeidos;
}

void FileSystem::imprimirUnidad() {
  std::cout << std::endl;

  char byte;
  for (int i = 0; i < TUNIDAD; i++) {
    this->unidad.seekg(i, std::ios::beg);
    this->unidad.read(&byte, 1);
    if (byte != '\0') {
      std::cout << byte << "";
    } else {
      std::cout << "";
    }
    if (i % TBLOQUE == TBLOQUE - 1) std::cout << std::endl;
  }

  std::cout << std::endl;
}

// TODO(any): void FileSystem::imprimirBitmap

void FileSystem::imprimir() {
  std::cout << "=== Directory ===" << std::endl;
  this->imprimirDirectorio();
  std::cout << "=== Storage Unit ===" << std::endl;
  this->imprimirUnidad();
  // std::cout << "=== Bitmap ===" << std::endl;
  // this->imprimirBitmap();
}

FileSystem::~FileSystem() {
  try {
    if (this->unidad.is_open()) {
      this->guardarBitmap();
      this->guardarDirectorio();
      this->unidad.flush();
      this->unidad.close();
    }
  } catch (...) {
  }
  delete[] this->directorio;
  delete[] this->bitmap;
}
