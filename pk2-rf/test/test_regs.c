#include <util.h>
#include <serial.h>
#include <sysctrl.h>
#include <watchdog.h>

extern int rtc_reg_test(void);
extern int dfe_reg_test(void);
extern int cfg_reg_test(void);

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
	int r = 0;

	// TIMER 0
	serial_puts("TIM0 reg:\n");
	periph_reset(SYSCTRL_MASK_RST1_TIMER0);
	dump_reg((void *)HWP_TIMER0,4);

	// TIMER 1
	serial_puts("TIM1 reg:\n");
	periph_reset(SYSCTRL_MASK_RST1_TIMER1);
	dump_reg((void *)HWP_TIMER1,4);

	// TIMER 2
	serial_puts("TIM2 reg:\n");
	periph_reset(SYSCTRL_MASK_RST1_TIMER2);
	dump_reg((void *)HWP_TIMER2,4);

	// WDOG 2
	serial_puts("WDOG reg:\n");
	periph_reset(SYSCTRL_MASK_RST1_WDOG);
	watchdog_lock(0);
	dump_reg((void *)HWP_WDOG,6);
	watchdog_lock(1);

	// UART
	serial_puts("UART reg:\n");
	dump_reg((void *)HWP_UART,5);

	// SYSCTRL & GPIO
	serial_puts("SYSCTRL & GPIO reg:\n");
	dump_reg((void *)HWP_SYSCTRL,7);


	//RTC register
	serial_puts("RTC reg:\n");
	r += rtc_reg_test();

	//DFE register
	serial_puts("DFE reg:\n");
	r += dfe_reg_test();

	// CFG register
	serial_puts("DFE reg:\n");
	r += cfg_reg_test();

	if(r) {
		serial_puts("dump_all_regs, error code ");
		print_u32(r);
		serial_puts("\n");
	}
	serial_puts("dump_all_regs, test success !\n");
	return r;
}
