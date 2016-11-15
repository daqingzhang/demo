#=======================================================================
# UCB CS250 Makefile fragment for benchmarks
#-----------------------------------------------------------------------
#

A_OBJS = \
	./lib/crt_boot.o \
	./lib/core_op.o

C_OBJS = \
	./lib/syscall.o		\
	./lib/irq.o		\
	./lib/serial.o		\
	./lib/string.o		\
	./lib/misc.o		\
	./lib/sysctrl.o		\
	./lib/timer.o		\
	./lib/watchdog.o	\
	./boot/main.o		\
	./boot/delay.o		\
	./boot/tmcu_irq.o	\
	./boot/init.o		\
	./boot/register.o	\
	./boot/gsm_tx.o		\
	./boot/calib_gsm.o	\
	./boot/calib_wcdma.o	\
	./boot/auxiliary.o	\
	./boot/wcdma_rf_onoff.o

