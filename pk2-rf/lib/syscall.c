// See LICENSE for license details.
#include <string.h>
#include <util.h>
#include <serial.h>
#include <irq.h>

#ifdef CONFIG_IRQ_TEST
void irq_tester(int irqs);
#endif

#ifndef CONFIG_PROJ_TEST
void dispatch_isr(int irqs);
#endif

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

void do_illegal_inst(void)
{
}

void do_lsu(void)
{
}

void do_ecall(void)
{
	// TODO: add code here
	// This exception can be used as software interrupts
}

void board_init(int flag)
{
	// TODO: add code here
	// Initialize modules before jump into main()
}
