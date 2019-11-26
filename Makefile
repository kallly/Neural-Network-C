CC=gcc
CFLAGS= -Wall -pedantic -std=c11 -Wextra -Werror -Wno-format-contains-nul -Wformat-overflow -Wformat-security
LDFLAGS= -lm
EXEC=ia

all: $(EXEC) clean

ia: Perceptron.o activation.o main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

main.o: Perceptron.h

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
