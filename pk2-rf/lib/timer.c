#include <timer.h>
#include <sysctrl.h>

void timer_init(hwp_timer_t *hwp_timer, u32 cnt)
{
	hwp_timer->reload = cnt;
}

void timer_enable(hwp_timer_t *hwp_timer, int enabled)
{
	if(enabled) {
		hwp_timer->value = 0;
		hwp_timer->ctrl |= (TIMER_ENABLE | TIMER_IT_ENABLE);
	} else {
		hwp_timer->ctrl &= ~(TIMER_ENABLE | TIMER_IT_ENABLE);
	}
}

u32 timer_get_curval(hwp_timer_t *hwp_timer)
{
	return hwp_timer->value;
}

void timer_clr_itstatus(hwp_timer_t *hwp_timer)
{
	hwp_timer->itstatus = MASK_TIMER_ITSTATUS;
}

u32 timer_get_itstatus(hwp_timer_t *hwp_timer)
{
	return (hwp_timer->itstatus & MASK_TIMER_ITSTATUS);
}

void timer_dly_us(hwp_timer_t *hwp_timer, int us)
{
	u32 cnt,clk,status;

	clk = sysctrl_get_system_clock();
	cnt = (clk / 1000000) * us;

	hwp_timer->ctrl &= ~(TIMER_ENABLE | TIMER_IT_ENABLE);
	hwp_timer->value = 0;
	hwp_timer->reload = cnt;
	hwp_timer->itstatus = MASK_TIMER_ITSTATUS;
	hwp_timer->ctrl = (TIMER_ENABLE | TIMER_IT_ENABLE);
	do {
		status = hwp_timer->itstatus & MASK_TIMER_ITSTATUS;
	}while(!status);

	hwp_timer->itstatus = MASK_TIMER_ITSTATUS;
	hwp_timer->ctrl &= ~(TIMER_ENABLE | TIMER_IT_ENABLE);
}

void udelay(unsigned int us)
{
	timer_dly_us(HWP_TIMER2,us);
}

void mdelay(unsigned int ms)
{
	timer_dly_us(HWP_TIMER2,ms * 1000);
}
