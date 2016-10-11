#include <irq.h>

struct rda_irq_tag {
	int flag;
	int id;
	int error;
	int count;

	u32 nested;
	u32 preempt;
	u32 irqs[3];
};

static struct rda_irq_tag irqtag;

enum IRQ_CASE_ID {
	IRQ_CASE_NULL,
	IRQ_CASE_SIMPLE,
	IRQ_CASE_NEST,
	IRQ_CASE_PREEMPT,
};

static int irq_case = IRQ_CASE_NULL;

static void sdelay(int cnt)
{
	for(;cnt != 0;cnt--);
}

void irq_tester(int irqs)
{
	u32 mask = 0;

	switch(irq_case) {
	default:
	case IRQ_CASE_NULL:
		break;
	case IRQ_CASE_SIMPLE:
		irqtag.flag = 1;
		mask = 1 << irqtag.id;
		if(mask != irqs)
			irqtag.error += 1;
		break;
	case IRQ_CASE_NEST:
		/*
		 * Interrupt nesting test !
		 */
		{
			irqtag.irqs[irqtag.count++] = irqs;
			switch(irqtag.count) {
			case 1:
				mask = 1 << irqtag.id;
				// enable interrupt and pend a new interrupt
				irq_enable(HWP_IRQ,mask);
				irq_set_pending(HWP_IRQ,mask);
				// enable global irq again in ISRs
				sdelay(1000);
				core_irq_enable();
				// wait until new interrupt is responed
				while(irqtag.count == 1);
				break;
			case 2:
				if(irqs < irqtag.irqs[0])
					irqtag.error = 1;
				mask = irqtag.irqs[0] & irqtag.irqs[1];
				if(mask)
					irqtag.nested = 1;
				else
					irqtag.nested = 0;
				irqtag.flag = 1;
				break;
			default:
				break;
			}
		}
		break;
	case IRQ_CASE_PREEMPT:
		/*
		 * Interrupt preemption test !
		 *
		 * We pend a new IRQ (we assume the id value is P1) while
		 * current interrupts (id value is P0) is being serviced.
		 * If current IRQ is preempted by the new one, P1 < P0.
		 * Otherwise, P1 > P0 and the preempion shouldn't occur.
		 *
		 */
		{
			u32 tick = 100;

			irqtag.irqs[irqtag.count++] = irqs;

			switch(irqtag.count) {
			case 1:
				// enable and pend a new interrupt
				mask = 1 << irqtag.id;
				irq_enable(HWP_IRQ,mask);
				irq_set_pending(HWP_IRQ,mask);
				//delay
				while(tick--) {
					sdelay(100);
				}
				// enable global ISR, now there are more than one IRQ is in pending state.
				core_irq_enable();
				break;
			case 2:
				if(irqs < irqtag.irqs[0])
					irqtag.error = 1;
				mask = irqtag.irqs[0] & irqtag.irqs[1];
				if(mask)
					irqtag.preempt = 1;
				else
					irqtag.preempt = 0;
				irqtag.flag = 1;
				break;
			default:
				break;
			}
		}
		break;
	}
}

int irq_simple_test(void)
{
	u32 mask = 0;
	hwp_irq_t *hwp_irq = HWP_IRQ;

	irq_case = IRQ_CASE_SIMPLE;

	irqtag.id = 0;
	irqtag.error = 0;

	while(1) {
		irqtag.flag = 0;
		mask = 1 << irqtag.id;

		irq_enable(hwp_irq,mask);
		irq_set_pending(hwp_irq,mask);

		while(irqtag.flag == 0) {
			sdelay(8000);
		}
		irqtag.id++;
		if(irqtag.id == 32)
			break;
	}
	irq_disable(hwp_irq,IRQ_DIS_MASK_ALL);

	return irqtag.error;
}

int irq_nesting_test(void)
{
	int err = 0;
	u32 mask = 0,id = 0;
	hwp_irq_t *hwp_irq = HWP_IRQ;

	irq_case = IRQ_CASE_NEST;

	irqtag.irqs[0] = 0;
	irqtag.irqs[1] = 0;
	irqtag.irqs[2] = 0;
	/*
	 * P0 = 2, P1 = 1, P1 < P0 ,nested
	 */
	irqtag.flag = 0;
	irqtag.count = 0;
	irqtag.nested = 0;
	irqtag.id = 1;
	id = 2;
	mask = 1 << id;
	irq_enable(hwp_irq,mask);
	irq_set_pending(hwp_irq,mask);
	while(irqtag.flag == 0);
	irq_disable(hwp_irq,IRQ_DIS_MASK_ALL);
	if(irqtag.nested == 0)
		err |= 0x01;
	if(irqtag.count != 2)
		err |= 0x02;
	if(err)
		writel(err, 0x00012DD8);
	return err;
}

int irq_preemption_test(void)
{
	int err = 0;
	u32 mask = 0,id = 0;
	hwp_irq_t *hwp_irq = HWP_IRQ;

	irq_case = IRQ_CASE_PREEMPT;

	irqtag.irqs[0] = 0;
	irqtag.irqs[1] = 0;
	irqtag.irqs[2] = 0;

	/*
	 * P0 = 2, P1 = 1, P1 < P0 ,preempted
	 */
	irqtag.flag = 0;
	irqtag.count = 0;
	irqtag.preempt = 0;
	irqtag.id = 1;
	id = 2;
	mask = 1 << id;
	irq_enable(hwp_irq,mask);
	irq_set_pending(hwp_irq,mask);
	while(irqtag.flag == 0);
	irq_disable(hwp_irq,IRQ_DIS_MASK_ALL);
	if(irqtag.preempt == 0)
		err |= 0x01;
	if(irqtag.count != 2)
		err |= 0x02;

	/*
	 * P0 = 1, P1 = 2, P1 < P0 ,no preempted
	 */
	irqtag.flag = 0;
	irqtag.count = 0;
	irqtag.preempt = 0;
	irqtag.id = 2;
	id = 1;
	mask = 1 << id;
	irq_enable(hwp_irq,mask);
	irq_set_pending(hwp_irq,mask);
	while(irqtag.flag == 0);
	irq_disable(hwp_irq,IRQ_DIS_MASK_ALL);
	if(irqtag.preempt != 0)
		err |= 0x04;
	if(irqtag.count != 2)
		err |= 0x08;

	if(err)
		writel(err, 0x00012DDC);
	return err;
}
