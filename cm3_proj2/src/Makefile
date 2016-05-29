# src Makefile

include ../Makefile.common

OBJS+=startup.o
OBJS+=main.o

all: src

.PHONY: src clean tshow

src: $(ALIB)

$(ALIB): $(OBJS)
		$(AR) cr $(ALIB) $(OBJS)
clean:
	rm -f $(ALIB) $(OBJS)
