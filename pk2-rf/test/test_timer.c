#include <timer.h>
#include <sysctrl.h>
#include <irq.h>
#include <serial.h>

volatile static int timer_callback_done = 0;

void timer_callback(unsigned int irqs)
{
	if(irqs & 0x10) {
		timer_callback_done = 0x10;
		timer_clr_itstatus(HWP_TIMER0);
	} else if(irqs & 0x20) {
		timer_callback_done = 0x20;
		timer_clr_itstatus(HWP_TIMER1);
	} else if(irqs & 0x40) {
		timer_callback_done = 0x40;
		timer_clr_itstatus(HWP_TIMER2);
	} else {
		timer_callback_done = 0x0;
	}
}

static int timer_irq_test(hwp_timer_t *hwp_timer)
{
	u32 cnt,clk,us,irqmask = 0;

	us = 20;
	clk = sysctrl_get_system_clock();
	cnt = (clk / 1000000) * us;

	if(hwp_timer == HWP_TIMER0)
		irqmask = 0x10;
	else if(hwp_timer == HWP_TIMER1)
		irqmask = 0x20;
	else if(hwp_timer == HWP_TIMER2)
		irqmask = 0x40;

	//disable global IRQ
	core_irq_disable();
	irq_disable(irqmask);
	timer_enable(hwp_timer,0);
	timer_init(hwp_timer,cnt);
	timer_clr_itstatus(hwp_timer);

	//clear flag
	timer_callback_done = 0x0;

	//enable global IRQ
	core_irq_enable();

	//enable IRQ controller for TIMERx
	irq_enable(irqmask);

	//enable TIMERx
	timer_enable(hwp_timer,1);

	//waiting until timer interrupts occur
	while(!timer_callback_done);

	//disable IRQ
	core_irq_disable();
	irq_disable(irqmask);
	timer_enable(hwp_timer,0);

	timer_callback_done |= readl(0x00012AA4);
	writel(timer_callback_done, 0x00012AA4);
	return 0;
}

static int timer_delay_demo(hwp_timer_t *hwp_timer,u32 cnt)
{
	timer_init(hwp_timer,cnt);
	timer_enable(hwp_timer,1);

	while(!timer_get_itstatus(hwp_timer));

	timer_clr_itstatus(hwp_timer);
	timer_enable(hwp_timer,0);

	return 0;
}

int timer_test(void)
{
	int r = 0;

	// set system clock
	sysctrl_set_system_clock(CONFIG_SYSCLK_VALUE);

	// timer0 delay 50 us
	timer_dly_us(HWP_TIMER0,50);
	// timer1 delay 30 us
	timer_dly_us(HWP_TIMER1,30);
	// timer2 delay 10 us
	timer_dly_us(HWP_TIMER2,10);

	// timer0 delay 260 ticks
	if(timer_delay_demo(HWP_TIMER0,260))
		r |= 0x01;
	// timer1 delay 520 ticks
	if(timer_delay_demo(HWP_TIMER1,520))
		r |= 0x02;
	// timer2 delay 780 ticks
	if(timer_delay_demo(HWP_TIMER2,780))
		r |= 0x03;

	if(timer_irq_test(HWP_TIMER0))
		r |= 0x08;
	if(timer_irq_test(HWP_TIMER1))
		r |= 0x10;
	if(timer_irq_test(HWP_TIMER2))
		r |= 0x20;
	if(r) {
		serial_puts("timer, error code ");
		print_u32(r);
		serial_puts("\n");
		writel(r, 0x00012AA0);
	}
	serial_puts("timer, test success !\n");
	return r;
}
