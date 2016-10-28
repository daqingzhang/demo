#include <util.h>
#include <register.h>

void delay_1us(void)//it depend on core's clock
{
	udelay(1);
}

void delay(int us)
{
	udelay(us);
}

void delay_n(int us)
{
	udelay(us);
}

void delay_calib(int us)
{
	udelay(us);
}
