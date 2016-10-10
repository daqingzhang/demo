#=======================================================================
# UCB CS250 Makefile fragment for benchmarks
#-----------------------------------------------------------------------
#

C_OBJS = \
	./boot/string.o	\
	./boot/serial.o	\
	./boot/interrupts.o	\
	./boot/syscall.o	\
	./test/sram_rw.o	\
	./test/multiply.o	\
	./test/math.o		\
	./test/test_main.o

A_OBJS = \
	./boot/crt.o
