#=======================================================================
# UCB CS250 Makefile fragment for benchmarks
#-----------------------------------------------------------------------
#

A_OBJS = \
	./boot/crt.o

C_OBJS = \
	./boot/string.o		\
	./boot/serial.o		\
	./boot/syscall.o	\
	./boot/gsm_tx.o		\
	./boot/calib_gsm.o	\
	./boot/calib_wcdma.o	\
	./boot/auxiliary.o	\
	./boot/wcdma_rf_onoff.o	\
	./boot/init.o		\
	./boot/register.o	\
	./boot/main.o
