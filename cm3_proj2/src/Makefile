include ../Makefile.common

all: bsp app

.PHONY: bsp app clean

bsp:
	@echo "Building $@ ..."
	$(MAKE) -C $@
app:
	@echo "Building $@ ..."
	$(MAKE) -C $@
clean:
	$(MAKE) -C bsp $@
	$(MAKE) -C app $@
