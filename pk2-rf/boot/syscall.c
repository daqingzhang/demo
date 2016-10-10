// See LICENSE for license details.
#include <string.h>
#include <util.h>
#include <serial.h>
#include <interrupts.h>

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

static void dispatch_irqs(int irqs);
void do_interrupts(void)
{
	unsigned int irqs = irq_get_status(HWP_IRQ,0xFFFFFFFF);

	dispatch_irqs(irqs);

	writel(irqs,STR_ADDR_EXCP);
	irq_clr_pending(HWP_IRQ,irqs);
}

static void dispatch_irqs(int irqs)
{
	//TODO: to process interrupts
#ifdef CONFIG_IRQ_TEST
	irq_tester(irqs);//just for test
#endif
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
