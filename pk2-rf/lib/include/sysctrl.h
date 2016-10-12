#ifndef __REG_SYSCTRL_H__
#define __REG_SYSCTRL_H__

#include <reg_sysctrl.h>

enum SYSCTRL_RST_MDL
{
	SYS_RST_DFE = 0,
	SYS_RST_BUS,
	SYS_RST_CGU,
	SYS_RST_DFE_PWD,
	SYS_RST_DFE_RISCV,
	SYS_RST_JTAG,
	SYS_RST_AHB_APB,
	SYS_RST_RAM,
	SYS_RST_TIMER0,
	SYS_RST_TIMER1,
	SYS_RST_TIMER2,
	SYS_RST_WDOG,
};

void sysctrl_set_system_clock(int clk);
int  sysctrl_get_system_clock(void);

void sysctrl_reset_module(enum SYSCTRL_RST_MDL mdl);

#endif
