
TC = arm-none-eabi
CC = $(TC)-gcc
OBJDUMP = $(TC)-objdump

.PHONY: all clean

all:
	$(CC) -c -O1 test1.c
	$(OBJDUMP) -d test1.o > test1.txt
clean:
	rm *.o
	rm *.txt
