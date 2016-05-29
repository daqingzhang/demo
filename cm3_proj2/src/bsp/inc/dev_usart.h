#ifndef __DEV_USART_H__
#define __DEV_USART_H__

#include <stdint.h>

/*
 *	USART DEVICE VARIBLE DEFINITION
 ****************************************************************
 */

void serial_init(void);
char serial_getc(void);
void serial_putc(char ch);
int serial_tstc(void);
void serial_puts(const char *str);
int serial_gets(char *buf);
void rprintf(const char *fmt,...);
int serial_config(int baud,int data,char parity,int stop);

int serial_read(uint8_t *buf,int len);
int serial_write(uint8_t *data,int len);

void hex2asc(uint8_t *phex,char *pasc,int hexlen);
uint64_t asc2hex(uint8_t *pstr,int len);

#endif
