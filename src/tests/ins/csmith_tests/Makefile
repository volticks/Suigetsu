CC=../../../../../Suigetsu/compiler/cross/bin/mn10300-elf-gcc
CFLAGS=-nostdlib -static -Wno-incompatible-pointer-types

CC_NTV=gcc

SRC = $(wildcard *.c)
PROGS_SIM = $(patsubst %.c,%_sim,$(SRC))
PROGS_NTV = $(patsubst %.c,%_ntv,$(SRC))

sim: $(PROGS_SIM)
ntv: $(PROGS_NTV)

%_sim: %.c
	$(CC) $(CFLAGS) -o $@ $<

%_ntv: %.c
	$(CC_NTV) -m32 -o $@ $<


clean: 
	rm -f $(PROGS_NTV) $(PROGS_SIM)
