#ifndef __REG_SYSCTRL_H__
#define __REG_SYSCTRL_H__
#include <config.h>

typedef volatile struct {
	__REG32_RW_ cfg_clk_sys;//0x00
	__REG32_RW_ cfg_rst1;	//0x04
	__REG32_RW_ cfg_rst2;	//0x08
	__REG32_RW_ gpio_out;	//0x0C
	__REG32_RW_ gpio_oen;	//0x10
	__REG32_RW_ gpio_in;	//0x14
	__REG32_RW_ debug;	//0x18
}hwp_sysctrl_t;

#define HWP_SYSCTRL	((hwp_sysctrl_t *)(RDA_SYSCTL_BASE))

/* cfg_rst2 */
#define SYSCTRL_CFG_RST2_BYPASS_WDOG_RST		(1 << 15)

/* debug */
#define SYSCTRL_DEBUG_HWERR_RESP_UNMASK	(1 << 0)
#endif
