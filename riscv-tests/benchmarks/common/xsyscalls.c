// See LICENSE for license details.
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include "util.h"

#define BOARD_INIT_ADDR  0x00011FF4
#define STR_ADDR_EXCP	 0x00011FF8
#define CALLEXIT_ADDR	 0x00011FFC
#define CALLEXIT_PASS	 0x900dc0de

static void save_excp_addr(unsigned int addr)
{
	writel(addr, STR_ADDR_EXCP);
	return;
}

void do_illegal_inst(void)
{
	save_excp_addr(0x84);
	return;
}

void do_lsu(void)
{
	save_excp_addr(0x8c);
	return;
}

void do_ecall(void)
{
	save_excp_addr(0x88);
	return;
}

void do_isr_uart(void)
{
	save_excp_addr(0x7c);
	return;
}

void board_init(int flag)
{
	writel(0x55aa,BOARD_INIT_ADDR);
	return;
}

void call_exit(int err)
{
	if(!err)
		err = CALLEXIT_PASS;
	writel(err,CALLEXIT_ADDR);
}

void* memcpy(void* dest, const void* src, size_t len)
{
	if ((((uintptr_t)dest | (uintptr_t)src | len) & (sizeof(uintptr_t)-1)) == 0) {
		const uintptr_t* s = src;
		uintptr_t *d = dest;
		while (d < (uintptr_t*)(dest + len))
			*d++ = *s++;
	} else {
		const char* s = src;
		char *d = dest;
		while (d < (char*)(dest + len))
			*d++ = *s++;
	}
	return dest;
}

void* memset(void* dest, int byte, size_t len)
{
	if ((((uintptr_t)dest | len) & (sizeof(uintptr_t)-1)) == 0) {
		uintptr_t word = byte & 0xFF;
		word |= word << 8;
		word |= word << 16;
		word |= word << 16 << 16;

		uintptr_t *d = dest;
		while (d < (uintptr_t*)(dest + len))
			*d++ = word;
	} else {
		char *d = dest;
		while (d < (char*)(dest + len))
			*d++ = byte;
	}
	return dest;
}

size_t strlen(const char *s)
{
	const char *p = s;
	while (*p)
		p++;
	return p - s;
}

size_t strnlen(const char *s, size_t n)
{
	const char *p = s;
	while (n-- && *p)
		p++;
	return p - s;
}

int strcmp(const char* s1, const char* s2)
{
	unsigned char c1, c2;

	do {
		c1 = *s1++;
		c2 = *s2++;
	} while (c1 != 0 && c1 == c2);

	return c1 - c2;
}

char* strcpy(char* dest, const char* src)
{
	char* d = dest;
	while ((*d++ = *src++))
		;
	return dest;
}

long atol(const char* str)
{
	long res = 0;
	int sign = 0;

	while (*str == ' ')
		str++;

	if (*str == '-' || *str == '+') {
		sign = *str == '-';
		str++;
	}

	while (*str) {
		res *= 10;
		res += *str++ - '0';
	}

	return sign ? -res : res;
}
