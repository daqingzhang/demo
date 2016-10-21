#include <iomap.h>

  .text
  .globl _start
_start:
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, interrupt_handler
	jal x0, reset_handler
	jal x0, illegal_inst_handler
	jal x0, ecall_handler
	jal x0, lsu_handler

	.align 2
interrupt_handler:
	addi sp, sp, -IRQ_STACK_FRAME_SIZE
	SREG x1, 0x0*REGBYTES(sp)
	jal x1, save_regs
	la x1, restore_regs
	jal x0, do_interrupts

	.align 2
reset_handler:
# clear general purpose registers
	li  x1, 0
	li  x2, 0
	li  x3, 0
	li  x4, 0
	li  x5, 0
	li  x6, 0
	li  x7, 0
	li  x8, 0
	li  x9, 0
	li  x10,0
	li  x11,0
	li  x12,0
	li  x13,0
	li  x14,0
	li  x15,0
	li  x16,0
	li  x17,0
	li  x18,0
	li  x19,0
	li  x20,0
	li  x21,0
	li  x22,0
	li  x23,0
	li  x24,0
	li  x25,0
	li  x26,0
	li  x27,0
	li  x28,0
	li  x29,0
	li  x30,0
	li  x31,0

# initialize stack pointer
	li sp, CONFIG_SYS_INIT_SP

# clear the BSS segment
	la x3, __bss_start__
	la x4, __bss_end__
clrbss_s:
	beq x3,x4, clrbss_e
	sw  x0,0x0(x3)
	addi x3,x3,4
	jal x0,clrbss_s
clrbss_e:

# get core id, musb be single core
	csrr a0, mhartid
	li a1, 0
1:	bne a0, a1, 1b

# enable global interrupt
	li t0, 0x1
	csrs mstatus, t0

# to initialize gpio, pin-mux, uart, i2c ...
	addi x10,x0,0
	jal x1, board_init

# jump to main (argc = argv = 0)
	addi a0,zero,0
	addi a1,zero,0
	jal x1, main

# jump to dead loop after exit main
9:	nop
	j 9b

	.align 2
illegal_inst_handler:
	addi sp, sp, -IRQ_STACK_FRAME_SIZE
	SREG x1, 0x0*REGBYTES(sp)
	jal x1, save_regs
	la x1, restore_regs_excp
	jal x0, do_illegal_inst

	.align 2
lsu_handler:
	addi sp, sp, -IRQ_STACK_FRAME_SIZE
	SREG x1, 0x0*REGBYTES(sp)
	jal x1, save_regs
	la x1, restore_regs_excp
	jal x0, do_lsu

	.align 2
ecall_handler:
	addi sp, sp, -IRQ_STACK_FRAME_SIZE	//sp = sp - 128
	SREG x1, 0x0*REGBYTES(sp)		//[sp + 0] = ra
	jal x1, save_regs			//ra = pc + 4, pc = save_regs
	la x1, restore_regs_excp		//ra = restore_regs
	jal x0, do_ecall			//pc = do_ecall

	.align 2
save_regs:
	/* save X2 - X31 */
	SREG x2,  1*REGBYTES(sp)
	SREG x3,  2*REGBYTES(sp)
	SREG x4,  3*REGBYTES(sp)
	SREG x5,  4*REGBYTES(sp)
	SREG x6,  5*REGBYTES(sp)
	SREG x7,  6*REGBYTES(sp)
	SREG x8,  7*REGBYTES(sp)
	SREG x9,  8*REGBYTES(sp)
	SREG x10, 9*REGBYTES(sp)
	SREG x11, 10*REGBYTES(sp)
	SREG x12, 11*REGBYTES(sp)
	SREG x13, 12*REGBYTES(sp)
	SREG x14, 13*REGBYTES(sp)
	SREG x15, 14*REGBYTES(sp)
	SREG x16, 15*REGBYTES(sp)
	SREG x17, 16*REGBYTES(sp)
	SREG x18, 17*REGBYTES(sp)
	SREG x19, 18*REGBYTES(sp)
	SREG x20, 19*REGBYTES(sp)
	SREG x21, 20*REGBYTES(sp)
	SREG x22, 21*REGBYTES(sp)
	SREG x23, 22*REGBYTES(sp)
	SREG x24, 23*REGBYTES(sp)
	SREG x25, 24*REGBYTES(sp)
	SREG x26, 25*REGBYTES(sp)
	SREG x27, 26*REGBYTES(sp)
	SREG x28, 27*REGBYTES(sp)
	SREG x29, 28*REGBYTES(sp)
	SREG x30, 29*REGBYTES(sp)
	SREG x31, 30*REGBYTES(sp)

