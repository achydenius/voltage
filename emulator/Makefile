CC = g++
CFLAGS = -Wall -pedantic
OBJS = main.o
LIBS = -lSDL2

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(INCLUDES)

clean:
	rm -f *.o
