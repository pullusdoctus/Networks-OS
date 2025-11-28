# Makefile Tenedor y Servidor de figuras
# CI-0123 Grupo 6: IPv3

CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread -I.
LDFLAGS = -lssl -lcrypto -pthread

# Directorios
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Archivos objeto comunes
COMMON_OBJS = $(OBJ_DIR)/VSocket.o $(OBJ_DIR)/Socket.o $(OBJ_DIR)/SSLSocket.o

# Targets
all: directories $(BIN_DIR)/tenedor $(BIN_DIR)/servidor

directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

############################
#        Tenedor
############################
$(BIN_DIR)/tenedor: $(OBJ_DIR)/Tenedor.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/Tenedor.o: Tenedor.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

############################
#   Servidor de Figuras
############################
$(BIN_DIR)/servidor: $(OBJ_DIR)/SSLServer.o $(OBJ_DIR)/FileSystem.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/SSLServer.o: SSLServer.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/FileSystem.o: FileSystem.cc FileSystem.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

############################
#     Objetos comunes
############################
$(OBJ_DIR)/Socket.o: Socket.cc Socket.h VSocket.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/SSLSocket.o: SSLSocket.cc SSLSocket.h Socket.h VSocket.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/VSocket.o: VSocket.cc VSocket.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

############################
#      Limpieza
############################
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

############################
#   Ejecutar componentes
############################
run-tenedor:
	$(BIN_DIR)/tenedor

run-servidor1:
	$(BIN_DIR)/servidor 8080 127.0.0.1 8082 127.0.0.1 4321 127.0.0.1

run-servidor2:
	$(BIN_DIR)/servidor 8083 127.0.0.1 8084 127.0.0.1 4321 127.0.0.2

############################
#         Pruebas
############################
test: all
	@echo "Iniciando pruebas del sistema..."
	@echo "1. Iniciando Tenedor..."
	@$(BIN_DIR)/tenedor &
	@sleep 2
	@echo "2. Iniciando Servidor 1..."
	@$(BIN_DIR)/servidor 8080 127.0.0.1 8082 127.0.0.1 4321 127.0.0.1 &
	@sleep 2
	@echo "Sistema listo para pruebas"

.PHONY: all clean directories run-tenedor run-servidor1 run-servidor2 test