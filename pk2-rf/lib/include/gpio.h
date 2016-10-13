#ifndef __GPIO_H__
#define __GPIO_H__
#include <regs/reg_sysctrl.h>

/*
 * gpio pins
 */
#define GPIO_PIN0	(1 << 0)
#define GPIO_PIN1	(1 << 1)
#define GPIO_PIN2	(1 << 2)
#define GPIO_PIN3	(1 << 3)
#define GPIO_PIN4	(1 << 4)
#define GPIO_PIN5	(1 << 5)
#define GPIO_PIN6	(1 << 6)
#define GPIO_PIN7	(1 << 7)
#define GPIO_PIN8	(1 << 8)
#define GPIO_PIN9	(1 << 9)
#define GPIO_PIN10	(1 << 10)
#define GPIO_PIN11	(1 << 11)
#define GPIO_PIN12	(1 << 12)
#define GPIO_PIN13	(1 << 13)
#define GPIO_PIN14	(1 << 14)
#define GPIO_PIN15	(1 << 15)

void gpio_set_direction(u32 pins, int input);
void gpio_set_value(u32 pins, int high);
int  gpio_get_value(u32 pins);

#endif
