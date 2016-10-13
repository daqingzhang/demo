#ifndef __TIMER_H__
#define __TIMER_H__
#include <regs/reg_timer.h>

#define TIMER_IT_STATUS	(1 << 0)

void timer_init(hwp_timer_t *hwp_timer, u32 cnt);
void timer_enable(hwp_timer_t *hwp_timer, int enabled);
void timer_clear_status(hwp_timer_t *hwp_timer,u32 status);
u32  timer_get_status(hwp_timer_t *hwp_timer,u32 status);
u32  timer_get_curval(hwp_timer_t *hwp_timer);

void timer_dly_us(hwp_timer_t *hwp_timer, int us);

#endif
