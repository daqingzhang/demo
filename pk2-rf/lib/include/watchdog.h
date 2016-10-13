#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__
#include <regs/reg_watchdog.h>

void wdog_init(u32 cnt);
void wdog_enable(int enabled);
void wdog_feed(u32 cnt);
#ifdef CONFIG_USE_WDOG_INTERRUPT
void wdog_it_enable(int enabled);
int  wdog_it_get_status(void);
void wdog_it_clr_status(void);
void wdog_it_handler(void);
#endif

#endif
