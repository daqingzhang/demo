#include <watchdog.h>

void wdog_lock(int locked)
{
	if(!locked)
		HWP_WDOG->lock = WDOG_MASK_UNLOCK;
	else
		HWP_WDOG->lock = WDOG_MASK_LOCK;
}

void wdog_init(u32 cnt)
{
	wdog_lock(0);
	HWP_WDOG->ctrl = 0;
	HWP_WDOG->intclr = WDOG_MASK_INTCLR;
	HWP_WDOG->load = cnt;
	wdog_lock(1);
}

void wdog_enable(int enabled)
{
	wdog_lock(0);
	if(enabled)
		HWP_WDOG->ctrl |= (WDOG_MASK_RESEN | WDOG_MASK_ITEN);
	else
		HWP_WDOG->ctrl &= ~(WDOG_MASK_RESEN | WDOG_MASK_ITEN);;
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
		HWP_WDOG->ctrl |= WDOG_MASK_ITEN;
	else
		HWP_WDOG->ctrl &= ~WDOG_MASK_ITEN;
	wdog_lock(1);
}

int wdog_get_itstatus(void)
{
	u32 status;

	wdog_lock(0);
	status = HWP_WDOG->mis & WDOG_MASK_MIS_ITSTATUS;
	wdog_lock(1);
	return status;
}

void wdog_clr_itstatus(void)
{
	wdog_lock(0);
	HWP_WDOG->intclr = WDOG_MASK_INTCLR;
	wdog_lock(1);
}
#endif
