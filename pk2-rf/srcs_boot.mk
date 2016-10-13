#=======================================================================
# UCB CS250 Makefile fragment for benchmarks
#-----------------------------------------------------------------------
#

A_OBJS = \
	./lib/crt.o

C_OBJS = \
	./lib/string.o		\
	./lib/serial.o		\
	./lib/irq.o		\
	./lib/sysctrl.o		\
	./lib/timer.o		\
	./lib/watchdog.o	\
	./lib/syscall.o		\
	./boot/init.o		\
	./boot/delay.o		\
	./boot/tmcu_irq.o	\
	./boot/main.o		\
	./boot/register.o	\
	./boot/gsm_tx.o		\
	./boot/calib_gsm.o	\
	./boot/calib_wcdma.o	\
	./boot/auxiliary.o	\
	./boot/wcdma_rf_onoff.o

