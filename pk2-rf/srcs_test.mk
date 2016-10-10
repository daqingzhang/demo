#=======================================================================
# UCB CS250 Makefile fragment for benchmarks
#-----------------------------------------------------------------------
#

A_OBJS = \
	./boot/crt.o

C_OBJS = \
	./boot/string.o	\
	./boot/serial.o	\
	./boot/interrupts.o	\
	./boot/syscall.o	\
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
