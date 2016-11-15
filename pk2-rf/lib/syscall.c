// See LICENSE for license details.
#include <string.h>
#include <util.h>
#include <sysctrl.h>
#include <gpio.h>

/**
 ************************************************************************
 * 			For TEST project !				*
 ************************************************************************
 */
#ifdef CONFIG_PROJ_TEST

volatile int g_irq_test = 0;
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

	if(!g_irq_test) {
#ifdef CONFIG_SERIAL_TEST
		serial_callback(status);
#endif
#ifdef CONFIG_TIMER_TEST
		timer_callback(status);
#endif
#ifdef CONFIG_WDOG_TEST
		wdog_callback(status);
#endif
	} else {
#ifdef CONFIG_IRQ_TEST
		irq_tester(status);//just for test
#endif
	}

	irq_clr_pending(status);
}

void do_ecall(void)
{
	u32 status = core_get_mstatus();
	serial_puts("ecall exception in, mstatus is: ");
	print_u32(status);
	serial_puts("\n");
#ifdef CONFIG_ECALL_TEST
	ecall_excp_done = 1;
#endif
}

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

/**
 ************************************************************************
 * 			For BOOT project !				*
 ************************************************************************
 */
#else

/*
 * do_illegal_inst - The illegal instruction exception
 * @desc: CPU will trap this exception when it execute a invalid instruction.
 */
void do_illegal_inst(void)
{
	serial_puts("do_illegal_inst\n");
	// TODO: add code here
}

/*
 * do_lsu - The load/store exception
 * @desc: CPU will trap this exception when it access a invalid memory address.
 */
void do_lsu(void)
{
	serial_puts("do_lsu\n");
	// TODO: add code here
}

/*
 * do_ecall - The ECALL exception
 * @desc: CPU will trap this exception when it execute ECALL instruction.
 */
void do_ecall(void)
{
	serial_puts("do_ecall\n");
	//TODO: add code here
}

extern void dispatch_isr(unsigned int state);
/*
 * do_interrupts - The interrupt service request function.
 */
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
	// TODO: add code here
	dispatch_isr(status);
}
#endif
