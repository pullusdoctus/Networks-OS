#include <cstdint>
#include <string>
#include <fstream>
#include <vector>

/**
 * @brief Tamaño de cada bloque en bytes.
 */
// TODO(any): decidir que nombre usar. independientemente, debe ser 256
const int TAMANIOBLOQUE = 256;
const int TBLOQUE = 256;

/**
 * @brief Bloque reservado para el directorio raíz.
 */
const int BLOQUEDIRECTORIO = 0;

/**
 * @brief Cantidad total de bloques en el disco.
 */
const int CANTIDADBLOQUES = 16384;

/**
 * @brief Tamaño total del disco en bytes.
 */
// TODO(any): decidir nombre. tambien, decidir si usar megabytes o mebibytes
const int TAMANIOBYTES = 1048576;  // 1 megabyte = 1000000 bytes, 1 mebibyte = 1024 * 1024 = 1048576 bytes
const int TUNIDAD = 16;

/**
 * @brief Cantidad de bloques reservados para almacenar el bitmap.
 */
const int CANTBLOQUESBITMAP = 32;

/**
 * @brief Bloque donde comienza el área de contenido de archivos.
 */
const int BLOQUEINICIOCONTENIDO = 33;

/**
 * @brief Tamanio del arreglo directorio en bytes?
 */
const int TDIRECTORIO = 3;

/**
 * @struct Entrada
 * @brief Representa una entrada de directorio en el sistema de archivos.
 */
struct Entrada {
    char nombre [10];   ///< Nombre del archivo (máximo 10 caracteres).
    int16_t indice;     ///< Número de bloque índice asociado al archivo.
};

/**
 * @struct Indices
 * @brief Representa un bloque índice que almacena referencias a bloques de datos.
 */
struct Indices {
    std::vector<int> indices; ///< Lista de bloques de datos asociados al archivo.
    int bloqueIndiceSiguiente; ///< Número del siguiente bloque índice (si existe).
};

class INodo {
  private:
    int bloque0;
    int bloque1;
    int bloque2;
    int bloque3;
    INodo* bloqueIndirecto0;
    INodo* bloqueIndirecto1;

  bool operator==(const INodo& other) const {
    return this->nombre == other.nombre;
  }
  
  public:
    std::string nombre;
    std::string fecha;

    INodo();
    ~INodo();

    bool asignarBloque(int bloque);
    int obtenerBloque0() { return this->bloque0; }
    int obtenerUltimoBloque();
};

struct Bloque {
  char datos[TBLOQUE]; // 4 bytes
};

/**
 * @class FileSystem
 * @brief Clase que implementa un sistema de archivos simple con directorio y bitmap.
 */

// TODO(any): journaling?
class FileSystem {
  public:
    /**
     * @brief Constructor de FileSystem.
     * @param crearNuevoDisco true para crear un nuevo disco, false para cargar uno existente.
     */
    FileSystem(bool crearNuevoDisco = false);

    /**
     * @brief Destructor de FileSystem.
     */
    ~FileSystem();

    /**
     * @brief Crea un archivo de disco binario con el tamaño especificado.
     * @param size_in_bytes Tamaño total del disco en bytes.
     */
    void crearDisco(uint64_t size_in_bytes);

    /**
     * @brief Lee el contenido del directorio.
     * @return Vector de entradas de directorio.
     */
    std::vector<Entrada> leerDirectorio();

    bool nombreExiste(std::string nombre);

    // ------------------- Manejo de archivos -------------------

    /**
     * @brief Guarda un archivo en el sistema de archivos.
     * @param nombre Nombre del archivo.
     * @param contenido Contenido del archivo como string.
     * @return true si se guardó con éxito, false en caso contrario.
     */
    bool guardarArchivo(const std::string& nombre, const std::string& contenido);

    /**
     * @brief Busca una entrada de directorio por nombre.
     * @param nombreBuscado Nombre del archivo a buscar.
     * @return Entrada encontrada (si no existe, debe manejarse en implementación).
     */
    int buscarArchivoPorNombre(const std::string& nombre);

    bool agregar(std::string nombre, char byte);

    char* leer(std::string nombre, int nbytes);

    bool escribir(std::string nombre, char* datos);

    bool renombrar(std::string nombre, std::string nombreNuevo); // devuelve falso en caso de fallo

    // ------------------- Manejo de inodos -------------------

    void crearInodo(std::string nombre);

    // ------------------- Manejo del bitmap -------------------

    /**
     * @brief Inicializa el bitmap en memoria, marcando los bloques reservados.
     */
    void inicializarBitmap();

    /**
     * @brief Carga el bitmap desde el disco hacia la memoria.
     */
    void cargarBitmap();

    /**
     * @brief Guarda el bitmap en disco desde la memoria.
     */
    void guardarBitmap();

    Bloque* obtenerBloque(int bloque);

    void guardarBloque(Bloque bloque[]);

    int buscarBloqueLibre();

    /**
     * @brief Marca una lista de bloques como ocupados en el bitmap.
     * @param bloques Vector con los números de bloque a marcar.
     */
    void marcarBloquesOcupados(const std::vector<int>& bloques);

    /**
     * @brief Marca una lista de bloques como libres en el bitmap.
     * @param bloques Vector con los números de bloque a liberar.
     */
    void marcarBloquesLibres(const std::vector<int>& bloques);

    /**
     * @brief Verifica si un bloque está libre.
     * @param bloque Número del bloque a verificar.
     * @return true si el bloque está libre, false si está ocupado.
     */
    bool estaLibre(int bloque);

    // ------------------- Impresiones -------------------

    void imprimirDirectorio();

    void imprimirUnidad();

    /**
     * @brief Imprime en consola el estado del bitmap.
     */
    void imprimirBitmap();

    void imprimir();

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

  private:
    // +--------------------+
    // | ATRIBUTOS PRIVADOS |
    // +--------------------+

    /**
     * @brief Nombre del archivo que representa el disco en el sistema de archivos.
     */
    std::string nombreDisco = "disco.bin";

    /**
     * @brief Entradas del directorio en memoria.
     */
    std::vector<Entrada> entradas;

    INodo* directorio;
    /**
     * @brief Bitmap en memoria. Cada bit representa un bloque (0/false = libre, 1/true = ocupado).
     */
    bool* bitmap;
    std::fstream unidad;
    int nFiles = 0;

    // +--------------------+
    // | MÉTODOS AUXILIARES |
    // +--------------------+
};
