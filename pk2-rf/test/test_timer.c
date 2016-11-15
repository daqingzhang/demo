#include <timer.h>
#include <sysctrl.h>
#include <irq.h>
#include <serial.h>

volatile static int timer_callback_done = 0;
extern volatile int g_irq_test;

void timer_callback(unsigned int irqs)
{
	hwp_timer_t *timer = NULL;
	//serial_puts("timer_callback, >>> ");
	//print_u32(irqs);
	//serial_puts("\n");

	if(irqs & 0x10) {
		timer_callback_done = 0x10;
		timer = HWP_TIMER0;
	} else if(irqs & 0x20) {
		timer_callback_done = 0x20;
		timer = HWP_TIMER1;
	} else if(irqs & 0x40) {
		timer_callback_done = 0x40;
		timer = HWP_TIMER2;
	} else {
		timer_callback_done = 0x0;
	}
	if((timer_callback_done) && (timer != NULL)) {
		timer_clr_itstatus(timer);
		while(timer_get_itstatus(timer) != 0);
		timer_enable(timer,0);
	}
}

static int timer_irq_test(hwp_timer_t *hwp_timer)
{
	u32 cnt,clk,us,irqmask = 0;

	serial_puts("timer_irq_test, start\n");

	g_irq_test = 0;
	us = 20;
	clk = sysctrl_get_system_clock();
	cnt = (clk / 1000000) * us;

	if(hwp_timer == HWP_TIMER0)
		irqmask = 0x10;
	else if(hwp_timer == HWP_TIMER1)
		irqmask = 0x20;
	else if(hwp_timer == HWP_TIMER2)
		irqmask = 0x40;

	//disable IRQ
	irq_disable(irqmask);
	timer_enable(hwp_timer,0);
	timer_init(hwp_timer,cnt);
	timer_clr_itstatus(hwp_timer);

	//clear flag
	timer_callback_done = 0x0;

	//enable IRQ controller for TIMERx
	irq_enable(irqmask);

	serial_puts("timer wait ...\n");
	//enable TIMERx
	timer_enable(hwp_timer,1);
	//waiting until timer interrupts occur
	while(!timer_callback_done);

	//disable IRQ
	irq_disable(irqmask);
	timer_enable(hwp_timer,0);
	serial_puts("timer wait done\n");

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

static int timer_dly_sec(hwp_timer_t *tim, int sec)
{
	while(sec--) {
		timer_dly_us(tim, 1000000);
	}
	return 0;
}

int timer_test(void)
{
	int r = 0;

	serial_puts("timer_test, test start\n");
	// set system clock
	//sysctrl_set_system_clock(CONFIG_SYSCLK_VALUE);

	timer_dly_us(HWP_TIMER0,50);
	serial_puts("TIM0 dly 50 us done !\n");

	timer_dly_us(HWP_TIMER1,30);
	serial_puts("TIM1 dly 30 us done !\n");

	timer_dly_us(HWP_TIMER2,10);
	serial_puts("TIM2 dly 10 us done !\n");

	if(timer_delay_demo(HWP_TIMER0,260))
		r |= 0x01;
	serial_puts("TIM0 dly 260 ticks done !\n");

	if(timer_delay_demo(HWP_TIMER1,520))
		r |= 0x02;
	serial_puts("TIM1 dly 520 ticks done !\n");

	if(timer_delay_demo(HWP_TIMER2,780))
		r |= 0x03;
	serial_puts("TIM2 dly 780 ticks done !\n");

	serial_puts("TIM0 dly 1 s ...\n");
	timer_dly_sec(HWP_TIMER0,1);
	serial_puts("done !\n");

	serial_puts("TIM0 dly 3 s ...\n");
	timer_dly_sec(HWP_TIMER0,3);
	serial_puts("done !\n");
#if 0
	serial_puts("TIM0 dly 5 s ...\n");
	timer_dly_sec(HWP_TIMER0,5);
	serial_puts("done !\n");
#endif
	serial_puts("TIM1 dly 1 s ...\n");
	timer_dly_sec(HWP_TIMER1,1);
	serial_puts("done !\n");

	serial_puts("TIM1 dly 3 s ...\n");
	timer_dly_sec(HWP_TIMER1,3);
	serial_puts("done !\n");
#if 0
	serial_puts("TIM1 dly 5 s ...\n");
	timer_dly_sec(HWP_TIMER1,5);
	serial_puts("done !\n");
#endif
	serial_puts("TIM2 dly 1 s ...\n");
	timer_dly_sec(HWP_TIMER2,1);
	serial_puts("done !\n");

	serial_puts("TIM2 dly 3 s ...\n");
	timer_dly_sec(HWP_TIMER2,3);
	serial_puts("done !\n");
#if 0
	serial_puts("TIM2 dly 5 s ...\n");
	timer_dly_sec(HWP_TIMER2,5);
	serial_puts("done !\n");
#endif
	if(timer_irq_test(HWP_TIMER0))
		r |= 0x08;
	serial_puts("TIM0 irq test done !\n");

	if(timer_irq_test(HWP_TIMER1))
		r |= 0x10;
	serial_puts("TIM1 irq test done !\n");

	if(timer_irq_test(HWP_TIMER2))
		r |= 0x20;
	serial_puts("TIM2 irq test done !\n");

	if(r) {
		serial_puts("timer_test, error code ");
		print_u32(r);
		serial_puts(" ###\n");
	}
	serial_puts("timer_test, test success !\n");
	return r;
}
