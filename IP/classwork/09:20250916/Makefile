# compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -O2
INCLUDES = -I.
LIBS = -lssl -lcrypto -lpthread

# source files
VSOCKET_SOURCES = VSocket.cc
SOCKET_SOURCES = Socket.cc
SSLSOCKET_SOURCES = SSLSocket.cc
CLIENT_SOURCES = SSLClient.cc
SERVER_SOURCES = SSLServer.cc

# object files
VSOCKET_OBJS = $(VSOCKET_SOURCES:.cc=.o)
SOCKET_OBJS = $(SOCKET_SOURCES:.cc=.o)
SSLSOCKET_OBJS = $(SSLSOCKET_SOURCES:.cc=.o)
CLIENT_OBJS = $(CLIENT_SOURCES:.cc=.o)
SERVER_OBJS = $(SERVER_SOURCES:.cc=.o)

# executables
CLIENT_TARGET = SSLClient
SERVER_TARGET = SSLServer

# certificate files
CERT_FILE = ci0123.pem
KEY_FILE = key0123.pem

all: $(CLIENT_TARGET) $(SERVER_TARGET) certificates

# SSL Client
$(CLIENT_TARGET): $(CLIENT_OBJS) $(SSLSOCKET_OBJS) $(SOCKET_OBJS) $(VSOCKET_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# SSL Server
$(SERVER_TARGET): $(SERVER_OBJS) $(SSLSOCKET_OBJS) $(SOCKET_OBJS) $(VSOCKET_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# .cc to .o
%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# create SSL certificates if they don't exist
certificates: $(CERT_FILE) $(KEY_FILE)

$(CERT_FILE) $(KEY_FILE):
	@echo "Generating SSL certificate and key..."
	@if [ ! -f $(CERT_FILE) ] || [ ! -f $(KEY_FILE) ]; then \
		openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
		-keyout $(KEY_FILE) -out $(CERT_FILE) \
		-subj "/C=CR/ST=SanJose/L=SanPedro/O=UCR/OU=ECCI/CN=localhost"; \
		echo "Certificate and key generated successfully!"; \
	else \
		echo "Certificate and key files already exist."; \
	fi

clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) $(SSLSOCKET_OBJS) $(SOCKET_OBJS) $(VSOCKET_OBJS)
	rm -f $(CLIENT_TARGET) $(SERVER_TARGET)

# clean even certificates
distclean: clean
	rm -f $(CERT_FILE) $(KEY_FILE)

test-client: $(CLIENT_TARGET) certificates
	./$(CLIENT_TARGET) 127.0.0.1 4321

test-server: $(SERVER_TARGET) certificates
	./$(SERVER_TARGET) 4321

help:
	@echo "Available targets:"
	@echo "  all          - Build both client and server (default)"
	@echo "  client       - Build only SSL client"
	@echo "  server       - Build only SSL server"
	@echo "  certificates - Generate SSL certificates"
	@echo "  clean        - Remove object files and executables"
	@echo "  distclean    - Remove everything including certificates"
	@echo "  test-client  - Run client test"
	@echo "  test-server  - Run server test"
	@echo "  help         - Show this help message"

# Individual targets
client: $(CLIENT_TARGET)
server: $(SERVER_TARGET)

# Phony targets
.PHONY: all clean distclean test-client test-server help
.PHONY: certificates check-openssl client server
