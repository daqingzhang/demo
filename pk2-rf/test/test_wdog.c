#include <watchdog.h>
#include <util.h>
#include <serial.h>

void wdog_lock(int locked);
static void dump_wdog_reg(void)
{
	serial_puts("dump reg\n");
	wdog_lock(0);
	print_u32(HWP_WDOG->load);
	serial_puts("\n");
	print_u32(HWP_WDOG->value);
	serial_puts("\n");
	print_u32(HWP_WDOG->ctrl);
	serial_puts("\n");
	print_u32(HWP_WDOG->intclr);
	serial_puts("\n");
	print_u32(HWP_WDOG->ris);
	serial_puts("\n");
	print_u32(HWP_WDOG->mis);
	serial_puts("\n");
	wdog_lock(1);
}
#ifdef CONFIG_USE_WDOG_INTERRUPT
volatile static int wdog_callback_done = 0;
void wdog_callback(unsigned int irqs)
{
	if(irqs & 0x80000000) {
		serial_puts("wdog irq in\n");
		wdog_callback_done = 1;
		wdog_clr_itstatus();
	}
}

static int wdog_interrupt_test(void)
{
	u32 irq_mask = 0x80000000;
	u32 tick = 0x70000;

	serial_puts("watchdog, interrupt test start !\n");
	irq_enable(irq_mask);
	wdog_it_enable(0);
	wdog_init(tick);
	wdog_clr_itstatus();
	wdog_callback_done = 0;
	wdog_it_enable(1);
	serial_puts("watchdog, waiting for flag ...\n");
	dump_wdog_reg();
	while(wdog_callback_done == 0) {
		if(wdog_get_itstatus()) {
			dump_wdog_reg();
			serial_puts("wdog,itstatus is set, but callback not be executived !\n");
			wdog_it_enable(0);
			irq_disable(irq_mask);
			return -1;
		}
	}
	wdog_it_enable(0);
	irq_disable(irq_mask);
	serial_puts("watchdog, interrupt test success !\n");
	return 0;
}
#endif

static int wdog_reset_test(void)
{
	u32 tick = 0x70000;

	serial_puts("watchdog, reset cpu ... \n");
	wdog_init(tick);
	wdog_enable(1);
	dump_wdog_reg();
	serial_puts("waiting for reset ...\n");
	deadloops();

	return 0;
}


int wdog_test(void)
{
	int r = 0;

#ifdef CONFIG_USE_WDOG_INTERRUPT
	r += wdog_interrupt_test();
#endif
	r += wdog_reset_test();

	return r;
}
