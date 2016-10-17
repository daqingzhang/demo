// See LICENSE for license details.
#include <string.h>
#include <util.h>
#include <serial.h>
#include <irq.h>
#include <sysctrl.h>
#include <gpio.h>

#ifdef CONFIG_IRQ_TEST
void irq_tester(int irqs);
#endif
#ifdef CONFIG_TIMER_TEST
void timer_callback(int irqs);
#endif
#ifndef CONFIG_PROJ_TEST
void dispatch_isr(int irqs);
#endif

void do_interrupts(void)
{
	unsigned int irqs = irq_get_status(0xFFFFFFFF);

#ifdef CONFIG_IRQ_TEST
	irq_tester(irqs);//just for test
#endif
#ifdef CONFIG_TIMER_TEST
	timer_callback(irqs);
#endif
	/*
	 * We must clear IRQ status before process it.
	 * And this is used to support nested IRQ
	 * and preemption IRQ.
	 */
	irq_clr_pending(irqs);

	/*
	 * During the exception service program executes, the
	 * interrupt is disabled by default. We can re-enable
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
	sysctrl_soft_rst1_en(SOFT_RST1_RISCV);
	while(1);
}

void do_ecall(void)
{
	// TODO: add code here
	// This exception can be used as swi
}

void board_init(int flag)
{
	/* disable global IRQ */
	core_irq_enable();

	/* configure system clock */
	sysctrl_set_system_clock(CONFIG_SYSCLK_VALUE);

	/* don't bypass watchdog */
	sysctrl_bypass_watchdog(0);

	/* enable hardware error response */
	sysctrl_hwerr_response(1);

	/* reset hardware */
	sysctrl_soft_rst1_en(SOFT_RST1_TIMER0
				| SOFT_RST1_TIMER1
				| SOFT_RST1_TIMER2);
	nop();
	nop();
	nop();
	nop();
	sysctrl_soft_rst1_dis(SOFT_RST1_TIMER0
				| SOFT_RST1_TIMER1
				| SOFT_RST1_TIMER2);
	nop();
	nop();
	nop();
	nop();

	/* initial pin-mux */

	/* initial GPIOs,input */
	gpio_set_direction(0xFFFFFFFF,1);

	/* initial UART */
	serial_init();

	/* enable IRQs */
	irq_clr_pending(0xFFFFFFFF);
	core_irq_enable();
	//irq_enable(0xFFFFFFFF);
}
