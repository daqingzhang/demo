CC	:= gcc

MDL	:= scull
TEST	:= main
OBJS	:= $(TEST).o
SRCS	:= $(OBJS:.o=.c)

ifneq ($(KERNELRELEASE),)
	obj-m := ${MDL}.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD	:= $(shell pwd)

.PHONY: all clean

all: $(MDL)

$(MDL):
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

$(TEST): $(OBJS)
	$(CC) -o $@ $^
$(OBJS): $(SRCS)
	$(CC) -c $^
clean:
	rm *.o *.ko .${MDL}.ko.cmd ${MDL}.mod.c .${MDL}.o.cmd .${MDL}.mod.o.cmd
	rm modules.order Module.symvers
	rm -rf .tmp_versions
mclean:
	rm $(TEST)
	rm *.o
endif
