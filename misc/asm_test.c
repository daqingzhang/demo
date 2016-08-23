#include <common.h>
#include <asm/string.h>

extern int num_add_one(int a);
extern int num_shift(int a);

extern int num_add(int a,int b); //a+b
extern int num_sub(int a,int b); //a-b
extern int num_mul(int a,int b); //a*b
extern int num_div(int a,int b); //a/b

/*
 * s(n) = 1 + 2 +...+ n
 */
extern int num_sum(int n);
/*
 * a(0) = x;
 * a(i+1) = a(i) + d;
 * s(n) = a(0) + a(1) + ... + a(n)
 */
extern int num_sn(int x,int d,int n);

extern void *memcpy_s(void *dst, const void *src,unsigned size);
extern void *memset_s(void *dst, int n,unsigned size);
extern int operate_bits1(int org, int val, int mask);

#define MEM_ADDR_DST 0x80000000
#define MEM_ADDR_SRC 0x84000000
#define BUF_LEN 32
unsigned char dst_buf[BUF_LEN] = {0};
unsigned char src_buf[BUF_LEN] = {0};

void *xmemdump(const void *dst, unsigned size);

void *xmemcpy1(void *dst, const void *res, unsigned int size);
void *xmemcpy2(void *dst, const void *res, unsigned int size);
void *xmemcpy3(void *dst, const void *res, unsigned int size);
void *xmemcpy4(void *dst, const void *res, unsigned int size);

void *xmemset1(void *s, int n, unsigned int size);
void *xmemset2(void *s, int n, unsigned int size);
void *xmemset3(void *s, int n, unsigned int size);
void *xmemset4(void *s, int n, unsigned int size);

int test_pfm_memset(void *addr, unsigned nMB);
int test_pfm_memcpy(void *to, const void *from,unsigned nMB);
int test_func_memset(void *addr, unsigned max_size);
int test_func_memcpy(void *to, void *from, unsigned max_size);

int asm_code_tests(int times)
{
	int r = 0,i = 0,len = 0;
	void *addr = 0;
	int val = 0;

	r = r;
	i = i;
	len = len;
	addr = addr;
	val = val;
#if 0
	val = operate_bits1(0xABC97FEF,0x0801,0x0FFF);
	printf("val = %x\n",val);
	val = operate_bits1(0xFFFFFFFF,0x0,0x0FFF);
	printf("val = %x\n",val);
	val = operate_bits1(0x0,0x0FFF,0x0FFF);
	printf("val = %x\n",val);
#endif

#if 1
	test_func_memset((void *)dst_buf,BUF_LEN);
//	test_func_memcpy((void *)dst_buf,(void *)src_buf,BUF_LEN);
#endif

#if 1
	test_pfm_memset((void *)MEM_ADDR_DST,100);
	test_pfm_memset((void *)MEM_ADDR_SRC,100);
#endif

#if 0
	test_pfm_memcpy((void *)MEM_ADDR_DST,(void *)MEM_ADDR_SRC,100);
	test_pfm_memcpy((void *)MEM_ADDR_DST,(void *)MEM_ADDR_SRC,200);

	test_pfm_memcpy((void *)MEM_ADDR_DST+1,(void *)MEM_ADDR_SRC+1,100);
	test_pfm_memcpy((void *)MEM_ADDR_DST+1,(void *)MEM_ADDR_SRC+1,200);

	test_pfm_memcpy((void *)MEM_ADDR_DST+2,(void *)MEM_ADDR_SRC+2,100);
	test_pfm_memcpy((void *)MEM_ADDR_DST+2,(void *)MEM_ADDR_SRC+2,200);
#endif

#if 0
	r = num_add_one(6);
	printf("num_add_one, r = 0x%x\n",r);

	r = num_shift(6);
	printf("num_shift, r = 0x%x\n",r);

	r = num_add(3,4);
	printf("num_add, r = %d\n",r);

	r = num_sub(6,4);
	printf("num_sub, r = %d\n",r);

	r = num_sum(5);
	printf("num_sum, r = %d\n",r);

	r = num_sum(100);
	printf("num_sum, r = %d\n",r);

	r = num_sum(200);
	printf("num_sum, r = %d\n",r);

	r = num_sn(1,1,5);
	printf("num_sn, r = %d\n",r);

	r = num_sn(1,2,5);
	printf("num_sn, r = %d\n",r);

	r = num_sn(0,3,5);
	printf("num_sn, r = %d\n",r);

	r = num_sn(10,10,5);
	printf("num_sn, r = %d\n",r);

#endif

#if 0
	memset_s(src_buf, 0x0,sizeof(src_buf));
	memset_s(dst_buf, 0x0,sizeof(dst_buf));

	len = 18;
	for(i = 0;i < len;i++)
		src_buf[i] = 0xa5;

	printf("dst_buf:\n");
	xmemdump(dst_buf,sizeof(dst_buf));
	printf("src_buf:\n");
	xmemdump(src_buf,sizeof(src_buf));

	printf("copy memory %d bytes...\n",len);
	addr = memcpy_s(dst_buf,src_buf,len);

	printf("addr = %x, dst_buf = %x\n",
		(unsigned int)addr,(unsigned int)dst_buf);

	printf("dst_buf:\n");
	xmemdump(dst_buf,sizeof(dst_buf));
	printf("src_buf:\n");
	xmemdump(src_buf,sizeof(src_buf));

#endif
	return 0;
}

