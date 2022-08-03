CXX = g++
LIBS = -lSDL2
CXXFLAGS = -std=c++11 -Wall -pedantic
VOLTAGE_PATH = ../Voltage/src
VOLTAGE_SOURCES = $(filter-out $(VOLTAGE_PATH)/Timer.cpp, $(wildcard $(VOLTAGE_PATH)/*.cpp))
VOLTAGE_OBJECTS = $(patsubst $(VOLTAGE_PATH)/%.cpp, ./%.o, $(VOLTAGE_SOURCES))

all: main

main: main.o voltage.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

voltage.a: $(VOLTAGE_OBJECTS)
	libtool -static -o $@ $(VOLTAGE_OBJECTS)

%.o: $(VOLTAGE_PATH)/%.cpp
	$(CXX) $(CXXFLAGS) -D VOLTAGE_EMULATOR -c -o $@ $^

clean:
	rm -f *.o voltage.a main
