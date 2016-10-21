#include <config.h>
#include <util.h>
#include <sram_rw.h>

static unsigned char mem8_pattern[]=
{0x11,0x5A,0xFF,0xEE,0xDE,0x80,0x77,0xF8,0xAB};


static unsigned int mem32_pattern[]=
{
	0x11223344,
	0xAABBCCDD,
	0xEEEEFFFF,
	0xFFFF0000,
	0x5A5A5A5A,
	0x87654321,
	0x1A2B3C4D,
	0x5E6F7890,
};

static unsigned short mem16_pattern[]=
{
	0x1234,
	0xABCD,
	0xFF00,
	0x00FF,
	0xEFEF,
	0xA5A5,
	0xFFFF,
	0xDEAD,
};

void init_pattern(void)
{
	mem8_pattern[0] = 0x11;
	mem16_pattern[0] = 0x1234;
	mem32_pattern[0] = 0x11223344;
}

#if 0
/**
 * mem32_simple_rw - A simple test for reading/writing memory
 * return: zero for success.
 */
int mem32_simple_rw(void)
{
	int i;
	unsigned int addr,rdata,t;
	int err = 0;

	/* write data to memory */
	*(unsigned int *)MEM32_ADDR = 0x11223344;
	for(i = 0;i < MEM32_SIZE;i++) {
		addr = MEM32_ADDR + (i << 2);
		*(unsigned int *)addr = (i << 2) + 1;
	}
	/* read & compare data from memory */
	rdata = *(unsigned int *)MEM32_ADDR;
	for(i = 0;i < MEM32_SIZE;i++) {
		t = (i << 2) + 1;
		addr = MEM32_ADDR + (i << 2);
		rdata = *(unsigned int *)addr;
		if(rdata != t)
			err++;
	}
	return err;
}
#endif

/**
 * mem32_rw - memory 32bit reading/writing test.
 * return: zero for success.
 */
int mem32_rw(unsigned int addr, unsigned int size)
{
	unsigned int *dest,tmp[20];
	int n = size >> 2,i,j;

	/* write data to SRAM */
	i = n;
	dest = (unsigned int *)addr;
	while(i) {
		dest[0] = mem32_pattern[0];
		dest[1] = mem32_pattern[1];
		dest[2] = mem32_pattern[2];
		dest[3] = mem32_pattern[3];
		dest[4] = mem32_pattern[4];
		dest[5] = mem32_pattern[5];
		dest[6] = mem32_pattern[6];
		dest[7] = mem32_pattern[7];

		dest += 8;
		i -= 8;
	}
	/* read data from SRAM */
	i = n;
	dest = (unsigned int *)addr;
	while(i) {
		tmp[0] = dest[0];
		tmp[1] = dest[1];
		tmp[2] = dest[2];
		tmp[3] = dest[3];
		tmp[4] = dest[4];
		tmp[5] = dest[5];
		tmp[6] = dest[6];
		tmp[7] = dest[7];

		for(j = 0; j < 8;j++) {
			if(tmp[j] != mem32_pattern[j])
				return (int)(dest + j);	// return error data's address
		}
		dest += 8;
		i -= 8;
	}
	return 0;
}

/**
 * mem32_rw - memory 16bit reading/writing test.
 * return:  zero for success.
 */
int mem16_rw(unsigned int addr, unsigned int size)
{
	unsigned short *dest,tmp[20];
	int n = size >> 1,i,j;

	/* write data to SRAM */
	i = n;
	dest = (unsigned short *)addr;
	while(i) {
		dest[0] = mem16_pattern[0];
		dest[1] = mem16_pattern[1];
		dest[2] = mem16_pattern[2];
		dest[3] = mem16_pattern[3];
		dest[4] = mem16_pattern[4];
		dest[5] = mem16_pattern[5];
		dest[6] = mem16_pattern[6];
		dest[7] = mem16_pattern[7];

		dest += 8;
		i -= 8;
	}
	/* read data from SRAM */
	i = n;
	dest = (unsigned short *)addr;
	while(i) {
		tmp[0] = dest[0];
		tmp[1] = dest[1];
		tmp[2] = dest[2];
		tmp[3] = dest[3];
		tmp[4] = dest[4];
		tmp[5] = dest[5];
		tmp[6] = dest[6];
		tmp[7] = dest[7];

		for(j = 0; j < 8;j++) {
			if(tmp[j] != mem16_pattern[j])
				return (int)(dest + j);
		}
		dest += 8;
		i -= 8;
	}
	return 0;
}

/**
 * mem32_rw - memory 8bit reading/writing test.
 * return:  zero for success.
 */