void *xmemdump(const void *dst, unsigned size)
{
	unsigned int i;
	for(i = 0;i < size;i++) {
		if(i%8 == 0)
			printf("\n");
		printf("%2x ",*((unsigned char *)dst + i));
	}
	printf("\n");
	return (void *)dst;
}

void *xmemcpy1(void *dst, const void *src, unsigned int size)
{
	char *pdst = dst;

	while(size--) {
		*(pdst++)  = *((const char *)src++);
	}
	return dst;
}

void *xmemcpy2(void *to, const void *from, unsigned int size)
{
	unsigned n = size,m,i;

	if(!n)
		return to;
	if(n < 4) {
		char *cto = to;
		const char *cfrom = from;
		do {
			*(cto++) = *(cfrom++);
			n--;
		} while(n != 0);
		return to;
	} else {
		unsigned int ifrom = (unsigned int)from;
		unsigned int ito = (unsigned int)to;
		unsigned int ato = (ito & (~0x3)) + 0x4;
		m = ato - ito;
		if(m % 0x4) {
			char *dstmem = (char *)ito;
			char *srcmem = (char *)ifrom;
			for(i = 0;i < m;i++)
				*(dstmem++) = *(srcmem++);
			ifrom = (unsigned int)srcmem;
			n = n - m;
		}
		if(m == 4)
			ato -= 4;
		m = n >> 2;
		if(m) {
			unsigned int *dstmem = (unsigned int *)ato;
			unsigned int *srcmem = (unsigned int *)ifrom;
			for(i = 0;i < m;i++)
				*(dstmem++) = *(srcmem++);
			ifrom = (unsigned int)srcmem;
			ato   = (unsigned int)dstmem;
		}
		/* last bytes = size - sizeof(int) * m */
		m = n - (m << 2);
		if(m) {
			char *dstmem = (char *)ato;
			char *srcmem = (char *)ifrom;
			for(i = 0;i < m;i++)
				*(dstmem++) = *(srcmem++);
		}
	}
	return to;
}

void *xmemset1(void *s, int n, unsigned int size)
{
	char *cs = s;
	while(size--) {
		*(cs++) = (char)n;
	}
	return s;
}

void *xmemset2(void *s, int n, unsigned int size)
{
	unsigned int len = size,i,m;

	if(!len)
		return s;
	if(len < 4) {
		char *cs = s;
		for(i = 0;i < len;i++)
			*(cs++) = (char)n;
	} else {
		unsigned int iaddr = (unsigned int)s;
		unsigned int aaddr = (iaddr & (~0x3)) + 0x4;

		m = aaddr - iaddr;
		if(m % 4) {
			char *cs = (char *)iaddr;
			for(i = 0;i < m;i++)
				*(cs++) = (char)n;
			len = len - m;
		}
		if(m == 4)
			aaddr -= 0x4;
		m = len >> 2;
		if(m) {
			int *imem = (int *)aaddr;
			int v;

			n = n & 0xFF;
			v = n | (n << 8) | (n << 16) | (n << 24);
			for(i = 0;i < m;i++)
				*(imem++) = v;
			aaddr = (unsigned int)imem;
		}
		m = len - (m << 2);
		if(m) {
			char *cs = (char *)aaddr;
			for(i = 0;i < m;i++)
				*(cs++) = (char)n;
		}
	}
	return s;
}

