#ABS_TOP_DIR	:= /home/daqingzhang/workspace/github/demo/pk2-rf
ABS_TOP_DIR	:= /home/zhangdaqing/workspace/github/demo/pk2-rf
PREFIX          := target
INSTALLDIR	:= $(ABS_TOP_DIR)/$(PREFIX)
XLEN		:= 32
REL_OUTDIR	:= out
REL_OBOOTDIR	:= $(REL_OUTDIR)/boot
REL_OTESTDIR	:= $(REL_OUTDIR)/test

all: clean test

boot:
	mkdir -p $(REL_OUTDIR) $(REL_OBOOTDIR)
	$(MAKE) -f Makefile.boot TOP_DIR=$(ABS_TOP_DIR) XLEN=$(XLEN)
	ls -l $(REL_OBOOTDIR)
test:
	mkdir -p $(REL_OUTDIR) $(REL_OTESTDIR)
	$(MAKE) -f Makefile.test TOP_DIR=$(ABS_TOP_DIR) XLEN=$(XLEN)
	ls -l $(REL_OTESTDIR)
clean:
	$(MAKE) -f Makefile.boot TOP_DIR=$(ABS_TOP_DIR) clean
	$(MAKE) -f Makefile.test TOP_DIR=$(ABS_TOP_DIR) clean

.PHONY: test boot lib clean
