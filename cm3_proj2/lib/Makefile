include ../Makefile.common

LIBS+=libstm32.a
CFLAGSlib+=-c

SYS_INC = cmsis/device/stm32f10x/include
SYS_SRC = cmsis/device/stm32f10x/source
LIB_SRC = stm32f10x_std_driver/src

all: $(LIBS)

.PHONY: clean

$(LIBS):
	@echo "Building objects ..."
	cd $(STMLIB)/$(SYS_SRC) && \
		$(CC) $(CFLAGSlib) \
			system_stm32f10x.c
	cd $(STMLIB)/stm32f10x_std_driver/src && \
		$(CC) $(CFLAGSlib) \
			-D"assert_param(expr)=((void)0)" \
			*.c
	@echo "Building $@ ..."
	$(AR) cr $(STMLIB)/$@ \
		$(STMLIB)/$(SYS_SRC)/system_stm32f10x.o \
		$(STMLIB)/$(LIB_SRC)/*.o
	@echo "done."

clean:
	rm -f $(STMLIB)/$(SYS_SRC)/system_stm32f10x.o
	rm -f $(STMLIB)/$(LIB_SRC)/*.o
	rm -f $(LIBS)
