#ifndef __SERIAL_H__
#define __SERIAL_H__

void setStats(int enable);
int verify(int n, const int* test, const int *verify);

void serial_init(void);
void serial_enable_rtscts(void);
int  serial_tstc(void);
int  serial_getc(void);
void serial_putc(char c);
void serial_puts(const char *s);
int  serial_gets(unsigned char *pstr);
int  putchar(int ch);
void print_u8(uint8_t data);
void print_u16(uint16_t data);
void print_u32(uint32_t data);
void printhex(uint32_t x);
void printstr(const char *str);
void rprintf(const char *fmt, ...);
void printArray(const char *name, int n, const int arr[]);
#endif
