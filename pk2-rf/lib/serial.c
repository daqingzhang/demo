#include <util.h>
#include <serial.h>

#ifdef CONFIG_HAS_SERIAL
#if 0
void serial_init(void)
{
	
}

void serial_enable_rtscts(void)
{

}

int serial_getc(void)
{
	return 0;
}

int serial_tstc(void)
{
	return 0;
}

void serial_putc(char c)
{

}

void serial_puts(const char *s)
{
	while(*s)
		serial_putc(*s++);
}

int serial_gets(unsigned char *pstr)
{
	return 0;
}

int putchar(int ch)
{
	serial_putc(ch);
	return ch;
}

void print_u8(uint8_t data)
{

}

void print_u16(uint16_t data)
{

}

void print_u32(uint32_t data)
{

}

void printhex(uint32_t x)
{
	print_u32(x);
}

void printstr(const char *str)
{
	serial_puts(str);
}
#endif

#endif

void rprintf(const char *fmt, ...)
{
}

void printArray(const char *name, int n, const int arr[])
{
}

void setStats(int enable)
{
}

int verify(int n, const int* test, const int *verify)
{
	return 0;
}
