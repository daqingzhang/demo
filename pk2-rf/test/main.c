// See LICENSE for license details.

#include <util.h>
#include <sram_rw.h>
#include <irq.h>

#define CALLEXIT_ADDR	 0x00011FFC
#define CALLEXIT_PASS	 0x900dc0de

void call_exit(int err)
{
	if(!err)
		err = CALLEXIT_PASS;
	writel(err,CALLEXIT_ADDR);
}

volatile int ecall_excp_done = 0;

int multi_test(void);

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
	u32 status;
#ifdef CONFIG_MULTIPLY_TEST
	if(multi_test())
		err |= 0x01;
#endif
#ifdef CONFIG_ECALL_TEST
	ecall_excp_done = 0;
	core_ecall();
	while(ecall_excp_done == 0);
	status = core_get_mstatus();
	serial_puts("ecall exception out, mstatus is: ");
	print_u32(status);
	serial_puts("\n");
	serial_puts("ecall, test success !\n");
#endif
#ifdef CONFIG_MEM32_TEST
	r = mem32_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x40;
		serial_puts("mem32_rw,error code ");
		print_u32(r);
		serial_puts("\n");
	} else {
		serial_puts("mem32_rw, test success !\n");
	}

	r = mem16_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x80;
		serial_puts("mem16_rw,error code ");
		print_u32(r);
		serial_puts("\n");
	} else {
		serial_puts("mem16_rw, test success !\n");
	}

	r = mem8_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x100;
		serial_puts("mem8_rw,error code ");
		print_u32(r);
		serial_puts("\n");
	} else {
		serial_puts("mem8_rw, test success !\n");
	}

	r = mem32_bit_1_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x200;
		serial_puts("mem32_bit_1_rw,error code ");
		print_u32(r);
		serial_puts("\n");
	} else {
		serial_puts("mem32_bit_1_rw, test success !\n");
	}

	r = mem32_bit_0_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x400;
		serial_puts("mem32_bit_0_rw,error code ");
		print_u32(r);
		serial_puts("\n");
	} else {
		serial_puts("mem32_bit_0_rw, test success !\n");
	}
#endif
#ifdef CONFIG_IRQ_TEST
	if(irq_simple_test())
		err |= 0x2000;
#ifdef CONFIG_SUPPORT_NESTED_IRQ
	if(irq_nesting_test())
		err |= 0x4000;
	if(irq_preemption_test())
		err |= 0x8000;
#endif

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
	if(err) {
		serial_puts("\n main, error code ");
		print_u32(err);
		serial_puts("\n");
		call_exit(err);
		return -1;
	}
	serial_puts("\n main, test success !!!\n");
	return 0;
}
