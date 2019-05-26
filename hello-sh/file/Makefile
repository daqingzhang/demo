AR	:=ar
CC	:=gcc
CCP	:=g++
CFLAGS	:=-g -O2
INC	:=-I./

APP	:=main
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

.PHONY: all clean lib

all: clean $(APP) $(LIBS)
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

%.o: %.c
	@echo "$< --> $@"
	$(CC) -c $(INC)  $<

%.o: %.cpp
	@echo "$< --> $@"
	$(CCP) -c $(INC) $<

clean:
	rm -rf *.o
	rm -rf $(APP)
