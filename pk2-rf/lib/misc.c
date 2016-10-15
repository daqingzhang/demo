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
