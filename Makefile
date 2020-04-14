
CC=g++
FLAGS = -std=c++11 -Iinclude
INCLUDE = $(wildcard include/*.h)
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:src/%.cpp=build/%.o)

all: bin/main

bin/%: main/%.cpp $(OBJECTS) | bin/
	$(CC) $(FLAGS) $(OBJECTS) $< -o $@

bin/:
	mkdir -p bin

build/%.o: src/%.cpp $(INCLUDE) | build/
	$(CC) $(FLAGS) -c $< -o $@

build/: 
	mkdir -p build

clean:
	rm -rf build bin