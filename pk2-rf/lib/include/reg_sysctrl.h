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

/* cfg_rst1 */
#define SYSCTRL_CFG_RST1_DFE		(1 << 0)
#define SYSCTRL_CFG_RST1_BUS		(1 << 1)
#define SYSCTRL_CFG_RST1_CGU		(1 << 2)
#define SYSCTRL_CFG_RST1_DFE_PWD	(1 << 3)
#define SYSCTRL_CFG_RST1_RISCV		(1 << 4)
#define SYSCTRL_CFG_RST1_JTAG		(1 << 5)
#define SYSCTRL_CFG_RST1_AHB_APB	(1 << 6)
#define SYSCTRL_CFG_RST1_RAM		(1 << 7)
#define SYSCTRL_CFG_RST1_TIME0_APB	(1 << 8)
#define SYSCTRL_CFG_RST1_TIME1_APB	(1 << 9)
#define SYSCTRL_CFG_RST1_TIME2_APB	(1 << 10)
#define SYSCTRL_CFG_RST1_WDOG		(1 << 11)
#define SYSCTRL_CFG_RST1_WDOG_APB	(1 << 12)
#define SYSCTRL_CFG_RST1_UART_APB	(1 << 13)
#define SYSCTRL_CFG_RST1_RXDP		(1 << 14)
#define SYSCTRL_CFG_RST1_TXDP		(1 << 15)

#endif
