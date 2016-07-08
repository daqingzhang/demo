CC	= gcc
APP	= main
OBJS	:= $(APP).o
SRCS	:= $(OBJS:.o=.c)

all: $(APP)
	@echo "build $(APP) done"
$(APP): $(OBJS)
	$(CC) -o $@ $^
$(OBJS): $(SRCS)
	$(CC) -c $^

clean:
	rm -f *.o
	rm -f $(APP)
