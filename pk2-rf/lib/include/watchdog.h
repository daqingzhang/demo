#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__
#include <regs/reg_watchdog.h>

void wdog_init(u32 cnt);
void wdog_enable(int enabled);
void wdog_feed(u32 cnt);
#ifdef CONFIG_USE_WDOG_INTERRUPT
void wdog_it_enable(int enabled);
int  wdog_get_itstatus(void);
void wdog_clr_itstatus(void);
void wdog_callback(void);
#endif

#endif
