#include <config.h>
#include <serial.h>

static unsigned int add_num32(unsigned int a,unsigned int b)
{
	return (a + b);
}

int add_test(void)
{
	int err = 0;

	if(add_num32(0x00000000,0x00000000) != 0x00000000)
		err |= 0x01;
	if(add_num32(0x00000001,0x00000001) != 0x00000002)
		err |= 0x02;
	if(add_num32(0x00000003,0x00000007) != 0x0000000A)
		err |= 0x04;

	if(add_num32(0xFFFF8000,0x00000000) != 0xFFFF8000)
		err |= 0x08;
	if(add_num32(0x80000000,0x00000000) != 0x80000000)
		err |= 0x10;
	if(add_num32(0x80000000,0xFFFF8000) != 0x7FFF8000)
		err |= 0x20;

	if(add_num32(0x00000000,0x00007FFF) != 0x00007FFF)
		err |= 0x40;
	if(add_num32(0x7FFFFFFF,0x00000000) != 0x7FFFFFFF)
		err |= 0x80;
	if(add_num32(0x7FFFFFFF,0x00007FFF) != 0x80007FFE)
		err |= 0x100;

	if(add_num32(0x80000000,0x00007FFF) != 0x80007FFF)
		err |= 0x200;
	if(add_num32(0x7FFFFFFF,0xFFFF8000) != 0x7FFF7FFF)
		err |= 0x400;

	if(add_num32(0xFFFFFFFF,0x00000000) != 0xFFFFFFFF)
		err |= 0x800;
	if(add_num32(0x00000001,0xFFFFFFFF) != 0x00000000)
		err |= 0x1000;
	if(add_num32(0xFFFFFFFF,0xFFFFFFFF) != 0xFFFFFFFE)
		err |= 0x2000;

	if(add_num32(0x00000001,0x7FFFFFFF) != 0x80000000)
		err |= 0x4000;

	if(err) {
		serial_puts("add_test, error code ");
		print_u32(err);
		serial_puts("\n");
		return err;
	}
	serial_puts("add_test, test success !\n");
	return 0;
}

static unsigned int sub_num32(unsigned int a,unsigned int b)
{
	return (a-b);
}

int sub_test(void)
{
	int err = 0;

	if(0x00000000 != sub_num32(0x00000000,0x00000000))
		err |= 0x01;
	if(0x00000000 != sub_num32(0x00000001,0x00000001))
		err |= 0x02;
	if(0xFFFFFFFC != sub_num32(0x00000003,0x00000007))
		err |= 0x04;

	if(0x00008000 != sub_num32(0x00000000,0xFFFF8000))
		err |= 0x08;
	if(0x80000000 != sub_num32(0x80000000,0x00000000))
		err |= 0x10;
	if(0x80008000 != sub_num32(0x80000000,0xFFFF8000))
		err |= 0x20;

	if(0xFFFF8001 != sub_num32(0x00000000,0x00007FFF))
		err |= 0x40;
	if(0x7FFFFFFF != sub_num32(0x7FFFFFFF,0x00000000))
		err |= 0x80;
	if(0x7FFF8000 != sub_num32(0x7FFFFFFF,0x00007FFF))
		err |= 0x100;

	if(0x7FFF8001 != sub_num32(0x80000000,0x00007FFF))
		err |= 0x200;
	if(0x80007FFF != sub_num32(0x7FFFFFFF,0xFFFF8000))
		err |= 0x400;

	if(0x00000001 != sub_num32(0x00000000,0xFFFFFFFF))
		err |= 0x800;
	if(0xfffffffe != sub_num32(0xffffffff,0x00000001))
		err |= 0x1000;
	if(0x00000000 != sub_num32(0xffffffff,0xffffffff))
		err |= 0x2000;

	if(err) {
		serial_puts("sub_test, error code ");
		print_u32(err);
		serial_puts("\n");
		return err;
	}
	serial_puts("sub_test, test success !\n");
	return 0;
}

static unsigned int mul_num32(unsigned int a,unsigned int b)
{
	return (a * b);
}

int mul_test(void)
{
	int err = 0;

	if(0x00000000 != mul_num32(0x00000000,0x00000001))
		err |= 0x01;
	if(0x00000009 != mul_num32(0x00000003,0x00000003))
		err |= 0x02;
	if(0x00000001 != mul_num32(0x00000001,0x00000001))
		err |= 0x04;

	if(0x00000000 != mul_num32(0x00000000,0xFFFF8000))
		err |= 0x08;
	if(0x00000000 != mul_num32(0x80000000,0x00000000))
		err |= 0x10;
	if(0x00000000 != mul_num32(0x80000000,0xFFFF8000))
		err |= 0x20;

	if(0x00007FFF != mul_num32(0x00000001,0x00007FFF))
		err |= 0x40;
	if(0x80000000 != mul_num32(0x80000000,0x00007FFF))
		err |= 0x80;
	if(0x00010000 != mul_num32(0x00007fff,0x7fff0000))
		err |= 0x100;

	if(0xffffffff != mul_num32(0xffffffff,0x00000001))
		err |= 0x200;
	if(0x80000000 != mul_num32(0xffffffff,0x80000000))
		err |= 0x400;
	if(0x00000001 != mul_num32(0xffffffff,0xffffffff))
		err |= 0x400;

	if(err) {
		serial_puts("mul_test, error code ");
		print_u32(err);
		serial_puts("\n");
		return err;
	}
	serial_puts("mul_test, test success !\n");
	return 0;
}

