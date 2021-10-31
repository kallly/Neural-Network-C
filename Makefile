CC=gcc
CFLAGS= -g -Wall -pedantic -std=c11 -Wextra -Werror -Wno-format-contains-nul -Wformat-overflow -Wformat-security
LDFLAGS= -lm -lpthread
EXEC=ia

all: $(EXEC) clean

ia: Network.o Perceptron.o activation.o ImportExport.o main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

main.o: Perceptron.h  Network.h activation.h ImportExport.h

clean:
	rm -rf *.o
	
mrproper: clean
	rm -rf $(EXEC)

