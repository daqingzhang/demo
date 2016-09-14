// See LICENSE for license details.

// *************************************************************************
// multiply function (c version)
// -------------------------------------------------------------------------

#define MEM32_ADDR 0x00010000

int mem32_simple_rw(void)
{
	int i;
	unsigned int addr,rdata,t;
	int err = 0;

	*(unsigned int *)MEM32_ADDR = 0x11223344;
	for(i = 0;i < 10;i++) {
		addr = MEM32_ADDR + (i << 2);
		*(unsigned int *)addr = (i << 2) + 1;
	}
	rdata = *(unsigned int *)MEM32_ADDR;
	for(i = 0;i < 10;i++) {
		t = (i << 2) + 1;
		addr = MEM32_ADDR + (i << 2);
		rdata = *(unsigned int *)addr;
		if(rdata != t)
			err++;
	}
	return err;
}

int data32_sum(int max)
{
	int n = 0;
	for(;max != 0;max--)
		n += max;
	return n;
}

int hex2asc(unsigned int hex, char *asc)
{
	unsigned int t,i = 8;

	for(i = 0;i < 8;i++) {
		t = hex & 0xf;
		if(t > 0x9)
			t += 0x37;
		else
			t += 0x30;
		asc[7 - i] = (char)t;
		hex = hex >> 4;
	}
	return 0;
}

unsigned int asc2hex(const char *asc)
{
	unsigned int t,hex = 0;
	int i;

	for(i = 0;i < 8;i++) {
		t = asc[7-i] - 0x30;
		if(t > 9)
			t -= 0x7;
		hex = hex | (t << (i << 2));
	}
	return hex;
}

int multiply( int x, int y )
{
	int i;
	int result = 0;

	for (i = 0; i < 32; i++) {
		if ((x & 0x1) == 1)
			result = result + y;
		x = x >> 1;
		y = y << 1;
	} 
	return result;
}
