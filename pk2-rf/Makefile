ABS_TOP_DIR	:= /home/daqingzhang/workspace/github/demo/pk2-rf
INSTALLDIR	:= $(ABS_TOP_DIR)/$(PREFIX)
PREFIX          := target
XLEN		:= 32

REL_OUTDIR	:= out
REL_OBOOTDIR	:= $(REL_OUTDIR)/boot
REL_OTESTDIR	:= $(REL_OUTDIR)/test

GEN_EXE		:= python2.7
GEN_IN		:= test.hex
GEN_OUT		:= $(GEN_IN).spicode

all: clean fpga

fpga: test
	rm -f $(GEN_OUT)
	$(GEN_EXE) spicodegen.py $(GEN_IN)
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

.PHONY: test boot lib clean gencodes
