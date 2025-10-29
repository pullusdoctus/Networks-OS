# Linux Makefile
#

CC  := gcc
CXX := g++
CFLAGS := -g -c
CXXFLAGS := -g -c -std=c++11
LDFLAGS := -lssl -lcrypto

DEPS := FileSystem.h SSLSocket.h Socket.h VSocket.h

# IPs y puertos por defecto
IPV4 := 127.0.0.1
PORT4 := 3145
IPV6 := ::1
PORT6 := 5413

# Macros de compilación para IPv4/IPv6
CXXFLAGS_IPV4 := -g -c -std=c++11
CXXFLAGS_IPV6 := -g -c -std=c++11 -DUSE_IPV6

# Targets
TARGETS := SSLServerIPv4.out SSLServerIPv6.out 

# Regla por defecto
all: $(TARGETS)

# Reglas genéricas para .o
%.o: %.cc $(DEPS)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Servidor IPv4
SSLServerIPv4.o: SSLServer.cc $(DEPS)
	$(CXX) $(CXXFLAGS_IPV4) -o $@ $<

SSLServerIPv4.out: SSLServerIPv4.o VSocket.o SSLSocket.o Socket.o FileSystem.o
	$(CXX) $^ $(LDFLAGS) -o $@

# Servidor IPv6
SSLServerIPv6.o: SSLServer.cc $(DEPS)
	$(CXX) $(CXXFLAGS_IPV6) -o $@ $<

SSLServerIPv6.out: SSLServerIPv6.o VSocket.o SSLSocket.o Socket.o FileSystem.o
	$(CXX) $^ $(LDFLAGS) -o $@

# Reglas para ejecutar manualmente (en terminal separada)
runserver4:
	./SSLServerIPv4.out $(PORT4)

runserver6:
	./SSLServerIPv6.out $(PORT6)

# Limpieza
clean:
	rm -f *.o *.out
