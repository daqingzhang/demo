// See LICENSE for license details.
#include <string.h>
#include <util.h>
#include <serial.h>
#include <irq.h>

#define BOARD_INIT_ADDR  0x00011FF4
#define STR_ADDR_EXCP	 0x00011FF8
#define CALLEXIT_ADDR	 0x00011FFC
#define CALLEXIT_PASS	 0x900dc0de

void irq_tester(int irqs);

void do_illegal_inst(void)
{
	writel(0x84,STR_ADDR_EXCP);
}

void do_lsu(void)
{
	writel(0x8C,STR_ADDR_EXCP);
}

void do_ecall(void)
{
	writel(0x88,STR_ADDR_EXCP);
}

void dispatch_isr(int irqs);
void do_interrupts(void)
{
	unsigned int irqs = irq_get_status(HWP_IRQ,0xFFFFFFFF);

#ifdef CONFIG_IRQ_TEST
	irq_tester(irqs);//just for test
#endif

#ifndef CONFIG_PROJ_TEST
	dispatch_isr(irqs);
#endif

	writel(irqs,STR_ADDR_EXCP);
	irq_clr_pending(HWP_IRQ,irqs);
}

void board_init(int flag)
{
	writel(0x55AA,BOARD_INIT_ADDR);
}

void call_exit(int err)
{
	if(!err)
		err = CALLEXIT_PASS;
	writel(err,CALLEXIT_ADDR);
}
