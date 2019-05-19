CC	:=gcc
CCP	:=g++
CFLAGS	:=-g -O2
INC	:=-I./

APP	:=main
OBJS	:=main.o \
		basefile.o \
		a.o

SRCS	:=$(OBJS:.o=.c)

.PHONY: all clean

all: clean $(APP)
	@echo "done"

$(APP): $(OBJS)
	@echo "$^ --> $@"
	$(CCP) -o $@ $(INC) $(CFLAGS) $^

%.o: %.c
	@echo "$< --> $@"
	$(CC) -c $(INC)  $<

%.o: %.cpp
	@echo "$< --> $@"
	$(CCP) -c $(INC) $<

clean:
	rm -rf *.o
	rm -rf $(APP)
