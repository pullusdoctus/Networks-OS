# Compilador y flags
CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pthread -MMD -MP
INC      := -Icommon -Icore

# Fuentes COMUNES
SRC_COMMON := common/Thread.cpp common/Semaphore.cpp common/Client.cpp

# Fuentes del core y de la app
SRC_CORE   := core/tenedor.cpp core/servidor.cpp
SRC_APP    := app/main.cpp
SRC_ALL    := $(SRC_COMMON) $(SRC_CORE) $(SRC_APP)

# Objetos
OBJ_ALL := $(SRC_ALL:.cpp=.o)
DEPS    := $(OBJ_ALL:.o=.d)

BIN := sim

.PHONY: all clean
all: $(BIN)

$(BIN): $(OBJ_ALL)
	$(CXX) $(CXXFLAGS) $(INC) $(OBJ_ALL) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJ_ALL) $(DEPS) $(BIN)

-include $(DEPS)
