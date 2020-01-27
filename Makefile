CC=gcc
CFLAGS= -Wall -pedantic -std=c11 -Wextra -Werror -Wno-format-contains-nul -Wformat-overflow -Wformat-security
LDFLAGS= -lm -ljson-c
EXEC=ia

all: $(EXEC)

ia: Network.o Perceptron.o activation.o main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

main.o: Perceptron.h  Network.h activation.o

clean:
	rm -rf *.o
	
mrproper: clean
	rm -rf $(EXEC)

