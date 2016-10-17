// See LICENSE for license details.

#include <util.h>
#include <sram_rw.h>
#include <multiply.h>

#define CALLEXIT_ADDR	 0x00011FFC
#define CALLEXIT_PASS	 0x900dc0de

void call_exit(int err)
{
	if(!err)
		err = CALLEXIT_PASS;
	writel(err,CALLEXIT_ADDR);
}

#define STR_MSTATUS_ADDR 0x00011FF0
void core_ecall(void);
unsigned int core_get_mstatus(void);
void core_set_mstatus(unsigned int status);

int irq_simple_test(void);
int irq_preemption_test(void);
int irq_nesting_test(void);

int add_test(void);
int sub_test(void);
int mul_test(void);
int div_test(void);
int shift_right_test(void);
int shift_left_test(void);

int timer_test(void);
int serial_test(void);
int wdog_test(void);

int main( int argc, char* argv[] )
{
	int r = 0,err = 0;
#ifdef CONFIG_MULTIPLY_TEST
	unsigned int hex = 0x1234ABCD;
	unsigned int status;
	char dst[10] = {0};

	r = mem32_simple_rw();
	if(r != 0)
		err |= 0x1;

	r = data32_sum(100);
	if(r != 5050)
		err |= 0x2;

	hex2asc(hex,dst);
	r = asc2hex(dst);
	if(r != hex)
		err |= 0x4;

	call_exit(err);

	/*
	 * ecall inst test
	 */
	core_ecall();
	status = core_get_mstatus();
	*(unsigned int *)STR_MSTATUS_ADDR = status;

	/*
	 * Multiply test
	 */
	if(multiply(10,8) != 80)
		err |= 0x8;

	if(multiply(10,80) != 800)
		err |= 0x10;

	if(multiply(10,800) != 8000)
		err |= 0x20;
#endif
#ifdef CONFIG_MEM32_TEST
	/*
	 * Memory 32bit, 16bit, 8bit reading/writing test
	 */
	if(mem32_rw() != 0)
		err |= 0x40;

	if(mem16_rw() != 0)
		err |= 0x80;

	if(mem8_rw() != 0)
		err |= 0x100;

	if(mem32_bit_1_rw_first512B() != 0)
		err |= 0x200;

	if(mem32_bit_0_rw_first512B() != 0)
		err |= 0x400;

	if(mem32_bit_1_rw_last512B() != 0)
		err |= 0x800;

	if(mem32_bit_0_rw_last512B() != 0)
		err |= 0x1000;
#endif
#ifdef CONFIG_IRQ_TEST
	if(irq_simple_test() != 0)
		err |= 0x2000;
	if(irq_nesting_test() != 0)
		err |= 0x4000;
	if(irq_preemption_test() != 0)
		err |= 0x8000;
#endif
#ifdef CONFIG_MATH_TEST
	if(add_test())
		err |= 0x10000;
	if(sub_test())
		err |= 0x20000;
	if(mul_test())
		err |= 0x40000;
	if(div_test())
		err |= 0x80000;
	if(shift_left_test())
		err |= 0x100000;
	if(shift_right_test())
		err |= 0x200000;
#endif
#ifdef CONFIG_TIMER_TEST
	if(timer_test())
		err |= 0x400000;
#endif
#ifdef CONFIG_SERIAL_TEST
	if(serial_test())
		err |= 0x800000;
#endif
#ifdef CONFIG_WDOG_TEST
	if(wdog_test())
		err |= 0x1000000;
#endif
#ifdef CONFIG_LSU_TEST
	if(mem32_invalid_access() != 0)
		err |= 0x80000000;
#endif
	call_exit(err);
	return 0;
}
