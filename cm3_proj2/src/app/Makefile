include ../../Makefile.common

INC := $(INCLUDE)
INC += -I$(TOP)/src/freertos/Config/
INC += -I$(TOP)/src/freertos/Source/include
INC += -I$(TOP)/src/freertos/Source/portable/RVDS/ARM_CM3
INC += -I$(TOP)/src/bsp/inc
INC += -I$(TOP)/src/app/inc

CFLAGS += $(INC)
CFLAGS += -c

LIB := $(TOP)/src/$(APPLIB)

OBJS += startup.o
OBJS += main.o
#OBJS += mtasks.o

SRCS := $(OBJS:.o=.c)

all: $(LIB)

$(LIB): $(OBJS)
	@echo "Building $@ ..."
	$(AR) cr $@ $^

$(OBJS): $(SRCS)
	@echo "Building $@ ..."
	$(CC) $(CFLAGS) $^
	@echo "done."

.PHONY: clean show

show:
	@echo "TOP = $(TOP)"
	@echo "INC = $(INC)"
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"

clean:
	rm -f *.o
	rm -f $(LIB)
