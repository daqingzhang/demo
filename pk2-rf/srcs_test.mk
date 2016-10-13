#=======================================================================
# UCB CS250 Makefile fragment for benchmarks
#-----------------------------------------------------------------------
#

A_OBJS = \
	./lib/crt.o

C_OBJS = \
	./lib/string.o	\
	./lib/serial.o	\
	./lib/irq.o	\
	./lib/timer.o	\
	./lib/watchdog.o \
	./lib/sysctrl.o	\
	./lib/syscall.o	\
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

ifneq ($(findstring CONFIG_LSU_TEST,$(TARGET_DEFS)),)
C_OBJS += ./test/test_lsu.o
endif
