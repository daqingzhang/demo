// See LICENSE for license details.
#include <string.h>
#include <util.h>
#include <serial.h>
#include <irq.h>
#include <sysctrl.h>
#include <gpio.h>

/************************************ Just for Test *********************************/
#ifdef CONFIG_PROJ_TEST

#ifdef CONFIG_IRQ_TEST
void irq_tester(int status);
#endif
#ifdef CONFIG_TIMER_TEST
void timer_callback(unsigned int status);
#endif
#ifdef CONFIG_WDOG_TEST
void wdog_callback(unsigned int status);
#endif
#ifdef CONFIG_SERIAL_TEST
void serial_callback(unsigned int status);
#endif
#ifdef CONFIG_ECALL_TEST
extern volatile int ecall_excp_done;
#endif

void do_interrupts(void)
{
	unsigned int status = irq_get_status(0xffffffff);

#ifdef CONFIG_SERIAL_TEST
	serial_callback(status);
#endif
#ifdef CONFIG_TIMER_TEST
	timer_callback(status);
#endif
#ifdef CONFIG_WDOG_TEST
	wdog_callback(status);
#endif
#ifdef CONFIG_IRQ_TEST
	irq_tester(status);//just for test
#endif
	irq_clr_pending(status);
}

void do_ecall(void)
{
#ifdef CONFIG_ECALL_TEST
	u32 status = core_get_mstatus();
	serial_puts("ecall exception in, mstatus is: ");
	print_u32(status);
	serial_puts("\n");
	ecall_excp_done = 1;
#endif
}
/************************************ Not for Test *********************************/
#else
void do_ecall(void)
{
	serial_puts("do_ecall\n");
	//TODO: add code here
}

void dispatch_isr(unsigned int status);
void do_interrupts(void)
{
	unsigned int status = irq_get_status(0xffffffff);

	/*
	 * Clear IRQ status before process ISR and ISR can be nested
	 * or preempted if global IRQ enabled again.
	 */
	irq_clr_pending(status);

	/*
	 * During exception executes, global IRQ is disabled as default
	 * option. It should be enabled again for supporting ISR nesting
	 * or preemption.
	 */
#ifdef CONFIG_SUPPORT_NESTED_IRQ
	core_irq_enable();
#endif
	dispatch_isr(status);
}
#endif

/************************************ General *********************************/
void do_illegal_inst(void)
{
	serial_puts("do_illegal_inst\n");
}

void do_lsu(void)
{
	serial_puts("do_lsu\n");

	// reset CPU ...
	sysctrl_soft_rst1_en(SYSCTRL_MASK_RST1_RISCV);
	while(1);
}

/*
 * board_init - to init system hardware such as pin-mux, uart, system clock and IRQs.
 * It is executed before main().
 * @param:	flag - a simple parammeter for future use.
 */

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
	sysctrl_soft_rst1_en(SYSCTRL_MASK_RST1_TIMER0
				| SYSCTRL_MASK_RST1_TIMER1
				| SYSCTRL_MASK_RST1_TIMER2);
	nop();
	nop();
	nop();
	nop();
	sysctrl_soft_rst1_dis(SYSCTRL_MASK_RST1_TIMER0
				| SYSCTRL_MASK_RST1_TIMER1
				| SYSCTRL_MASK_RST1_TIMER2);
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
