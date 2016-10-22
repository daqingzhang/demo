#ifndef __REG_WATCHDOG_H__
#define __REG_WATCHDOG_H__
#include <config.h>

typedef volatile struct {
	__REG32_RW_	load;		//0x000
	__REG32_R__	value;		//0x004
	__REG32_RW_	ctrl;		//0x008
	__REG32_W__	intclr;		//0x00C
	__REG32_R__	ris;		//0x010
	__REG32_R__	mis;		//0x014
	__REG32_R__	rsv1[762]; 	//0x018 ~ 0xBFC
	__REG32_RW_	lock;		//0xC00
	__REG32_R__	rsv2[191];	//0xC04 ~ 0xEFC
	__REG32_RW_	itcr;		//0xF00
	__REG32_W__	itop;		//0xF04
}hwp_wdog_t;

#define HWP_WDOG	((hwp_wdog_t *)(RDA_WDOG_BASE))

/* ctrl */
#define WDOG_MASK_RESEN		(1 << 1)
#define WDOG_MASK_ITEN		(1 << 0)

/* intclr */
#define WDOG_MASK_INTCLR	(1 << 0)

/* ris */
#define WDOG_MASK_RIS_ITSTATUS	(1 << 0)

/* mis */
#define WDOG_MASK_MIS_ITSTATUS	(1 << 0)

/* lock */
#define WDOT_MASK_LOCK		(1 << 0)
#define WDOG_MASK_UNLOCK	(0x1ACCE551)

#endif
