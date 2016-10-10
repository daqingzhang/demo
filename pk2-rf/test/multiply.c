// See LICENSE for license details.

#ifdef CONFIG_MULTIPLY_TEST

int data32_sum(int max)
{
	int n = 0;
	for(;max != 0;max--)
		n += max;
	return n;
}

int hex2asc(unsigned int hex, char *asc)
{
	unsigned int t,i;

	for(i = 8;i != 0;i--) {
		t = hex & 0xF;
		if(t > 0x9)
			t += 0x37;
		else
			t += 0x30;
		asc[i-1] = (char)t;
		hex = hex >> 4;
	}
	return 0;
}

unsigned int asc2hex(const char *asc)
{
	unsigned int t,hex = 0;
	int i;

	for(i = 0;i < 8;i++) {
		t = asc[i] - 0x30;
		if(t > 9)
			t -= 0x7;
		t = t << (28 - (i << 2));
		hex |= t;
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
		if(x == 0)
			break;
	} 
	return result;
}
#endif
