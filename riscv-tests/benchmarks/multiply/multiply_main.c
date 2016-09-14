// See LICENSE for license details.

// *************************************************************************
// multiply filter bencmark
// -------------------------------------------------------------------------
//
#include "util.h"

int mem32_simple_rw(void);
int data32_sum(int max);
int hex2asc(unsigned int hex, char *asc);
unsigned int asc2hex(const char *asc);

void do_ecall_test(void);

#define STR_MSTATUS_ADDR 0x00011FF0
unsigned int read_csr_mstatus(void);
void write_csr_mstatus(unsigned int status);

char gdst[100] = {0};

int main( int argc, char* argv[] )
{
	int r = 0,err = 0;
	unsigned int hex = 0x1234ABCD;
	unsigned int status;
	char dst[80] = {0};

	r = mem32_simple_rw();
	if(r != 0)
		err |= 0x1;

	r = data32_sum(100);
	if(r != 5050)
		err |= 0x2;

	hex2asc(hex,dst);
	r = asc2hex(dst);
	if(r != hex)
		err |= 0x4;

	call_exit(err);

	/*
	 * This is used to test ecall inst
	 * after do_ecall_test return, we do mem copy test again
	 * to sure that CPU runs okay.
	 */
	do_ecall_test();
	status = read_csr_mstatus();
	*(unsigned int *)STR_MSTATUS_ADDR = status;

	r = mem32_simple_rw();
	if(r != 0)
		err |= 0x8;

	hex2asc(hex,gdst);
	r = asc2hex(gdst);
	if(r != hex)
		err |= 0x10;

	call_exit(err);

	return 0;
}
