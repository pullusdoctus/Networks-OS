# Linux Makefile
CXX := g++
CXXFLAGS := -g -std=c++11
SOURCES := Socket.cc VSocket.cc
HEADERS := Socket.h VSocket.h

all: ForkMirrorServer.out MirrorClient.out ThreadMirrorServer.out

ForkMirrorServer.out: ForkMirrorServer.cc $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) ForkMirrorServer.cc $(SOURCES) -o $@

MirrorClient.out: MirrorClient.cc $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) MirrorClient.cc $(SOURCES) -o $@

ThreadMirrorServer.out: ThreadMirrorServer.cc $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) ThreadMirrorServer.cc $(SOURCES) -lpthread -o $@

clean:
	rm -f *.out

.PHONY: all clean
