$(info text=$(subst ll,dd,hello world))

src := $(wildcard lib/*.c)
obj := $(strip $(src))
obj := $(patsubst %.c,%.o,$(obj))
$(info obj1=$(obj))

$(info mkfile_list=$(MAKEFILE_LIST))
obj := $(patsubst lib/%.c,%.o,$(wildcard lib/*.c))
$(info obj2=$(obj))

cur_volt ?=0.8V
volt	?=0.8V 0.9V 1.0V
ifneq ($(filter $(volt),$(cur_volt)),)
$(info cur_volt=$(cur_volt) is valid !!!)
else
$(info cur_volt=$(cur_volt) is not valid !!!)
endif

volt	:=$(subst .,P,$(volt))
volt	:=$(addprefix VOLT_,$(volt))
$(info volt1=$(volt))

volt	:=$(patsubst %,CUR_%,$(volt))
$(info volt2=$(volt))

volt	:=$(addsuffix _VAL,$(volt))
$(info volt3=$(volt))

dep := $(obj:.o=.d)
$(info dep=$(dep))

key := $(findstring cc, aa/bb/ccc/dd/e.c)
$(info key=$(key))

key := $(findstring e.c, aa/bb/ccc/dd/e.c)
$(info key1=$(key))

key := $(findstring d.c, aa/bb/ccc/dd/e.c)
$(info key2=$(key))

$(info dir1=$(CURDIR))
$(info dir2=$(dir $(CURDIR)))
$(info dir3=$(dir $(lastword $(MAKEFILE_LIST))))
$(info dir4=$(notdir $(CURDIR)))

all:
	make -f src.mk CHIP=rda8850e GITVER=abcdef
clean:
	rm -f src/*.o
	rm -f src/main
