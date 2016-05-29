include ../../Makefile.common

INC := $(INCLUDE)
INC += -I$(TOP)/src/freertos/Config/
INC += -I$(TOP)/src/freertos/Source/include
INC += -I$(TOP)/src/freertos/Source/portable/RVDS/ARM_CM3
INC += -I$(TOP)/src/bsp/inc
INC += -I$(TOP)/src/app/inc

CFLAGS += -c
CFLAGS += $(INC)

LIB := libapp.a

OBJS += startup.o
OBJS += mtasks.o
OBJS += main.o

SRCS := $(OBJS:.o=.c)

all: $(LIB)

.PHONY: clean show

$(LIB): $(OBJS)
	@echo "Building $@ ..."
	$(AR) cr $@ $^

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
