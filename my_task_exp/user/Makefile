include ../Makefile.common

CFLAGSlib+=-c
#OBJS+=startup.o
OBJS+=main.o

SRCS+=main.c

LIBS+=libuser.a

all: user

.PHONY: user clean tshow

user: $(LIBS)

$(LIBS):
	@echo -n "Building $@ ..."
	@cd $(USERDIR) && \
		$(CC) $(CFLAGSlib) main.c
	@echo -n "build in $(USERDIR) ..."
	@cd $(USERDIR) && \
		$(AR) cr $@ $(OBJS)
	@echo "done"
clean:
		rm -f $(USERDIR)/$(OBJS)
		rm -f $(USERDIR)/$(LIBS)
tshow:
		@echo "######################################################################################################"
		@echo "################# optimize settings: $(InfoTextLib), $(InfoTextSrc)"
		@echo "######################################################################################################"