int test_pfm_memset(void *addr, unsigned nMB)
{
	void *pmem;
	unsigned size = nMB << 20;
	int n = 0x5A;
	unsigned long long tick;

	printf("xmemset1 test... %x, %d\n",(unsigned int )addr,nMB);
	tick = get_ticks();
	pmem = xmemset1(addr,n,size);
	tick = get_ticks() - tick;
	if(pmem != addr) {
		printf("memory address error ! %x\n",(unsigned int)pmem);
		return -1;
	}
	printf("xmemset1 cost: tick = %llu\n",tick);


	printf("xmemset2 test... %x, %d\n",(unsigned int )addr,nMB);
	tick = get_ticks();
	pmem = xmemset2(addr,n,size);
	tick = get_ticks() - tick;
	if(pmem != addr) {
		printf("memory address error ! %x\n",(unsigned int)pmem);
		return -1;
	}
	printf("xmemset2 cost: tick = %llu\n",tick);


	printf("xmemset3 test... %x, %d\n",(unsigned int )addr,nMB);
	tick = get_ticks();
	pmem = xmemset3(addr,n,size);
	tick = get_ticks() - tick;
	if(pmem != addr) {
		printf("memory address error ! %x\n",(unsigned int)pmem);
		return -1;
	}
	printf("xmemset3 cost: tick = %llu\n",tick);


	printf("xmemset4 test... %x, %d\n",(unsigned int )addr,nMB);
	tick = get_ticks();
	pmem = xmemset4(addr,n,size);
	tick = get_ticks() - tick;
	if(pmem != addr) {
		printf("memory address error ! %x\n",(unsigned int)pmem);
		return -1;
	}
	printf("xmemset4 cost: tick = %llu\n",tick);


	printf("\ntest done !!!\n");
	return 0;
}

int test_pfm_memcpy(void *to, const void *from,unsigned nMB)
{
	void *pmem;
	unsigned size = nMB << 20;
	unsigned long long tick1;

	printf("memcpy test... to = %x, from = %x, %d\n",
		(unsigned int )to,(unsigned int) from,nMB);
	tick1 = get_ticks();
	pmem = memcpy(to,from,size);
	tick1 = get_ticks() - tick1;
	if(pmem != to) {
		printf("memory address error ! %x\n",(unsigned int)pmem);
		return -1;
	}
	printf("memcpy cost: tick = %llu\n",tick1);

	printf("xmemcpy1 test... to = %x, from = %x, %d\n",
		(unsigned int )to,(unsigned int) from,nMB);
	tick1 = get_ticks();
	pmem = xmemcpy1(to,from,size);
	tick1 = get_ticks() - tick1;
	if(pmem != to) {
		printf("memory address error ! %x\n",(unsigned int)pmem);
		return -1;
	}
	printf("xmemcpy1 cost: tick = %llu\n",tick1);

	printf("xmemcpy2 test... to = %x, from = %x,  %d\n",
		(unsigned int)to,(unsigned int )from,nMB);
	tick1 = get_ticks();
	pmem = xmemcpy2(to,from,size);
	tick1 = get_ticks() - tick1;
	if(pmem != to) {
		printf("memory address error ! %x\n",(unsigned int)pmem);
		return -1;
	}
	printf("xmemcpy2 cost: tick = %llu\n",tick1);
	printf("\ntest done !!!\n");
	return 0;

}

int test_func_memset(void *addr, unsigned max_size)
{
	void *pmem;
	unsigned int offs;
	unsigned int size = max_size >> 1;
	int n;

	for(offs = 0;offs < size;offs++) {
		n = offs + 1;
		printf("\n%s, addr = %x, n = %d,  size = %d\n\n",__func__,
			(unsigned int)(addr + offs),n,size);
	
		xmemset1(addr,0x0,max_size);
		xmemdump(addr,max_size);

		//pmem = xmemset1((addr + offs),n,size);
		//pmem = xmemset2((addr + offs),n,size);
		//pmem = xmemset3((addr + offs),n,size);
		pmem = xmemset4((addr + offs),n,size);
		xmemdump(addr,max_size);
	}
	pmem = pmem;
	return 0;
}

int test_func_memcpy(void *to, void *from, unsigned max_size)
{
	void *pmem = 0;
	unsigned int offs = 0;

	printf("%s, init data...\n",__func__);

	xmemset1(to,0x0,max_size);
	for(offs = 0;offs < max_size;offs++)
		xmemset1(from + offs,offs+1,1);
	xmemdump(to,max_size);
	xmemdump(from,max_size);

	for(offs = 0;offs < max_size;offs++) {
		printf("\nmem clear, to = %x, size = %d\n",
			(unsigned int)to,max_size);
		xmemset1(to,0x0,max_size);
		xmemdump(to,max_size);

		printf("\nmem copy, to = %x, from = %x, offs = %x, size = %d\n",
		(unsigned int )to, (unsigned int)from, offs, (max_size - offs));

		xmemcpy2(to + offs,from + offs,max_size - offs);
		xmemdump(to,max_size);
		printf("\n");
	}
	pmem = pmem;
	return 0;
}
