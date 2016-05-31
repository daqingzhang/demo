include ../Makefile.common

CFLAGSlib += -c

LIBS += libstm32.a

STM_DIR := $(LIBDIR)/stm32f10x_std_driver/src
SYS_DIR := $(LIBDIR)/cmsis/device/stm32f10x/source

OBJS += $(STM_DIR)/*.o
OBJS += $(SYS_DIR)/*.o

SRCS := $(OBJS:.o=.c)

all: $(LIBS)

.PHONY: clean

$(LIBS): $(OBJS)
	@echo "Building $@ ..."
	$(AR) cr $@ $(LIBDIR)/*.o
	@echo "done"

$(OBJS): $(SRCS)
	@echo "Builing $@ ..."
	$(CC) $(CFLAGSlib) \
		-D"assert_param(expr)=((void)0)" \
		$^
clean:
	rm -f *.o
	rm -f $(LIBS)
