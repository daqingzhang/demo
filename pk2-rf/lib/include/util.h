// See LICENSE for license details.
#ifndef __UTIL_H
#define __UTIL_H

#include <stdint.h>

#define static_assert(cond) switch(0) { case 0: case !!(long)(cond): ; }

#ifdef __riscv
#include <encoding.h>
#endif

#include <irq.h>
#include <serial.h>
#include <timer.h>

#define nop() asm volatile ("nop")

void mdelay(unsigned int ms);
void udelay(unsigned int us);
void deadloops(void);

#endif //__UTIL_H
