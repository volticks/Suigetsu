
CFLAGS=-nostdlib -static

SRC = $(wildcard *.c)
PROGS = $(patsubst %.c,%,$(SRC))

all: $(PROGS)

%: %.c
	../../../../compiler/cross/bin/mn10300-elf-gcc $(CFLAGS) -o $@ $<

clean: 
	rm -f $(PROGS)
