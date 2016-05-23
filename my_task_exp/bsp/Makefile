# libs Makefile
include ../Makefile.common
LIBS+=lbsp.a
CFLAGSlib+=-c

all: bsp

.PHONY: bsp clean tshow

bsp: $(LIBS)

$(LIBS):
	@echo -n "Building $@ ..."
	@cd $(BSPDIR) && \
		$(CC) $(CFLAGSlib) \
		-I$(LIBDIR)/inc	\
		-I$(CMSISDIR)/inc	\
			*.c
	$(AR) cr $@ $(BSPDIR)/*.o
	@echo "done."

clean:
	rm -f $(BSPDIR)/*.o
	rm -f $(LIBS)
tshow:
	@echo "######################################################################################################"
	@echo "################# optimize settings: $(InfoTextLib), $(InfoTextSrc)"
	@echo "######################################################################################################"
