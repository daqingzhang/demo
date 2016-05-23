# src Makefile
include ../Makefile.common

APPDIR=$(SRCDIR)/app
BSPDIR=$(SRCDIR)/bsp
OBJDIR=$(SRCDIR)/obj
OSDIR=$(SRCDIR)/os

OS_CFG=$(OSDIR)/config
OS_SRC=$(OSDIR)/source
OS_PORT=$(OS_SRC)/portable/RVDS/ARM_CM3
OS_MEM=$(OS_SRC)/portable/MemMang/

OS_INCLUDE=-I$(INCLUDE)
OS_INCLUDE+=-I$(OS_CFG)
OS_INCLUDE+=-I$(OS_SRC)/include
OS_INCLUDE+=-I$(OS_PORT)
OS_INCLUDE+=-I$(APPDIR)/include

all: os bsp app applib

.PHONY: applib app os bsp clean tshow

applib:
	@echo "make applib ..."
	cp -p $(APPDIR)/*.o $(OBJDIR)/.;	\
	cp -p $(BSPDIR)/*.o $(OBJDIR)/.;	\
	cp -p $(OSDIR)/source/*.o $(OBJDIR)/.;	\
	cp -p $(OSDIR)/source/RVDS/ARM_CM3/*.o $(OBJDIR)/.

	$(AR) cr $(APPLIB) $(OBJDIR)/*.o
app:
	@echo "make app ..."
bsp:
	@echo "make bsp ..."
os:
	@echo "in $(OS_SRC) ..."
	cd $(OS_SRC);	\
	$(CC) $(CFLAGS) $(OS_INCLUDE)	\
		tasks.c timers.c queue.c list.c
	@echo "in $(OS_PORT) ..."
	cd $(OS_PORT);	\
	$(CC) $(CFLAGS) $(OS_INCLUDE)	\
		port.c;
	@echo "in $(OS_MEM) ..."
	cd $(OS_MEM);	\
	$(CC) $(CFLAGS) $(OS_INCLUDE)	\
		heap_2.c;
clean:
	@echo "clean src ..."
	rm -f $(APPLIB) $(OBJS)
	rm -f $(OSDIR)/*.o
	rm -f $(BSPDIR)/*.o
	rm -f $(APPDIR)/*.o
tshow:
		@echo "######################################################################################################"
		@echo "################# optimize settings: $(InfoTextLib), $(InfoTextSrc)"
		@echo "######################################################################################################"
