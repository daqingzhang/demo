include Makefile.common

all: lib src exe
	@echo "done !"
lib:
	@echo "to make libs ..."
	@(cd $(LIBDIR);$(MAKE))
src:
	@echo "to make srcs ..."
	@(cd $(SRCDIR);$(MAKE))
exe:
	@echo "to make app ..."
	@cp -p $(SRCDIR)/*.o $(LIBDIR)/*.o $(LIBDIR)/*.a $(OUTDIR)/.
	@$(CC) $(INCLUDE) -o $(BINDIR)/$(APP) $(OUTDIR)/*.o
clean:
	@(rm -f *.o;)
	@(cd $(OUTDIR);rm -f *.o *.a;)
	@(cd $(LIBDIR);rm -f *.o *.a;)
	@(cd $(SRCDIR);rm -f *.o;)
	@echo "clean all done ..."
run:
	@cd $(BINDIR);	\
	./$(APP)
show:
	@echo "libs:";	\
	ls -l $(LIBDIR);	\
	echo "------------------------------------------------";	\
	echo "srcs:";	\
	ls -l $(SRCDIR);	\
	echo "------------------------------------------------";	\
	echo "out:";	\
	ls -l $(OUTDIR);	\
	echo "------------------------------------------------";	\
	echo "bin:";	\
	ls -l $(BINDIR);	\
	echo "------------------------------------------------";

