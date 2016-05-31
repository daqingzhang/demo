include ../../Makefile.common

INC := $(INCLUDE)
INC += -I$(TOP)/src/bsp/inc

CFLAGS += $(INC)
CFLAGS += -c

LIB := $(TOP)/src/$(BSPLIB)

OBJS += dev_usart.o
OBJS += dev_led.o
OBJS += dev_key.o

SRCS := $(OBJS:.o=.c)

all: $(LIB)

.PHONY: clean show

$(LIB): $(OBJS)
	@echo "Building $@ ..."
	$(AR) cr $@ $^
	@echo "done."

$(OBJS): $(SRCS)
	@echo "Building $@ ..."
	$(CC) $(CFLAGS) $^
	@echo "done."

show:
	@echo "TOP = $(TOP)"
	@echo "INC = $(INC)"
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"
clean:
	rm -f *.o
	rm -f $(LIB)
