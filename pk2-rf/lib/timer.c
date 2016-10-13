#include <timer.h>
#include <sysctrl.h>

void timer_init(hwp_timer_t *hwp_timer, u32 cnt)
{
	hwp_timer->reload = cnt;
}

void timer_enable(hwp_timer_t *hwp_timer, int enabled)
{
	if(enabled)
		hwp_timer->ctrl |= TIMER_ENABLE;
	else
		hwp_timer->ctrl &= ~TIMER_ENABLE;
}

u32 timer_get_curval(hwp_timer_t *hwp_timer)
{
	return hwp_timer->value;
}

void timer_clear_status(hwp_timer_t *hwp_timer,u32 status)
{
	hwp_timer->itstatus = status;
}

u32 timer_get_status(hwp_timer_t *hwp_timer,u32 status)
{
	return (hwp_timer->itstatus & status);
}

void timer_dly_us(hwp_timer_t *hwp_timer, int us)
{
	u32 cnt,clk,status;

	clk = sysctrl_get_system_clock();
	cnt = (clk / 1000000) * us;

	hwp_timer->ctrl &= ~TIMER_ENABLE;
	hwp_timer->reload = cnt;
	hwp_timer->itstatus = MASK_TIMER_IT_STATUS;
	hwp_timer->ctrl |= TIMER_ENABLE;
	do {
		status = hwp_timer->itstatus & MASK_TIMER_IT_STATUS;
	}while(!status);

	hwp_timer->itstatus = MASK_TIMER_IT_STATUS;
	hwp_timer->ctrl &= ~TIMER_ENABLE;
}
