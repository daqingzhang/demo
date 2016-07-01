TC	= arm-none-eabi
CC	= $(TC)-gcc
LD	= $(TC)-ld -v
OBJCOPY = $(TC)-objcopy
OBJDUMP = $(TC)-objdump
AS	= $(TC)-as
AR	= $(TC)-ar
GDB	= $(TC)-gdb

AS_FLAGS = -g -mcpu=cortex-m3 -mthumb


OBJS	:= sum.o
SRCS	:= $(OBJS:.o=.S)

OUT	:= sum.elf
BIN	:= $(OUT:.elf=.bin)
TXT	:= $(OUT:.elf=.list)

all:$(OUT)

.PHONY: clean

$(OUT): $(OBJS)
	$(LD) -Ttext 0x0 -o $@ $^
	@echo "ld $(OUT) done"
	$(OBJCOPY) -O binary $@ $(BIN)
	$(OBJDUMP) -S $@ > $(TXT)

$(OBJS):$(SRCS)
	@echo "$@"
	@echo "$^"
	$(AS) $(AS_FLAGS) -o $@ $^
	@echo "obj done"

clean:
	rm -f $(OUT)
	rm -f $(BIN)
	rm -f $(TXT)
	rm -f $(OBJS)

