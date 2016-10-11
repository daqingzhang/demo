#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <riscv_iomap.h>

#define RV32_ARRAY_SIZE(tab) (sizeof((tab)) / sizeof((tab)[0]))

#define readl(addr) (*(volatile unsigned int *)(addr))
#define writel(val,addr) (*(volatile unsigned int *)(addr) = (val))

#ifndef NULL
#define NULL 0
#endif

typedef volatile unsigned int __REG32_R__;
typedef volatile unsigned int __REG32_RW_;
typedef volatile unsigned int __REG32_RW1;
typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;
typedef signed char	s8;
typedef signed short	s16;
typedef signed int	s32;

void call_exit(int err);

#endif /* __CONFIG_H__ */