#ifdef CONFIG_SUPPORT_NESTED_IRQ
	csrr x31, mepc
	SREG x31, 31*REGBYTES(sp)
	LREG x31, 30*REGBYTES(sp)
#endif
	/* pc = ra, go back */
	jalr x0,x1

	.align 2
restore_regs_excp: /* MEPC = MEPC + 4 */
#ifdef CONFIG_SUPPORT_NESTED_IRQ
	LREG x1, 31*REGBYTES(sp)
	addi x1, x1, 4
	SREG x1, 31*REGBYTES(sp)
#else
	csrr x1, mepc
	addi x1, x1, 4
	csrw mepc, x1
	nop
	nop
	nop
	nop
#endif
restore_regs:
	/* restore X1 - X29 */
	LREG x1, 0x0*REGBYTES(sp)	/* ra = [sp + 0] */
	LREG x2,  1*REGBYTES(sp)
	LREG x3,  2*REGBYTES(sp)
	LREG x4,  3*REGBYTES(sp)
	LREG x5,  4*REGBYTES(sp)
	LREG x6,  5*REGBYTES(sp)
	LREG x7,  6*REGBYTES(sp)
	LREG x8,  7*REGBYTES(sp)
	LREG x9,  8*REGBYTES(sp)
	LREG x10, 9*REGBYTES(sp)
	LREG x11, 10*REGBYTES(sp)
	LREG x12, 11*REGBYTES(sp)
	LREG x13, 12*REGBYTES(sp)
	LREG x14, 13*REGBYTES(sp)
	LREG x15, 14*REGBYTES(sp)
	LREG x16, 15*REGBYTES(sp)
	LREG x17, 16*REGBYTES(sp)
	LREG x18, 17*REGBYTES(sp)
	LREG x19, 18*REGBYTES(sp)
	LREG x20, 19*REGBYTES(sp)
	LREG x21, 20*REGBYTES(sp)
	LREG x22, 21*REGBYTES(sp)
	LREG x23, 22*REGBYTES(sp)
	LREG x24, 23*REGBYTES(sp)
	LREG x25, 24*REGBYTES(sp)
	LREG x26, 25*REGBYTES(sp)
	LREG x27, 26*REGBYTES(sp)
	LREG x28, 27*REGBYTES(sp)
	LREG x29, 28*REGBYTES(sp)
	LREG x30, 29*REGBYTES(sp)

#ifdef CONFIG_SUPPORT_NESTED_IRQ
	LREG x31, 31*REGBYTES(sp)
	csrw mepc,x31
	nop
	nop
	nop
	nop
#endif
	LREG x31, 30*REGBYTES(sp)

	addi sp, sp, IRQ_STACK_FRAME_SIZE
	eret
/*
 * void core_irq_enable(void)
 */
.globl core_irq_enable
core_irq_enable:
	csrsi mstatus,0x1
	ret
/*
 * void core_irq_disable(void)
 */
.globl core_irq_disable
core_irq_disable:
	csrci mstatus,0x1
	ret
/*
 * void core_ecall(void);
 */
 .globl core_ecall
core_ecall:
	ecall
	ret
/*
 * unsigned int core_get_mstatus(void);
 */
.globl core_get_mstatus
core_get_mstatus:
	csrr a0, mstatus
	ret
/*
 * void core_set_mstatus(unsigned int status);
 */
.globl core_set_mstatus
core_set_mstatus:
	csrw mstatus, a0
	ret
