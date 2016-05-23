# libs Makefile
include ../Makefile.common
LIBS+=libstm32.a
CFLAGSlib+=-c

all: libs

.PHONY: libs clean tshow

libs: $(LIBS)

$(LIBS):
	@echo -n "Building $@ ..."
	@cd $(STMLIB)/src && \
		$(CC) $(CFLAGSlib) \
			-D"assert_param(expr)=((void)0)" \
			-I$(LIBDIR)/inc	\
			-I$(CMSISDIR)	\
			-I$(USERDIR)	\
			*.c
	@echo "build in $(LIBDIR) ..."
	$(AR) cr $@ src/*.o 
	@echo "done."

clean:
	rm -f $(STMLIB)/src/*.o
	rm -f $(LIBS)
tshow:
	@echo "######################################################################################################"
	@echo "################# optimize settings: $(InfoTextLib), $(InfoTextSrc)"
	@echo "######################################################################################################"
