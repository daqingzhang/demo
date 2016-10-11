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

RISCV_PREFIX	:= riscv$(XLEN)-unknown-elf-
RISCV_GCC	:= $(RISCV_PREFIX)gcc
RISCV_GCC_OPTS	:= -mcmodel=medany -static -std=gnu99 -O2 -ffast-math -fno-common -fno-builtin-printf
RISCV_LINK	:= $(RISCV_GCC) -T linker_boot.ld $(INCS)
RISCV_LINK_OPTS := -nostdlib -nostartfiles -ffast-math -lgcc
RISCV_OBJDUMP	:= $(RISCV_PREFIX)objdump
RISCV_OBJDUMP_OPTS := --disassemble-all --disassemble-zeroes --section=.text --section=.text.startup --section=.data

TARGET		 := boot
TARGET_RISCV	 :=$(TARGET).riscv
TARGET_RISCV_DUMP:=$(TARGET).riscv.dump
TARGET_LIST	 :=$(TARGET).list
TARGET_SYMBOLS	 :=$(TARGET).symbols

#====================================================================
# MACRO DEFINITIONS
#====================================================================
TARGET_DEFS	:= -DPREALLOCATE=1 -DHOST_DEBUG=0

TARGET_CYCLES	:= 80000

include srcs_boot.mk

all: $(TARGET_RISCV_DUMP)

$(TARGET_RISCV_DUMP): $(TARGET_RISCV)
	$(RISCV_OBJDUMP) $(RISCV_OBJDUMP_OPTS) $< > $@
	$(RISCV_OBJDUMP) $(RISCV_OBJDUMP_OPTS) -r $(LIB_DIR)/crt.o > $(OUT_DIR)/crt.txt
	$(RISCV_OBJDUMP) $(RISCV_OBJDUMP_OPTS) -r $(LIB_DIR)/syscall.o > $(OUT_DIR)/syscall.txt
	$(RISCV_OBJDUMP) $(RISCV_OBJDUMP_OPTS) -r $(LIB_DIR)/string.o > $(OUT_DIR)/string.txt
	$(RISCV_OBJDUMP) $(RISCV_OBJDUMP_OPTS) -r $(LIB_DIR)/serial.o > $(OUT_DIR)/serial.txt
	$(RISCV_OBJDUMP) $(RISCV_OBJDUMP_OPTS) -r $(LIB_DIR)/irq.o > $(OUT_DIR)/irq.txt
	$(RISCV_OBJDUMP) $(RISCV_OBJDUMP_OPTS) -r $(BOOT_DIR)/main.o > $(OUT_DIR)/main.txt
	$(RISCV_OBJDUMP) -t $< > $(OUT_DIR)/$(TARGET_SYMBOLS)
	$(RISCV_OBJDUMP) -h $< > $(OUT_DIR)/$(TARGET_LIST)
	mv $(LIB_DIR)/*.o $(OUT_DIR)/.
	mv $(BOOT_DIR)/*.o $(OUT_DIR)/.
	cp $(TARGET_RISCV) $(OUT_DIR)/.
	cp $(TARGET_RISCV_DUMP) $(OUT_DIR)/.

$(TARGET_RISCV): $(C_OBJS) $(A_OBJS)
	$(RISCV_LINK) $^ -o $@ $(RISCV_LINK_OPTS)

%.o: %.c
	$(RISCV_GCC) $(RISCV_GCC_OPTS) $(TARGET_DEFS) \
	             -c $(INCS) $< -o $@
%.o: %.S
	$(RISCV_GCC) $(RISCV_GCC_OPTS) $(TARGET_DEFS) -D__ASSEMBLY__=1 \
	             -c $(INCS) $< -o $@

JUNK += $(TARGET_RISCV) $(TARGET_RISCV_DUMP)

clean:
	rm -rf $(OBJS) $(JUNK)
	rm -f  *.o
	rm -f  $(BOOT_DIR)/*.o
	rm -f  $(OUT_DIR)/*.o
	rm -f  $(OUT_DIR)/*.txt
	rm -f  $(OUT_DIR)/*.riscv
	rm -f  $(OUT_DIR)/*.list
	rm -f  $(OUT_DIR)/*.symbols
	rm -f  $(OUT_DIR)/*.riscv.dump
