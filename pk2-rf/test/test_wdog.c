#include <watchdog.h>
#include <util.h>

static int wdog_reset_test(void)
{
	wdog_init(0x100000);
	wdog_enable(1);
	serial_puts("waiting for reset ...\n");
	deadloops();
	return 0;
}

#ifdef CONFIG_USE_WDOG_INTERRUPT
volatile static int wdog_callback_done = 0;
void wdog_callback(int irqs)
{
	wdog_clr_itstatus();
	serial_puts("wdog irq in\n");
	wdog_callback_done = 1;
}

static int wdog_interrupt_test(void)
{
	wdog_callback_done = 0;
	wdog_init(0x10000);
	wdog_it_enable(1);
	while(wdog_callback_done == 0);
	wdog_it_enable(0);
	return 0;

}
#endif

int wdog_test(void)
{
	int r = 0;

#ifdef CONFIG_USE_WDOG_INTERRUPT
	r += wdog_interrupt_test();
#endif
	r += wdog_reset_test();

	return r;
}
