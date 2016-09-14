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

