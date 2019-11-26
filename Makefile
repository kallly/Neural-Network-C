CC=gcc
CFLAGS= -Wall -ansi -pedantic -std=c11
LDFLAGS=
EXEC=rectangle

all: $(EXEC) clean

rectangle: Rectangle.o main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

main.o: Rectangle.h

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
