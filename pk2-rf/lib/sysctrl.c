#include <sysctrl.h>

#define CONFIG_SYSCLK_26MHZ	26000000
#define CONFIG_SYSCLK_156MHZ	156000000

void sysctrl_set_system_clock(int clk)
{
	switch(clk) {
	case CONFIG_SYSCLK_156MHZ:
		HWP_SYSCTRL->cfg_clk_sys &= ~0x01;
		break;
	case CONFIG_SYSCLK_26MHZ:
	default:
		HWP_SYSCTRL->cfg_clk_sys |= 0x01;
		break;
	}
}

int sysctrl_get_system_clock(void)
{
	u32 reg = HWP_SYSCTRL->cfg_clk_sys & 0x1;

	if(reg)
		return CONFIG_SYSCLK_26MHZ;
	else
		return CONFIG_SYSCLK_156MHZ;
}

void sysctrl_reset_module(enum SYSCTRL_RST_MDL mdl)
{
	switch(mdl) {
	case SYS_RST_CPU:
		HWP_SYSCTRL->cfg_rst1 &= ~SYSCTRL_CFG_RST1_RISCV;
		break;
	default:
		break;
	}
}
