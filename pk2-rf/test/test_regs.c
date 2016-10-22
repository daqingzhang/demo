#include <util.h>
#include <serial.h>
#include <sysctrl.h>

static void sdly(int cnt)
{
	for(;cnt !=0;cnt--)
		nop();
}

int test_regs(void)
{
	// TIMER 0
	sysctrl_soft_rst1_en(SOFT_RST1_TIMER0)
	sdly(8000);
	sysctrl_soft_rst1_dis(SOFT_RST1_TIMER0)
	dump_reg(HWP_TIMER0,5);

	// TIMER 1
	sysctrl_soft_rst1_en(SOFT_RST1_TIMER1)
	sdly(8000);
	sysctrl_soft_rst1_dis(SOFT_RST1_TIMER1)
	dump_reg(HWP_TIMER1,5);

	// TIMER 2
	sysctrl_soft_rst1_en(SOFT_RST1_TIMER2)
	sdly(8000);
	sysctrl_soft_rst1_dis(SOFT_RST1_TIMER2)
	dump_reg(HWP_TIMER2,5);

	// WDOG 2
	sysctrl_soft_rst1_en(SOFT_RST1_WDOG)
	sdly(8000);
	sysctrl_soft_rst1_dis(SOFT_RST1_TIMER2)
	dump_reg(HWP_WDOG,5);
}
