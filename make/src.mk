$(info mkfile_list=$(MAKEFILE_LIST))
dirs	:= src lib
all:
	@echo "CHIP = $(CHIP)"
	@echo "GITVER = $(GITVER)"
	make -C src
	make -C lib
