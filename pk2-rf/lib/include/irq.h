#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__
#include <reg_irqs.h>

#define IRQ_EN_MASK_ALL  0xffffffff
#define IRQ_DIS_MASK_ALL 0xffffffff

void irq_enable(hwp_irq_t *irqs, u32 mask);
void irq_disable(hwp_irq_t *irqs, u32 mask);
void irq_set_pending(hwp_irq_t *irqs, u32 mask);
void irq_clr_pending(hwp_irq_t *irqs, u32 mask);
u32  irq_get_status(hwp_irq_t *irqs, u32 mask);
u32  irq_get_enable_status(hwp_irq_t *irqs);

void core_irq_enable(void);
void core_irq_disable(void);

#endif
