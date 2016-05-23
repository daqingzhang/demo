# libs Makefile
include ../Makefile.common
LIBS+=lcmsis.a
CFLAGSlib+=-c

all: cmsis

.PHONY: cmsis clean tshow

cmsis: $(LIBS)

$(LIBS):
	@echo -n "Building $@ ..."
	cd $(CMSISDIR) && \
		$(CC) $(CFLAGSlib) \
			system_stm32f10x.c	\
			core_cm3.c
#			startup/startup_stm32f10x_hd.s
	$(AR) cr $@ $(CMSISDIR)/*.o
	@echo "done."

clean:
	rm -f $(CMSISDIR)/*.o
	rm -f $(LIBS)
tshow:
	@echo "######################################################################################################"
	@echo "################# optimize settings: $(InfoTextLib), $(InfoTextSrc)"
	@echo "######################################################################################################"
