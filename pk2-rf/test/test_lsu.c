#include <config.h>

int mem32_invalid_access(void)
{
	unsigned int t = 0;

	t += *(unsigned int *)(CONFIG_SRAM1_BASE - 4);
	t += *(unsigned int *)(CONFIG_SRAM1_BASE + CONFIG_SRAM1_SIZE + 4);

	return t;
}
