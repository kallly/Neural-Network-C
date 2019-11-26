CC=gcc
CFLAGS= -Wall -ansi -pedantic -std=c11
LDFLAGS=
EXEC=ia

all: $(EXEC) run

ia: main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

main.o:

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)

run: 
	./${EXEC}
