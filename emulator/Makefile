CXX = g++
LIBS = -lSDL2
CXXFLAGS = -std=c++11 -Wall -pedantic

VOLTAGE_PATH = ../Voltage/src
VOLTAGE_SOURCES = $(filter-out $(VOLTAGE_PATH)/Timer.cpp, $(wildcard $(VOLTAGE_PATH)/*.cpp))
VOLTAGE_OBJECTS = $(patsubst $(VOLTAGE_PATH)/%.cpp, %.o, $(VOLTAGE_SOURCES))
VOLTAGE_DEPENDS = $(patsubst %.o, %.d, $(VOLTAGE_OBJECTS))

all: main

main: main.o voltage.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

voltage.a: $(VOLTAGE_OBJECTS)
	libtool -static -o $@ $(VOLTAGE_OBJECTS)

-include $(VOLTAGE_DEPENDS)

%.o: $(VOLTAGE_PATH)/%.cpp Makefile
	$(CXX) $(CXXFLAGS) -D VOLTAGE_EMULATOR -MMD -c $< -o $@

clean:
	rm -f $(VOLTAGE_OBJECTS) $(VOLTAGE_DEPENDS) voltage.a main.o main
