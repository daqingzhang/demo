#ifndef __SYSCTRL_H__
#define __SYSCTRL_H__
#include <regs/reg_sysctrl.h>

/*
 * software reset1 modules
 */

#define SOFT_RST1_DFE		(1 << 0)
#define SOFT_RST1_BUS		(1 << 1)
#define SOFT_RST1_CGU		(1 << 2)
#define SOFT_RST1_DFE_PWD	(1 << 3)
#define SOFT_RST1_RISCV 	(1 << 4)
#define SOFT_RST1_JTAG		(1 << 5)
#define SOFT_RST1_AHB2APB	(1 << 6)
#define SOFT_RST1_RAM		(1 << 7)
#define SOFT_RST1_TIMER0	(1 << 8)
#define SOFT_RST1_TIMER1	(1 << 9)
#define SOFT_RST1_TIMER2	(1 << 10)
#define SOFT_RST1_WDOG		(1 << 11)
#define SOFT_RST1_WDOG_APB	(1 << 12)
#define SOFT_RST1_UART_APB	(1 << 13)
#define SOFT_RST1_RXDP		(1 << 14)
#define SOFT_RST1_TXDP		(1 << 15)

/*
 * software reset2 modules
 */

#define SOFT_RST2_RFSPI 	(1 << 0)
#define SOFT_RST2_RFAPB 	(1 << 1)

void sysctrl_set_system_clock(int clk);
int  sysctrl_get_system_clock(void);

void sysctrl_bypass_watchdog(int yes);
void sysctrl_hwerr_response(int yes);

void sysctrl_soft_rst1_en(u32 ips);
void sysctrl_soft_rst1_dis(u32 ips);
void sysctrl_soft_rst2_en(u32 ips);
void sysctrl_soft_rst2_dis(u32 ips);
void sysctrl_set_debug(u32 value);

#endif
