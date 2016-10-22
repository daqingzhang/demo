#ifndef __GPIO_H__
#define __GPIO_H__
#include <regs/reg_sysctrl.h>

/*
 * gpio pins
 */
#define GPIO_MASK_P0	(1 << 0)
#define GPIO_MASK_P1	(1 << 1)
#define GPIO_MASK_P2	(1 << 2)
#define GPIO_MASK_P3	(1 << 3)
#define GPIO_MASK_P4	(1 << 4)
#define GPIO_MASK_P5	(1 << 5)
#define GPIO_MASK_P6	(1 << 6)
#define GPIO_MASK_P7	(1 << 7)
#define GPIO_MASK_P8	(1 << 8)
#define GPIO_MASK_P9	(1 << 9)
#define GPIO_MASK_P10	(1 << 10)
#define GPIO_MASK_P11	(1 << 11)
#define GPIO_MASK_P12	(1 << 12)
#define GPIO_MASK_P13	(1 << 13)
#define GPIO_MASK_P14	(1 << 14)
#define GPIO_MASK_P15	(1 << 15)

void gpio_set_direction(u32 pins, int input);
void gpio_set_value(u32 pins, int high);
int  gpio_get_value(u32 pins);

#endif
