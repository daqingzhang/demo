#include <util.h>

/*
 * RISC-V Core Bugs !!!
 * When the RISC-V core executes the while(1) or for(;;) instructions
 * the interrupt will not be responsed or detected immediately.
 * To avoid this situation, we should use deadloops() to instead of
 * while(1) or for(;;) so that the IRQs can be normally responsed.
 */

void deadloops(void)
{
	while(1) {
		nop();
		nop();
	}
}

void dump_reg(void *hwp,int num)
{
	u32 addr = (u32)hwp,offs = 0;

	serial_puts("\ndump_reg: \n");
	while(num--) {
		print_u32(addr + offs);
		serial_puts(": ");
		print_u32(*(unsigned int *)(addr + offs));
		serial_puts("\n");
		offs += 4;
	}
}


