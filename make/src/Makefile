CC	= gcc
APP	= main
OBJS	:= $(APP).o
SRCS	:= $(OBJS:.o=.c)

$(info mkfile_list=$(MAKEFILE_LIST))
curdir	:=$(CURDIR)
$(info curdir=$(CURDIR))
file	:=$(strip $(subst /, ,$(curdir)))
file	:=$(lastword $(file))
$(info file=$(file))

all: $(APP)
	@echo "build $(APP) done"
$(APP): $(OBJS)
	$(CC) -o $@ $^
$(OBJS): $(SRCS)
	$(CC) -c $^

clean:
	rm -f *.o
	rm -f $(APP)
