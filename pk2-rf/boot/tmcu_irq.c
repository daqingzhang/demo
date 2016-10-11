#include <irq.h>
#include <tmcu_irq.h>

/*
 * enable_interrupts - enable global IRQs
 */
void enable_interrupts(void)
{
	core_irq_enable();
}

/*
 * disable_interrupts - disable global IRQs
 */
void disable_interrupts(void)
{
	core_irq_disable();
}

/*
 * get_interrupt_mask - get enable register's
 * value from IRQ controller
 *
 * @return: enable register's value
 */
int get_interrupt_mask(void)  
{
	return irq_get_enable_status(HWP_IRQ);
}

/*
 * set_interrupt_mask - disable interrupts when
 * mask's bit is set 1.
 */
void set_interrupt_mask(unsigned int mask)
{
	irq_disable(HWP_IRQ,mask);
}

/*
 * set_interrupt_unmask - enable interrupts when
 * mask's bit is set 1.
 */
void set_interrupt_unmask(unsigned int mask)
{
	irq_enable(HWP_IRQ,mask);
}

/*
 * core_halt - disable global IRQs and jump into
 * a deadloop
 */
void core_halt(void)
{
	core_irq_disable();
	while(1)
	;
}

/*
 * software_interrupt - trigger a interrupts which
 * vector's address is specified by vector_addr.
 *
 * @description:  This is not a real swi instruction but
 * series operations. the RISCV core do not support real
 * software interrupt.
 */
void software_interrupt(int vector_addr)
{
	int id = vector_addr >> 2;
	u32 mask = 1 << id;

	irq_enable(HWP_IRQ,mask);
	irq_set_pending(HWP_IRQ,mask);
}
