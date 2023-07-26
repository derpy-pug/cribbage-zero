CC=gcc
CFLAGS=-I. -Iinclude -Wall -Wextra -Werror -Wpedantic -std=c99 -Wno-unused-parameter -g
DEPS = 

OBJDIR=obj
BINDIR=bin

SRC = $(wildcard src/*.c src/*/*.c src/*/*/*.c src/*/*/*/*.c)
OBJ = $(SRC:src/%.c=$(OBJDIR)/%.o)

$(shell find src -type d ! -path "src" -exec sh -c 'mkdir -p $(OBJDIR)/$${0#src/}' {} \;)
$(shell mkdir -p $(BINDIR))

$(OBJDIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cribbage: $(OBJ)
	$(CC) -o $(BINDIR)/$@ $^ $(CFLAGS)

make optimized:
	$(CC) -o $(BINDIR)/cribbage $(SRC) $(CFLAGS) -O3

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o $(OBJDIR)/**/*.o *~ core $(INCDIR)/*~ $(BINDIR)/*
	rm -rf $(OBJDIR) $(BINDIR)

run:
	./$(BINDIR)/cribbage
