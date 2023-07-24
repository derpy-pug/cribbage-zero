CC=gcc
CFLAGS=-I. -Iinclude -Wall -Wextra -Werror -Wpedantic -std=c99 -Wno-unused-parameter -g
DEPS = 

OBJDIR=obj
BINDIR=bin

SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=$(OBJDIR)/%.o)

$(OBJDIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cribbage: $(OBJ)
	$(CC) -o $(BINDIR)/$@ $^ $(CFLAGS)

make optimized:
	$(CC) -o $(BINDIR)/cribbage $(SRC) $(CFLAGS) -O3

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCDIR)/*~ $(BINDIR)/*

run:
	./$(BINDIR)/cribbage
