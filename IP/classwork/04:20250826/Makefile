CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SERVER = server
FORK = fork

SERVER_SRC = server.cpp
FORK_SRC = fork.cpp
SOCKET_SRC = Socket.cc VSocket.cc

all: $(SERVER) $(FORK)

$(SERVER): $(SERVER_SRC) $(SOCKET_SRC)
	$(CXX) $(CXXFLAGS) -o $(SERVER) $(SERVER_SRC) $(SOCKET_SRC)

$(FORK): $(FORK_SRC) $(SOCKET_SRC)
	$(CXX) $(CXXFLAGS) -o $(FORK) $(FORK_SRC) $(SOCKET_SRC)

clean:
	rm -f $(SERVER) $(FORK)

rebuild: clean all

run-server: $(SERVER)
	./$(SERVER)

run-fork: $(FORK)
	./$(FORK)

setup:
	mkdir -p ascii_storage

help:
	@echo "Available targets:"
	@echo "  all        - Build both server and fork binaries (default)"
	@echo "  server     - Build only the server binary"
	@echo "  fork       - Build only the fork binary"
	@echo "  clean      - Remove compiled binaries"
	@echo "  rebuild    - Clean and rebuild everything"
	@echo "  setup      - Create ascii_storage directory"
	@echo "  run-server - Build and run the server"
	@echo "  run-fork   - Build and run the fork"
	@echo "  help       - Show this help message"

.PHONY: all clean rebuild setup run-server run-fork help
