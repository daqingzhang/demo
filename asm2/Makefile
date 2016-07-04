TC	= arm-none-eabi
CC	= $(TC)-gcc
LD	= $(TC)-ld -v
OBJCOPY = $(TC)-objcopy
OBJDUMP = $(TC)-objdump
AS	= $(TC)-as
AR	= $(TC)-ar
GDB	= $(TC)-gdb

COMMFLAGS := -g -mcpu=cortex-m3 -mthumb
ASFLAGS := $(COMMFLAGS)
CCFLAGS := $(COMMFLAGS)
LDFLAGS := -Tlinker.ld

OBJS	:= startup.o
OBJS	+= sum.o

SRCS	:= $(OBJS:.o=.S)

APP 	:= main
OUT	:= $(APP).elf
BIN	:= $(APP).bin
HEX	:= $(APP).hex
TXT	:= $(APP).txt
MAP	:= $(APP).map

all:$(OUT)

.PHONY: clean

$(OUT): $(OBJS)

	$(LD) $(LDFLAGS) -Map $(MAP) -o $@ $^
	@echo "gen $(OUT) done"

	$(OBJCOPY) -O binary $@ $(BIN)
	@echo "gen $(BIN) done"

	$(OBJCOPY) -O ihex $@ $(HEX)
	@echo "gen $(HEX) done"

	$(OBJDUMP) -S $@ > out.txt
	$(OBJDUMP) -d sum.o > sum.txt
	$(OBJDUMP) -d startup.o > startup.txt
	@echo "gen *.txt done"

$(OBJS):$(SRCS)
	$(AS) $(ASFLAGS) startup.S -o startup.o
	$(AS) $(ASFLAGS) sum.S -o sum.o

clean:
	rm -f $(OUT)
	rm -f *.o
	rm -f *.txt
	rm -f *.bin
	rm -f *.hex
	rm -f *.map
unusd:
	#LDFLAGS := -Ttext 0x80000000
	$(CC) $(LDFLAGS) -o $@ $^
