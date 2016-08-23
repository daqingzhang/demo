include ../../Makefile.common

INC := $(INCLUDE)
INC += -I$(TOP)/src/freertos/Config/
INC += -I$(TOP)/src/freertos/Source/include
INC += -I$(TOP)/src/freertos/Source/portable/RVDS/ARM_CM3
INC += -I$(TOP)/src/bsp/inc
INC += -I$(TOP)/src/app/inc

OS_SRCS := $(TOP)/src/freertos/Source
OS_PORT := $(TOP)/src/freertos/Source/portable/RVDS/ARM_CM3

ASFLAGS += $(INC)

CFLAGS += -c
CFLAGS += $(INC)

LIB := librtos.a

OBJS += $(OS_SRCS)/queue.o
OBJS += $(OS_SRCS)/tasks.o
OBJS += $(OS_SRCS)/list.o
OBJS += $(OS_PORT)/port.o

SRCS := $(OBJS:.o=.c)

ASSRCS := $(OS_PORT)/asm.S
ASOBJS += $(OS_PORT)/asm.o

all: $(LIB)

.PHONY: clean show

$(LIB): $(OBJS) $(ASOBJS)
	@echo "Building $@ ..."
	$(AR) cr $@ $^
	@echo "done."

$(ASOBJS): $(ASSRCS)
	@echo "Building $@ ..."
	@echo "AS flags $(ASFLAGS) ..."
	$(AS) $(ASFLAGS) -o $@ $^
	@echo "done."

$(OBJS): $(SRCS)
	@echo "Building $@ ..."
	$(CC) $(CFLAGS) $^
	@echo "done."

show: $(SRCS)
	@echo "TOP = $(TOP)"
	@echo "INC = $(INC)"
	@echo "@ =  $@ ..."
	@echo "^ = $^"
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"

clean:
	rm -f *.o
	rm -f $(LIB)
