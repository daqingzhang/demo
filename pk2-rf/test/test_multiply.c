#include <serial.h>

static int data32_sum(int max)
{
	int n = 0;
	for(;max != 0;max--)
		n += max;
	return n;
}

static int hex2asc(unsigned int hex, char *asc)
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

static unsigned int asc2hex(const char *asc)
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

static int multiply( int x, int y )
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

int multi_test(void)
{
	int r = 0,err = 0;
	unsigned int hex = 0x1234ABCD;
	char dst[10] = {0};

	if(r != 0)
		err |= 0x1;

	r = data32_sum(100);
	if(r != 5050)
		err |= 0x2;

	hex2asc(hex,dst);
	r = asc2hex(dst);
	if(r != hex)
		err |= 0x4;

	if(multiply(10,8) != 80)
		err |= 0x8;

	if(multiply(10,80) != 800)
		err |= 0x10;

	if(multiply(10,800) != 8000)
		err |= 0x20;

	if(err) {
		serial_puts("multi, error code ");
		print_u32(err);
		serial_puts("\n");
		return err;
	}
	serial_puts("multi, test success !\n");
	return 0;
}
