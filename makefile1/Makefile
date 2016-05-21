CC=gcc
APP=main

LIBDIR=libs
INCDIR=inc
SRCDIR=srcs
OUTDIR=out
BINDIR=bin

all:app
	@echo "make done !"
app:lib src
	@echo "make app ..."
	@mv *.o $(OUTDIR)
	$(CC) -I$(INCDIR) -o $(BINDIR)/$(APP) $(OUTDIR)/*.o
lib:
	@echo "make libs ..."
	$(CC) -I$(INCDIR) -c $(LIBDIR)/*.c
src:
	@echo "make srcs ..."
	$(CC) -I$(INCDIR) -c $(SRCDIR)/*.c
clean:
	@rm -f *.o	\
	rm -f $(OUTDIR)/*.o	\
	rm -f $(BINDIR)/$(APP)
run:
	./$(BINDIR)/$(APP)
