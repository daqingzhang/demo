include ../Makefile.common

all:lib
	@echo "done !"
lib:
	@echo "make libs ..."
	$(CC) -I../inc -Iinc -c *.c
	$(AR) $(CRV) $(LIB) $^
clean:
	@rm -f *.o *.a
	@echo "clean libs done !"
