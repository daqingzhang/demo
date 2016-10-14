#ifndef __SERIAL_H__
#define __SERIAL_H__
#include <regs/reg_uart.h>

void serial_init(void);
int  serial_tstc(void);
char serial_getc(void);
void serial_putc(char c);
void serial_set_baud(int baud);
void serial_puts(const char *s);
int  serial_gets(unsigned char *pstr);
int  putchar(int ch);
void print_u32(u32 data);

#endif