int mem8_rw(unsigned int addr, unsigned int size)
{
	volatile unsigned char *dest;
	unsigned char tmp[20];
	int n = size,i,j;

	/* write data to SRAM */
	i = n;
	dest = (volatile unsigned char *)addr;
	while(i) {
		dest[0] = mem8_pattern[0];
		dest[1] = mem8_pattern[1];
		dest[2] = mem8_pattern[2];
		dest[3] = mem8_pattern[3];
		dest[4] = mem8_pattern[4];
		dest[5] = mem8_pattern[5];
		dest[6] = mem8_pattern[6];
		dest[7] = mem8_pattern[7];

		dest += 8;
		i -= 8;
	}
	/* read data from SRAM */
	i = n;
	dest = (volatile unsigned char *)addr;
	while(i) {
		tmp[0] = dest[0];
		tmp[1] = dest[1];
		tmp[2] = dest[2];
		tmp[3] = dest[3];
		tmp[4] = dest[4];
		tmp[5] = dest[5];
		tmp[6] = dest[6];
		tmp[7] = dest[7];

		for(j = 0; j < 8;j++) {
			if(tmp[j] != mem8_pattern[j]) {
				serial_puts("addr: ");print_u32((u32)dest);serial_puts("\n");
				serial_puts("j   : ");print_u32((u32)j);serial_puts("\n");
				serial_puts("temp: ");print_u32(tmp[j]);serial_puts("\n");
				serial_puts("patt: ");print_u32(mem8_pattern[j]);serial_puts("\n");
				return (int)(dest + j);
			}
		}
		dest += 8;
		i -= 8;
	}
	return 0;
}

static void *mem32_set(void *mem, unsigned int n, int nbytes)
{
	volatile unsigned int *dst;
	unsigned int mask = ~0x3;
	unsigned int count = nbytes >> 2;

	dst = (volatile unsigned int *)((unsigned int)mem & mask);

	if(count <= 8) {
		while(count--) {
			*(dst++) = n;
		}
		return mem;
	}

	while(count) {
		dst[0] = n;
		dst[1] = n;
		dst[2] = n;
		dst[3] = n;
		dst[4] = n;
		dst[5] = n;
		dst[6] = n;
		dst[7] = n;

		dst += 8;
		count -= 8;
	}
	return (void *)mem;
}

static void *mem32_bit_set(void *mem, int pos)
{
	volatile unsigned int *dst = (volatile unsigned int *)mem;
	unsigned int mask = *dst;

	mask |= (1 << pos);
	*dst = mask;
	return mem;
}

static void *mem32_bit_clr(void *mem, int pos)
{
	volatile unsigned int *dst = (volatile unsigned int *)mem;
	unsigned int mask = *dst;

	mask &= (~(1 << pos));
	*dst = mask;
	return mem;
}

static int mem32_bit_get(void *mem, int pos)
{
	volatile unsigned int *dst = (volatile unsigned int *)mem;
	unsigned int mask = 1 << pos;

	return (((*dst) & mask) ? 1 : 0);
}

#define MEM32_DATA_ERROR 1
#define MEM32_DATA_OK 	 0

static int mem32_bit_1_check(void *base, void *mem, int pos, int size)
{
	volatile unsigned int *addr = (volatile unsigned int *)base;
	volatile unsigned int *dst  = (volatile unsigned int *)mem;
	unsigned int mask = 1 << pos,i,tmp;

	if(mem32_bit_get(mem,pos) != 1)
		return -MEM32_DATA_ERROR;

	for(i = 0;i < (size >> 2);i++) {
		if(addr == dst)
			continue;
		tmp = *addr;
		/* if find any bit is not 0 ,error */
		if(tmp != 0)
			return (int)addr;
		addr++;
	}
	return 0;
}

static int mem32_bit_0_check(void *base, void *mem, int pos, int size)
{
	volatile unsigned int *addr = (volatile unsigned int *)base;
	volatile unsigned int *dst  = (volatile unsigned int *)mem;
	unsigned int mask = 1 << pos,i,tmp;

	if(mem32_bit_get(mem,pos) != 0)
		return -MEM32_DATA_ERROR;

	for(i = 0;i < (size >> 2);i++) {
		if(addr == dst)
			continue;
		tmp = *addr;
		/* if find any bit is not 1, then error */
		if(tmp != 0xFFFFFFFF)
			return (int)addr;
		addr++;
	}
	return 0;
}

/**
 * mem32_bit_1_rw - memory set bit 1 test
 * return:  zero for success.
 */
