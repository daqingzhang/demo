#ifndef __REG_TIMER_H__
#define __REG_TIMER_H__
#include <config.h>

typedef volatile struct {
	__REG32_RW_	ctrl;
	__REG32_R__	value;
	__REG32_RW_	reload;
	__REG32_RW_	itstatus;
}hwp_timer_t;

#define HWP_TIMER0	((hwp_timer_t *)(RDA_TIMER0_BASE))
#define HWP_TIMER1	((hwp_timer_t *)(RDA_TIMER1_BASE))
#define HWP_TIMER2	((hwp_timer_t *)(RDA_TIMER2_BASE))

/* ctrl */
#define TIMER_IT_ENABLE		(1 << 3)
#define TIMER_EXTIN_AS_CLOCK	(1 << 2)
#define TIMER_EXTIN_AS_ENABLE	(1 << 1)
#define TIMER_ENABLE		(1 << 0)

/* itstatus */
#define MASK_TIMER_IT_STATUS	(1 << 0)

#endif
