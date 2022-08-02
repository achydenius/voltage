CC = g++
CFLAGS = -std=c++11 -Wall
OBJS = main.o
LIBS = -lSDL2

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(INCLUDES)

clean:
	rm -f *.o
