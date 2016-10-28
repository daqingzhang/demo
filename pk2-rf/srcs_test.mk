#=======================================================================
# UCB CS250 Makefile fragment for benchmarks
#-----------------------------------------------------------------------
#

A_OBJS = \
	./lib/crt.o

C_OBJS = \
	./lib/syscall.o	\
	./lib/irq.o	\
	./lib/serial.o	\
	./lib/string.o	\
	./lib/misc.o	\
	./lib/sysctrl.o	\
	./lib/timer.o	\
	./lib/watchdog.o \
	./test/main.o

ifneq ($(findstring CONFIG_MULTIPLY_TEST,$(TARGET_DEFS)),)
C_OBJS += ./test/test_multiply.o
endif

ifneq ($(findstring CONFIG_MEM32_TEST,$(TARGET_DEFS)),)
C_OBJS += ./test/test_sram.o
endif

ifneq ($(findstring CONFIG_MATH_TEST,$(TARGET_DEFS)),)
C_OBJS += ./test/test_math.o
endif

ifneq ($(findstring CONFIG_IRQ_TEST,$(TARGET_DEFS)),)
C_OBJS += ./test/test_irq.o
endif

ifneq ($(findstring CONFIG_TIMER_TEST,$(TARGET_DEFS)),)
C_OBJS += ./test/test_timer.o
endif

ifneq ($(findstring CONFIG_LSU_TEST,$(TARGET_DEFS)),)
C_OBJS += ./test/test_lsu.o
endif

ifneq ($(findstring CONFIG_SERIAL_TEST,$(TARGET_DEFS)),)
C_OBJS += ./test/test_serial.o
endif

ifneq ($(findstring CONFIG_WDOG_TEST,$(TARGET_DEFS)),)
C_OBJS += ./test/test_wdog.o
endif

ifneq ($(findstring CONFIG_REGS_TEST,$(TARGET_DEFS)),)
C_OBJS += ./test/test_regs.o
endif
