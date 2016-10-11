// See LICENSE for license details.
#include <string.h>
#include <util.h>
#include <serial.h>
#include <irq.h>

#define CALLEXIT_ADDR	 0x00011FFC
#define CALLEXIT_PASS	 0x900dc0de

void irq_tester(int irqs);

void do_illegal_inst(void)
{
}

void do_lsu(void)
{
}

void do_ecall(void)
{
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
	irq_clr_pending(HWP_IRQ,irqs);
}

void board_init(int flag)
{
}

void call_exit(int err)
{
	if(!err)
		err = CALLEXIT_PASS;
	writel(err,CALLEXIT_ADDR);
}
