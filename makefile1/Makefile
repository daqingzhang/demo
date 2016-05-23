include Makefile.common

all: libs srcs app
	@echo "done !"

.PHONY: libs srcs clean run show

libs:
	@echo "to make libs ..."
	@(cd $(LIBDIR);$(MAKE))
srcs:
	@echo "to make srcs ..."
	@(cd $(SRCDIR);$(MAKE))
app:
	@echo "to make app ..."
	@cp -p $(SRCDIR)/*.o $(LIBDIR)/*.o $(LIBDIR)/*.a $(OUTDIR)/.
	@$(CC) $(INCLUDE) -o $(BINDIR)/$(APP) $(OUTDIR)/*.o
clean:
	@(rm -f *.o;)
	@(cd $(OUTDIR);rm -f *.o *.a;)
	@(cd $(LIBDIR);rm -f *.o *.a;)
	@(cd $(SRCDIR);rm -f *.o;)
	@(cd $(BINDIR);rm -f *;)
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

