#include <irq.h>

void irq_enable(hwp_irq_t *irqs, u32 mask)
{
	irqs->enable |= mask;
}

void irq_disable(hwp_irq_t *irqs, u32 mask)
{
	irqs->enable &= ~mask;
}

u32 irq_get_enable_status(hwp_irq_t *irqs)
{
	return irqs->enable;
}

void irq_set_pending(hwp_irq_t *irqs, u32 mask)
{
	irqs->setpending = mask;
}

void irq_clr_pending(hwp_irq_t *irqs, u32 mask)
{
	irqs->clrpending = mask;
}

u32 irq_get_status(hwp_irq_t *irqs, u32 mask)
{
	return (irqs->pending & mask);
}