int mem32_bit_1_rw(unsigned int addr, unsigned int size)
{
	unsigned int *dst = (unsigned int *)addr;
	unsigned int n = size >> 2;
	int i,r;

	mem32_set((void *)addr,0,size);
	while(n) {
		for(i = 0;i < 32;i++) {
			mem32_bit_set(dst,i);
			r = mem32_bit_1_check((unsigned int *)addr,dst,i,size);
			mem32_bit_clr(dst,i);
			if(r)
				return r;
		}
		dst++;
		n--;
	}
	return 0;
}

/**
 * mem32_bit_1_rw - memory set bit 0 test
 * return:  zero for success.
 */
int mem32_bit_0_rw(unsigned int addr, unsigned int size)
{
	unsigned int *dst = (unsigned int *)addr;
	unsigned int n = size >> 2;
	int i,r;

	mem32_set((void *)addr,0xFFFFFFFF,size);
	while(n) {
		for(i = 0;i < 32;i++) {
			mem32_bit_clr(dst,i);
			r = mem32_bit_0_check((unsigned int *)addr,dst,i,size);
			mem32_bit_set(dst,i);
			if(r)
				return r;
		}
		dst++;
		n--;
	}
	return 0;
}

/**
 * mem32_bit_1_rw_first512B - memory set bit 1 test
 * First 512 bytes area of SRAM will be used.
 * return:  zero for success.
 */
int mem32_bit_1_rw_first512B(void)
{
	unsigned int *dst = (unsigned int *)DATA_SRAM_BASE;
	unsigned int size = 512;
	unsigned int n = size >> 2;
	int i,r;

	mem32_set((void *)DATA_SRAM_BASE,0,DATA_SRAM_SIZE);
	while(n) {
		for(i = 0;i < 32;i++) {
			mem32_bit_set(dst,i);
			r = mem32_bit_1_check((unsigned int *)DATA_SRAM_BASE,dst,i,size);
			mem32_bit_clr(dst,i);
			if(r)
				return r;
		}
		dst++;
		n--;
	}
	return 0;
}

/**
 * mem32_bit_1_rw_first512B - memory set bit 0 test
 * First 512 bytes area of SRAM will be used.
 * return:  zero for success.
 */
int mem32_bit_0_rw_first512B(void)
{
	unsigned int *dst = (unsigned int *)DATA_SRAM_BASE;
	unsigned int size = 512;
	unsigned int n = size >> 2;
	int i,r;

	mem32_set((void *)DATA_SRAM_BASE,0xFFFFFFFF,DATA_SRAM_SIZE);
	while(n) {
		for(i = 0;i < 32;i++) {
			mem32_bit_clr(dst,i);
			r = mem32_bit_0_check((unsigned int *)DATA_SRAM_BASE,dst,i,size);
			mem32_bit_set(dst,i);
			if(r)
				return r;
		}
		dst++;
		n--;
	}
	return 0;
}

/**
 * mem32_bit_1_rw_first512B - memory set bit 1 test
 * First 512 bytes area of SRAM will be used.
 * return:  zero for success.
 */
int mem32_bit_1_rw_last512B(void)
{
	unsigned int *dst = (unsigned int *)DATA_SRAM_BASE2;
	unsigned int size = 512;
	unsigned int n = size >> 2;
	int i,r;

	mem32_set((void *)DATA_SRAM_BASE,0,DATA_SRAM_SIZE);
	while(n) {
		for(i = 0;i < 32;i++) {
			mem32_bit_set(dst,i);
			r = mem32_bit_1_check((unsigned int *)DATA_SRAM_BASE,dst,i,size);
			mem32_bit_clr(dst,i);
			if(r)
				return r;
		}
		dst++;
		n--;
	}
	return 0;
}

/**
 * mem32_bit_1_rw_first512B - memory set bit 0 test
 * First 512 bytes area of SRAM will be used.
 * return:  zero for success.
 */
int mem32_bit_0_rw_last512B(void)
{
	unsigned int *dst = (unsigned int *)DATA_SRAM_BASE2;
	unsigned int size = 512;
	unsigned int n = size >> 2;
	int i,r;

	mem32_set((void *)DATA_SRAM_BASE,0xFFFFFFFF,DATA_SRAM_SIZE);
	while(n) {
		for(i = 0;i < 32;i++) {
			mem32_bit_clr(dst,i);
			r = mem32_bit_0_check((unsigned int *)DATA_SRAM_BASE,dst,i,size);
			mem32_bit_set(dst,i);
			if(r)
				return r;
		}
		dst++;
		n--;
	}
	return 0;
}
