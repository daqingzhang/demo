#include <util.h>
#include <register.h>

void delay_1us(void)//it depend on core's clock
{
	int i;
	for(i = 8000;i != 0;i--)
		nop();
}

void delay(int us)
{
	while(us--)
		delay_1us();
}

void delay_n(int us)
{
	while(us--)
		delay_1us();
}

void delay_calib(int us)
{
	while(us--)
		delay_1us();
}
