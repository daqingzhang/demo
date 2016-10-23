#include <util.h>
#include <serial.h>
#include <sysctrl.h>

void serial_set_baud(int div)
{
	div = (div > 16) ? div : 16;

	HWP_UART->bauddiv = div;
}

void serial_init(void)
{
	u32 ctrl;
	u32 sysclk = sysctrl_get_system_clock();

	serial_set_baud(sysclk / CONFIG_SERIAL_BAUD);

	ctrl = UART_MASK_TX_EN | UART_MASK_RX_EN;
	HWP_UART->ctrl = ctrl;
}

int serial_tstc(void)
{
	while(!(HWP_UART->state & UART_MASK_RX_FUL))
		;
	return 1;
}

char serial_getc(void)
{
	while(!(HWP_UART->state & UART_MASK_RX_FUL))
		;
	return HWP_UART->data;
}

void serial_putc(char c)
{
	while(HWP_UART->state & UART_MASK_TX_FUL)
		;
	HWP_UART->data = (u8)c;
}

void serial_puts(const char *s)
{
	while(*s != '\0') {
		if(*s == '\n')
			serial_putc('\r');
		serial_putc(*(s++));
	}
}

int serial_gets(unsigned char *s)
{
	int cnt = 0;
	char ch;

	do {
		ch = serial_getc();
		s[cnt++] = ch;
	}while(ch != '\n');

	return cnt;
}

int putchar(int ch)
{
	serial_putc(ch);
	return ch;
}

void print_u32(u32 data)
{
	u32 i = 8,tmp,cnt;

	for(;i != 0;i--) {
		cnt = (i - 1) << 2;
		tmp = ((data >> cnt) & 0x0F) | 0x30;
		if(tmp > 0x39)
			tmp += 0x07;
		serial_putc((u8)tmp);
	}
}
