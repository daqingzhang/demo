#include <util.h>
#include <serial.h>
#include <sysctrl.h>
#include <watchdog.h>

static void sdly(int cnt)
{
	for(;cnt !=0;cnt--)
		nop();
}

static void periph_reset(u32 id)
{
	sysctrl_soft_rst1_en(id);
	sdly(8000);
	sysctrl_soft_rst1_dis(id);
}

static void watchdog_lock(int locked)
{
	if(!locked)
		HWP_WDOG->lock = WDOG_MASK_UNLOCK;
	else
		HWP_WDOG->lock = WDOG_MASK_LOCK;
}

int dump_all_regs(void)
{
	// TIMER 0
	periph_reset(SYSCTRL_MASK_RST1_TIMER0);
	dump_reg((void *)HWP_TIMER0,4);

	// TIMER 1
	periph_reset(SYSCTRL_MASK_RST1_TIMER1);
	dump_reg((void *)HWP_TIMER1,4);

	// TIMER 2
	periph_reset(SYSCTRL_MASK_RST1_TIMER2);
	dump_reg((void *)HWP_TIMER2,4);

	// WDOG 2
	periph_reset(SYSCTRL_MASK_RST1_WDOG);
	watchdog_lock(0);
	dump_reg((void *)HWP_WDOG,6);
	watchdog_lock(1);

	// UART
	dump_reg((void *)HWP_UART,5);

	// SYSCTRL & GPIO
	dump_reg((void *)HWP_SYSCTRL,7);
	return 0;
}
