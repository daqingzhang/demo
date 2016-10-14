#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__
#include <regs/reg_irqs.h>

#define IRQ_EN_MASK_ALL  0xffffffff
#define IRQ_DIS_MASK_ALL 0xffffffff

void irq_enable(u32 mask);
void irq_disable(u32 mask);
void irq_set_pending(u32 mask);
void irq_clr_pending(u32 mask);
u32  irq_get_status(u32 mask);
u32  irq_get_enable_status(void);

void core_irq_enable(void);
void core_irq_disable(void);
void core_ecall(void);
void core_set_mstatus(unsigned int status);
u32  core_get_mstatus(void);

#endif
