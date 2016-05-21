CC=gcc
APP=main

LIBDIR=libs
INCDIR=inc
SRCDIR=srcs

all:app

app:lib src
	$(CC) -I$(INCDIR) -o $(APP) *.o
lib:
	$(CC) -I$(INCDIR) -c $(LIBDIR)/*.c
src:
	$(CC) -I$(INCDIR) -c $(SRCDIR)/*.c
clean:
	rm -f *.o
	rm -f $(APP)
	rm -f $(SRCDIR)/*.o
	rm -f $(LIBDIR)/*.o
run:
	./$(APP)
