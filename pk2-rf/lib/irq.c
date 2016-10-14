#include <irq.h>

void irq_enable(u32 mask)
{
	HWP_IRQ->enable |= mask;
}

void irq_disable(u32 mask)
{
	HWP_IRQ->enable &= ~mask;
}

u32 irq_get_enable_status(void)
{
	return HWP_IRQ->enable;
}

void irq_set_pending(u32 mask)
{
	HWP_IRQ->setpending = mask;
}

void irq_clr_pending(u32 mask)
{
	HWP_IRQ->clrpending = mask;
}

u32 irq_get_status(u32 mask)
{
	return (HWP_IRQ->pending & mask);
}
