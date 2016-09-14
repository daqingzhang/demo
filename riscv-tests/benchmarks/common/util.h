// See LICENSE for license details.
#ifndef __UTIL_H
#define __UTIL_H

//--------------------------------------------------------------------------
// Macros

// Set HOST_DEBUG to 1 if you are going to compile this for a host
// machine (ie Athena/Linux) for debug purposes and set HOST_DEBUG
// to 0 if you are compiling with the smips-gcc toolchain.

#ifndef HOST_DEBUG
#define HOST_DEBUG 0
#endif

#if HOST_DEBUG
#include <stdio.h>
static void setStats(int enable) {}
#else
extern void setStats(int enable);
#endif

#include <stdint.h>

#define static_assert(cond) switch(0) { case 0: case !!(long)(cond): ; }

#ifdef __riscv
#include "encoding.h"
#endif

#define stringify_1(s) #s
#define stringify(s) stringify_1(s)
#define stats(code, iter) do { \
    unsigned long _c = -read_csr(mcycle), _i = -read_csr(minstret); \
    code; \
    _c += read_csr(mcycle), _i += read_csr(minstret); \
    if (cid == 0) \
      printf("\n%s: %ld cycles, %ld.%ld cycles/iter, %ld.%ld CPI\n", \
             stringify(code), _c, _c/iter, 10*_c/iter%10, _c/_i, 10*_c/_i%10); \
  } while(0)


#define RV32_ARRAY_SIZE(tab) (sizeof((tab)) / sizeof((tab)[0]))

#define readl(addr) (*(volatile unsigned int *)(addr))
#define writel(val,addr) (*(volatile unsigned int *)(addr) = (val))

void core_irq_enable(void);
void core_irq_disable(void);

void call_exit(int err);

#endif //__UTIL_H
