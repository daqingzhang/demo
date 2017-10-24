TOP := $(dir $(word $(words $(MAKEFILE_LIST)), $(MAKEFILE_LIST)))
TOP := $(patsubst %/,%,$(TOP))

TOP2 := $(shell pwd)
TOP3 := $(dir $(TOP2))

#FIND := workspaces
FIND := workspace

TOP4 := $(subst $(FIND),hello,$(TOP3))
TOP5 := $(filter %/$(FIND),$(TOP3))

$(info checking param ...)

ifeq ($(findstring $(FIND),$(TOP3)),)
$(error not find $(FIND))
else
$(warning find str $(FIND))
endif

VER := $(shell make -v)
VER := $(strip $(word 3, $(VER)))

all:
	@echo "TOP = $(TOP)"
	@echo "TOP2 = $(TOP2)"
	@echo "TOP3 = $(TOP3)"
	@echo "TOP4 = $(TOP4)"
	@echo "TOP5 = $(TOP5)"
	@echo "Make version  =$(VER)"
