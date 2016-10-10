#ifndef __REG_IRQ_H__
#define __REG_IRQ_H__
#include <config.h>

typedef volatile struct {
	__REG32_RW_ enable;
	__REG32_R__ pending;
	__REG32_RW1 setpending;
	__REG32_RW1 clrpending;
}hwp_irq_t;

#define HWP_IRQ	((hwp_irq_t *)(RDA_IRQ_BASE))

#endif
