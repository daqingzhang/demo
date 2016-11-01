#=======================================================================
# UCB VLSI FLOW: Makefile for riscv-bmarks
#-----------------------------------------------------------------------
# Yunsup Lee (yunsup@cs.berkeley.edu)
#

BOOT_DIR:= $(TOP_DIR)/boot
LIB_DIR := $(TOP_DIR)/lib
OUT_DIR := $(TOP_DIR)/out/boot

INCS	:= -I$(LIB_DIR)
INCS	+= -I$(LIB_DIR)/include
INCS	+= -I$(BOOT_DIR)
INCS	+= -I$(BOOT_DIR)/include
XLEN	?= 32

TARGET		 := boot
TARGET_RISCV	 :=$(TARGET).riscv
TARGET_MAP	 :=$(TARGET).map
TARGET_BIN	 :=$(TARGET).bin
TARGET_HEX	 :=$(TARGET).hex
TARGET_SYMBOLS	 :=$(TARGET).symbols
TARGET_RISCV_DUMP:=$(TARGET_RISCV).dump
TARGET_HEXDUMP	 :=$(TARGET_HEX).dump
TARGET_BINDUMP	 :=$(TARGET_BIN).dump

RISCV_PREFIX	:= riscv$(XLEN)-unknown-elf-
RISCV_GCC	:= $(RISCV_PREFIX)gcc
RISCV_OBJDUMP	:= $(RISCV_PREFIX)objdump
RISCV_OBJCOPY	:= $(RISCV_PREFIX)objcopy
RISCV_LINK	:= $(RISCV_GCC) -T linker_boot.ld $(INCS)
RISCV_GCC_OPTS	:= -mcmodel=medany -static -std=gnu99 -O2 -ffast-math -Wall -fno-common -fno-builtin-printf -m32 -march=IMXpulpv2 -msave-restore -mrvc
#RISCV_GCC_OPTS	:= -mcmodel=medany -static -std=gnu99 -Os -ffast-math -fno-common -fno-builtin-printf
RISCV_LINK_OPTS := -nostdlib -nostartfiles -ffast-math -lgcc -Wl,-Map=$(TARGET_MAP)
RISCV_OBJDUMP_OPTS := --disassemble-all --disassemble-zeroes --section=.text --section=.text.startup --section=.sdata --section=.data

#====================================================================
# MACRO DEFINITIONS
#====================================================================
TARGET_DEFS	:= -DPREALLOCATE=1 -DHOST_DEBUG=0
TARGET_DEFS	+= -DCONFIG_SUPPORT_NESTED_IRQ
#====================================================================

include srcs_boot.mk

all: $(TARGET_BINDUMP) $(TARGET_HEXDUMP) $(TARGET_RISCV_DUMP)

$(TARGET_HEXDUMP): $(TARGET_HEX)
	hexdump $< > $@

$(TARGET_BINDUMP): $(TARGET_BIN)
	xxd $< > $@

$(TARGET_RISCV_DUMP): $(TARGET_RISCV)
	$(RISCV_OBJDUMP) $(RISCV_OBJDUMP_OPTS) $< > $@
	$(RISCV_OBJDUMP) -t $< > $(TARGET_SYMBOLS)
	$(RISCV_OBJDUMP) -h $<
#	$(RISCV_OBJDUMP) $(RISCV_OBJDUMP_OPTS) -r $(LIB_DIR)/crt.o > $(OUT_DIR)/crt.txt
	mv $(LIB_DIR)/*.o $(OUT_DIR)/.
	mv $(BOOT_DIR)/*.o $(OUT_DIR)/.
	cp $(TARGET_RISCV) $(OUT_DIR)/.
	cp $(TARGET_BIN) $(OUT_DIR)/.
	cp $(TARGET_HEX) $(OUT_DIR)/.

$(TARGET_HEX): $(TARGET_RISCV)
	$(RISCV_OBJCOPY) -O ihex $< $(TARGET).hex

$(TARGET_BIN): $(TARGET_RISCV)
	$(RISCV_OBJCOPY) -O binary $< $(TARGET).bin

$(TARGET_RISCV): $(C_OBJS) $(A_OBJS)
	$(RISCV_LINK) $^ -o $@ $(RISCV_LINK_OPTS)

%.o: %.c
	$(RISCV_GCC) $(RISCV_GCC_OPTS) $(TARGET_DEFS) \
	             -c $(INCS) $< -o $@
%.o: %.S
	$(RISCV_GCC) $(RISCV_GCC_OPTS) $(TARGET_DEFS) -D__ASSEMBLY__=1 \
	             -c $(INCS) $< -o $@

JUNK	:= $(TARGET_RISCV) $(TARGET_BIN) $(TARGET_HEX) $(TARGET_MAP) \
	$(TARGET_SYMBOLS) $(TARGET_RISCV_DUMP) $(TARGET_HEXDUMP) $(TARGET_BINDUMP)

clean:
	rm -rf $(JUNK)
	rm -f $(TEST_DIR)/*.o
	rm -f $(OUT_DIR)/*.riscv $(OUT_DIR)/*.o
	rm -f $(OUT_DIR)/*.bin $(OUT_DIR)/*.hex
