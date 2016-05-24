include ../Makefile.common
LIBS+=libbsp.a
CFLAGSlib+=-c

all: bsp

.PHONY: bsp clean tshow

bsp: $(LIBS)

$(LIBS):
	@echo -n "Building $@ ..."
	@cd $(BSPDIR) && \
		$(CC) $(CFLAGSlib) \
			*.c
	@echo -n "Build in $(BSPDIR) ..."
	@cd $(BSPDIR) && \
		$(AR) cr $@ $(BSPDIR)/*.o
	@echo "done."
clean:
	rm -f $(BSPDIR)/*.o
	rm -f $(LIBS)
tshow:
	@echo "######################################################################################################"
	@echo "################# optimize settings: $(InfoTextLib), $(InfoTextSrc)"
	@echo "######################################################################################################"
