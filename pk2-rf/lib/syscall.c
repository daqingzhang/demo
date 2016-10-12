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
	/*
	 * We must clear IRQ status before process it.
	 * And this is used to support nested IRQ
	 * and preemption IRQ.
	 */
	irq_clr_pending(HWP_IRQ,irqs);

	/*
	 * During the exception service program executes, the
	 * interrupt is disabled by default. We can re-enabled
	 * it to supoort interrupt nesting and preemption.
	 */
#ifdef CONFIG_SUPPORT_NESTED_IRQ
	core_irq_enable();
#endif
#ifndef CONFIG_PROJ_TEST
	dispatch_isr(irqs);
#endif
}

void do_illegal_inst(void)
{
}

void do_lsu(void)
{
	// TODO: reset CPU ...
	while(1);
}

void do_ecall(void)
{
	// TODO: add code here
	// This exception can be used as swi
}

void board_init(int flag)
{
	// TODO: add code here
	// Initialize hardware before jump into main()
}
