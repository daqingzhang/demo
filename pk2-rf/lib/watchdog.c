#include <watchdog.h>

static void wdog_lock(int locked)
{
	if(!locked)
		HWP_WDOG->lock = WDOG_LOCK_VALUE;
	else
		HWP_WDOG->lock = 1;
}

void wdog_init(u32 cnt)
{
	wdog_lock(0);
	HWP_WDOG->ctrl = 0;
	HWP_WDOG->intclr = WDOG_INTCLR_VALUE;
	HWP_WDOG->load = cnt;
	wdog_lock(1);
}

void wdog_enable(int enabled)
{
	wdog_lock(0);
	if(enabled)
		HWP_WDOG->ctrl |= WDOG_CTRL_RESEN;
	else
		HWP_WDOG->ctrl &= ~WDOG_CTRL_RESEN;
	wdog_lock(1);
}

void wdog_feed(u32 cnt)
{
	wdog_lock(0);
	HWP_WDOG->load = cnt;
	wdog_lock(1);
}

#ifdef CONFIG_USE_WDOG_INTERRUPT
void wdog_it_enable(int enabled)
{
	wdog_lock(0);
	if(enabled)
		HWP_WDOG->ctrl |= WDOG_CTRL_INTEN;
	else
		HWP_WDOG->ctrl &= ~WDOG_CTRL_INTEN;
	wdog_lock(1);
}

int wdog_it_get_status(void)
{
	u32 status;

	wdog_lock(0);
	status = HWP_WDOG->mis & WDOG_MIS_ITSTATUS;
	wdog_lock(1);
	return status;
}

void wdog_it_clr_status(void)
{
	wdog_lock(0);
	HWP_WDOG->intclr = WDOG_INTCLR_VALUE;
	wdog_lock(1);
}

void wdog_it_handler(void)
{
	wdog_it_clr_status();
	// TODO: Do work while interrupt occuring ...
}
#endif
