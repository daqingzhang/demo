AR	:=ar
CC	:=gcc
CCP	:=g++
OBJDUMP :=objdump

APP	:=main

CFLAGS	:=-g -O2 -Wl,-Map=$(APP).map
INC	:=-I./

OBJS	:=main.o \
		basefile.o \
		datafile.o \
		test.o \
		test2.o

SRCS	:=$(OBJS:.o=.c)

ifeq ($(GEN_LIB),1)
LIBS	:=lib.a
else
LIBS	:=
endif

.PHONY: all clean lib lst

all: clean $(APP) $(LIBS) lst
	@echo "done"

$(APP): $(OBJS)
	@echo "$^ --> $@"
	$(CCP) -o $@ $(INC) $(CFLAGS) $^ -lusb-1.0

$(LIBS): $(OBJS)
	@echo "$^ --> $@"
	$(AR) -rv $@ $^

rel: $(LIBS)
	cd ../
	git archive --format=tar -o $@.tar.gz HEAD file

lst: $(APP)
	$(OBJDUMP) -d $< > $(APP).lst
	$(OBJDUMP) -h $< > $(APP).sym
	$(OBJDUMP) -t $< > $(APP).sec

%.o: %.c
	@echo "$< --> $@"
	$(CC) -c $(INC)  $<

%.o: %.cpp
	@echo "$< --> $@"
	$(CCP) -c $(INC) $<

clean:
	rm -rf *.o *.lst *.sym *.sec *.map
	rm -rf $(APP)
