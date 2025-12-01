CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

TARGET = subnetter

SOURCES = main.cpp IP.cpp subnet.cpp subnetter.cpp

HEADERS = IP.h subnet.h subnetter.h

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