static unsigned int div_num32(unsigned int a,unsigned int b)
{
	return(a/b);
}

int div_test(void)
{
	int err = 0;

	if(0x00000000 != div_num32(0x00000000,0x00000001))
		err |= 0x01;
	if(0x00000001 != div_num32(0x00000003,0x00000003))
		err |= 0x02;
	if(0x00000003 != div_num32(0x00000007,0x00000002))
		err |= 0x04;

	if(0x00000000 != div_num32(0x00000000,0xFFFF8000))
		err |= 0x08;
	if(0xffffffff != div_num32(0x80000000,0x00000000))// This is a very bad feature !
		err |= 0x10;
	if(0x00000000 != div_num32(0x80000000,0xFFFF8000))
		err |= 0x20;

	if(0x00000000 != div_num32(0x00000001,0x00007FFF))
		err |= 0x40;
	if(0x00010002 != div_num32(0x80000000,0x00007FFF))
		err |= 0x80;
	if(0x00010000 != div_num32(0x7fff0000,0x00007fff))
		err |= 0x100;

	if(0xffffffff != div_num32(0xffffffff,0x00000001))
		err |= 0x200;
	if(0x00000001 != div_num32(0xffffffff,0x80000000))
		err |= 0x400;
	if(0x00020004 != div_num32(0xffffffff,0x00007fff))
		err |= 0x800;
	if(0x00000001 != div_num32(0xffffffff,0xffffffff))
		err |= 0x1000;

	if(err) {
		serial_puts("div_test, error code ");
		print_u32(err);
		serial_puts("\n");
		return err;
	}
	serial_puts("div_test, test success !\n");
	return 0;
}

static unsigned int lsl_num32(unsigned int val, unsigned int bit)
{
	val = val << bit;
	return val;
}

int shift_left_test(void)
{
	int err = 0;

	if(0x00000002 != lsl_num32(0x00000001,1))
		err |= 0x01;
	if(0x00000004 != lsl_num32(0x00000001,2))
		err |= 0x02;
	if(0x00000008 != lsl_num32(0x00000001,3))
		err |= 0x04;
	if(0x00000010 != lsl_num32(0x00000001,4))
		err |= 0x08;

	if(0x00000000 != lsl_num32(0x80000000,1))
		err |= 0x10;
	if(0xfff00000 != lsl_num32(0x7fff0000,4))
		err |= 0x20;
	if(0x00000000 != lsl_num32(0x7fff0000,16))
		err |= 0x40;
	if(0x0007fff0 != lsl_num32(0x00007fff,4))
		err |= 0x80;
	if(0x7fff0000 != lsl_num32(0x00007fff,16))
		err |= 0x100;

	if(err) {
		serial_puts("shift_left_test, error code ");
		print_u32(err);
		serial_puts("\n");
		return err;
	}
	serial_puts("shift_left_test, test success !\n");
	return err;
}

static unsigned int lsr_num32(unsigned int val, unsigned int bit)
{
	val = val >> bit;
	return val;
}

int shift_right_test(void)
{
	int err = 0;

	if(0x00000000 != lsr_num32(0x00000001,1))
		err |= 0x01;
	if(0x00000000 != lsr_num32(0x00000001,2))
		err |= 0x02;
	if(0x00000000 != lsr_num32(0x00000001,3))
		err |= 0x04;
	if(0x00000000 != lsr_num32(0x00000001,4))
		err |= 0x08;

	if(0x40000000 != lsr_num32(0x80000000,1))///////
		err |= 0x10;
	if(0x07fff000 != lsr_num32(0x7fff0000,4))
		err |= 0x20;
	if(0x00007fff != lsr_num32(0x7fff0000,16))
		err |= 0x40;
	if(0x000007ff != lsr_num32(0x00007fff,4))
		err |= 0x80;
	if(0x00000000 != lsr_num32(0x00007fff,16))
		err |= 0x100;

	if(err) {
		serial_puts("shift_right_test, error code ");
		print_u32(err);
		serial_puts("\n");
		return err;
	}
	serial_puts("shift_right_test, test success !\n");
	return 0;
}
