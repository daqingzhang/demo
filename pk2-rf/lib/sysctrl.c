#include <sysctrl.h>
#include <gpio.h>

/*
 * sysctrl_set_system_clock - set system clock frequency
 * @param:
 *	clk:	value will be set
 * @note:
 *	cfg_clk_sys:
 *		bit[11]	 1, clksrc is 26MHz,0,clksrc is 156MHz
 *		bit[10]	 1, update divider ration
 *		bit[8:5] divider numerator
 *		bit[3:0] divider denominator 
 */
void sysctrl_set_system_clock(int clk)
{
	switch(clk) {
	case CONFIG_SYSCLK_156MHZ:
		HWP_SYSCTRL->cfg_clk_sys  = 0x0000;
		HWP_SYSCTRL->cfg_clk_sys |= 0x0400;
		break;
	case CONFIG_SYSCLK_78MHZ:
		HWP_SYSCTRL->cfg_clk_sys  = 0x0022;
		HWP_SYSCTRL->cfg_clk_sys |= 0x0400;
		break;
	case CONFIG_SYSCLK_13MHZ:
		HWP_SYSCTRL->cfg_clk_sys  = 0x0822;
		HWP_SYSCTRL->cfg_clk_sys |= 0x0400;
		break;
	case CONFIG_SYSCLK_26MHZ:
	default:
		HWP_SYSCTRL->cfg_clk_sys  = 0x0800;
		HWP_SYSCTRL->cfg_clk_sys |= 0x0400;
		break;
	}
}

/*
 * sysctrl_get_system_clock - get system clock freuency
 * @return: frequency value
 */
int sysctrl_get_system_clock(void)
{
#if 0
	u32 reg = HWP_SYSCTRL->cfg_clk_sys;

	reg &= ~(1 << 10);
	switch(reg) {
	case 0x0800:
		return CONFIG_SYSCLK_156MHZ;
	case 0x0822:
		return CONFIG_SYSCLK_78MHZ;
	case 0x0022:
		return CONFIG_SYSCLK_13MHZ;
	case 0x0000;
		return CONFIG_SYSCLK_26MHZ;
	}
#else
	return CONFIG_SYSCLK_VALUE;
#endif
}

/*
 * sysctrl_soft_rst1_en - start to reset IPs modules
 * @param:
 *	mask: bit[n] = 1,reset IPs modules
 */
void sysctrl_soft_rst1_en(u32 mask)
{
	HWP_SYSCTRL->cfg_rst1 &= ~mask;
}

/*
 * sysctrl_soft_rst1_dis - stop to reset IPs modules
 * @param:
 *	mask: bit[n] = 1,stop to reset IPs modules
 */
void sysctrl_soft_rst1_dis(u32 mask)
{
	HWP_SYSCTRL->cfg_rst1 |= mask;
}

/*
 * sysctrl_soft_rst2_en - start to reset IPs modules
 * @param:
 *	mask: bit[n] = 1,reset IPs modules
 */
void sysctrl_soft_rst2_en(u32 mask)
{
	HWP_SYSCTRL->cfg_rst2 &= ~mask;
}

/*
 * sysctrl_soft_rst2_dis - stop to reset IPs modules
 * @param:
 *	mask: bit[n] = 1,stop to reset IPs modules
 */
void sysctrl_soft_rst2_dis(u32 mask)
{
	HWP_SYSCTRL->cfg_rst2 |= mask;
}

void sysctrl_set_debug(u32 value)
{
	HWP_SYSCTRL->debug = value;
}

void sysctrl_bypass_watchdog(int yes)
{
	if(yes)
		HWP_SYSCTRL->cfg_rst2 |= SYSCTRL_MASK_RST2_BYPASS_WDOG_RST;//bypass watch dog
	else
		HWP_SYSCTRL->cfg_rst2 &= ~SYSCTRL_MASK_RST2_BYPASS_WDOG_RST;//do not bypass watchdog
}

void sysctrl_hwerr_response(int yes)
{
	if(yes)
		HWP_SYSCTRL->debug |= SYSCTRL_MASK_DEBUG_HWERR_RESP;
	else
		HWP_SYSCTRL->debug &= ~SYSCTRL_MASK_DEBUG_HWERR_RESP;
}

/*
 * gpio_set_direction - set gpio pin direction
 * @param:
 *	pins:	the gpio pins that will be set
 *	input:	1 is input, 0 is output
 */
void gpio_set_direction(u32 pins, int input)
{
	if(input)
		HWP_SYSCTRL->gpio_oen |= pins;
	else
		HWP_SYSCTRL->gpio_oen &= ~pins;
}

/*
 * gpio_set_value - set gpio pin level
 * @param:
 *	pins:	the gpio pins that will be set
 *	high:	1 is high level, 0 is low level
 */
void gpio_set_value(u32 pins, int high)
{
	if(high)
		HWP_SYSCTRL->gpio_out |= pins;
	else
		HWP_SYSCTRL->gpio_out &= ~pins;
}

/*
 * gpio_get_value - get gpio pins' value
 * @return:	1 is high level, 0 is low level
 */
int gpio_get_value(u32 pins)
{
	u32 val = HWP_SYSCTRL->gpio_in & pins;

	return (val ? 1 : 0);
}
