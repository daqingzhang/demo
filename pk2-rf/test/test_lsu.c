#include <config.h>

int mem32_invalid_access(void)
{
	unsigned int t = 0;
	unsigned int addr;

	//enable debug err response
	sysctrl_hwerr_response(1);
	addr = 0x50000;
	t += *(unsigned int *)addr;
	addr = 0x80000;
	t += *(unsigned int *)addr;

	return t;
}
