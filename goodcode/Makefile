
TC = arm-eabi
CC = $(TC)-gcc
OBJDUMP = $(TC)-objdump

.PHONY: all clean

all:
	$(CC) -c -O2 test1.c
	$(OBJDUMP) -d test1.o > info.txt
clean:
	rm *.o
	rm *.txt
