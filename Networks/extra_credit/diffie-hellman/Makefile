CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

TARGET = dh

SOURCES = main.cpp diffieHellman.cpp
HEADERS = diffieHellman.h
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean all

.PHONY: all run clean rebuild