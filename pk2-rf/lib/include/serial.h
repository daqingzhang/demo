#ifndef __SERIAL_H__
#define __SERIAL_H__
#include <reg_uart.h>

#define SERIAL_DEFAULT_BAUD 	115200

void serial_init(void);
int  serial_tstc(void);
char serial_getc(void);
void serial_putc(char c);
void serial_set_baud(int baud);
void serial_puts(const char *s);
int  serial_gets(unsigned char *pstr);
int  putchar(char ch);
void print_u32(u32 data);

#endif
