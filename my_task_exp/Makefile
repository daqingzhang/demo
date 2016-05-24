# general Makefile
# make OptLIB=0 OptSRC=0 all tshow 
include Makefile.common
LDFLAGS=$(COMMONFLAGS) -fno-exceptions -ffunction-sections -fdata-sections -L$(LIBDIR) -nostartfiles -Wl,--gc-sections,-Tlinker.ld

LDLIBS+=-lm
LDLIBS+=-libstm32
LDLIBS+=-libcmsis
LDLIBS+=-libbsp
#LDLIBS+=-libos

STARTUP=startup.s

all: lib cmsis bsp user
	@echo "make all ..."
	$(CC) -o $(PROGRAM).elf $(LDFLAGS) \
		-Wl,--whole-archive \
			user/libuser.a \
		-Wl,--no-whole-archive \
			$(LDLIBS)
	$(OBJCOPY) -O ihex $(PROGRAM).elf $(PROGRAM).hex
	$(OBJCOPY) -O binary $(PROGRAM).elf $(PROGRAM).bin
#Extract info contained in ELF to readable text-files:
	arm-none-eabi-readelf -a $(PROGRAM).elf > $(PROGRAM).info_elf
	arm-none-eabi-size -d -B -t $(PROGRAM).elf > $(PROGRAM).info_size
	arm-none-eabi-objdump -S $(PROGRAM).elf > $(PROGRAM).info_code
	arm-none-eabi-nm -t d -S --size-sort -s $(PROGRAM).elf > $(PROGRAM).info_symbol

.PHONY: lib cmsis bsp user os clean tshow

lib:
	@echo "Build $@ ..."
	@(cd $(LIBDIR);$(MAKE);)
#	$(MAKE) -C lib $@
cmsis:
	@echo "Build $@ ..."
	@(cd $(CMSISDIR);$(MAKE);)
#	$(MAKE) -C cmsis $@
bsp:
	@echo "Build $@ ..."
	@(cd $(BSPDIR);$(MAKE);)
#	$(MAKE) -C bsp $@
user:
	@echo "Build $@ ..."
	@(cd $(USERDIR);$(MAKE);)
#	$(MAKE) -C user $@
os:
	@echo "Build $@ ..."
#	$(MAKE) -C os $@
clean:
	@echo "Clean objs ..."
	$(MAKE) -C lib $@
	$(MAKE) -C cmsis $@
	$(MAKE) -C bsp $@
	$(MAKE) -C user $@
	$(MAKE) -C os $@
	rm -f $(PROGRAM).elf $(PROGRAM).hex $(PROGRAM).bin $(PROGRAM).info_elf $(PROGRAM).info_size
	rm -f $(PROGRAM).info_code
	rm -f $(PROGRAM).info_symbol
tshow:
	@echo "######################################################################################################"
	@echo "################# optimize settings: $(InfoTextLib), $(InfoTextSrc)"
	@echo "######################################################################################################"

flash:all
	./do_flash.pl $(TOP)/main.bin  
