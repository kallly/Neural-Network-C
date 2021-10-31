CC=gcc
CFLAGS=-g -Wall -pedantic -std=c11 -Wextra -Werror -Wno-format-contains-nul -Wformat-overflow -Wformat-security
LDFLAGS=-lm -lpthread

NAME=brain.bin
EXEC=$(NAME)

SRCDIR=src
OBJDIR=obj

_OBJ=Network.o Perceptron.o activation.o ImportExport.o main.o
_HEAD=Perceptron.h  Network.h activation.h ImportExport.h


OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))
HEAD = $(patsubst %,$(SRCDIR)/%,$(_HEAD))


all: $(EXEC)

$(NAME): $(OBJ)
	$(CC) -o $@  $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

main.o: $(HEAD)

clean:
	rm -rf *.o
	
mrproper: clean
	rm -rf $(EXEC)

