# Linux Makefile
# Socket classes
#

CC  := gcc
CXX := g++
CFLAGS := -g -c
CXXFLAGS := -g -c
DEPS := Socket.h
TARGETS := ipv4-test.out ipv6-test.out

ALL := $(TARGETS)

%.o: %.cc $(DEPS)
	$(CXX) $(CXXFLAGS) $<

%.out: %.o Socket.o VSocket.o
	$(info $$^ = $^)
	$(CXX) -g $^ Socket.o VSocket.o -o $^

ipv4-test.out: ipv4-test.o Socket.o VSocket.o
	$(CXX) -g ipv4-test.o Socket.o VSocket.o -o $@

ipv6-test.out: ipv6-test.o Socket.o VSocket.o
	$(CXX) -g ipv6-test.o Socket.o VSocket.o -o $@

clean:
	rm -f *.o *.out

