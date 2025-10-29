# Linux Makefile
#

CC  := gcc
CXX := g++
CFLAGS := -g -c
CXXFLAGS := -g -c -std=c++11
LDFLAGS := -lssl -lcrypto

DEPS := SSLSocket.h Socket.h VSocket.h

# IPs y puertos por defecto
IPV4 := 127.0.0.1
PORT4 := 3145
IPV6 := ::1
PORT6 := 5413

# Macros de compilación para IPv4/IPv6
CXXFLAGS_IPV4 := -g -c -std=c++11
CXXFLAGS_IPV6 := -g -c -std=c++11 -DUSE_IPV6

# Targets
TARGETS := ClientIPv4.out ClientIPv6.out

# Regla por defecto
all: $(TARGETS)

# Reglas genéricas para .o
%.o: %.cc $(DEPS)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Cliente IPv4
ClientIPv4.o: Client.cc $(DEPS)
	$(CXX) $(CXXFLAGS_IPV4) -o $@ $<

ClientIPv4.out: ClientIPv4.o VSocket.o SSLSocket.o Socket.o
	$(CXX) $^ $(LDFLAGS) -o $@

# Cliente IPv6
ClientIPv6.o: Client.cc $(DEPS)
	$(CXX) $(CXXFLAGS_IPV6) -o $@ $<

ClientIPv6.out: ClientIPv6.o VSocket.o SSLSocket.o Socket.o
	$(CXX) $^ $(LDFLAGS) -o $@

# Reglas para ejecutar manualmente (en terminal separada)
runclient4:
	./ClientIPv4.out $(IPV4) $(PORT4)

runclient6:
	./ClientIPv6.out $(IPV6) $(PORT6)

# Limpieza
clean:
	rm -f *.o *.out
