// See LICENSE for license details.
#include <util.h>
#include <irq.h>

#define CALLEXIT_ADDR	 0x00011FFC
#define CALLEXIT_PASS	 0x900dc0de

static void call_exit(int err)
{
	if(!err)
		err = CALLEXIT_PASS;
	writel(err,CALLEXIT_ADDR);
}

#ifdef CONFIG_ECALL_TEST
volatile int ecall_excp_done = 0;
#endif
#ifdef CONFIG_MULTIPLY_TEST
int multi_test(void);
#endif
#ifdef CONFIG_MEM32_TEST
#include <sram_rw.h>
#endif
#ifdef CONFIG_IRQ_TEST
int irq_simple_test(void);
int irq_preemption_test(void);
int irq_nesting_test(void);
#endif
#ifdef CONFIG_MATH_TEST
int add_test(void);
int sub_test(void);
int mul_test(void);
int div_test(void);
int shift_right_test(void);
int shift_left_test(void);
#endif
#ifdef CONFIG_TIMER_TEST
int timer_test(void);
#endif
#ifdef CONFIG_SERIAL_TEST
int serial_test(void);
#endif
#ifdef CONFIG_WDOG_TEST
int wdog_test(void);
#endif
#ifdef CONFIG_REGS_TEST
int dump_all_regs(void);
#endif
#ifdef CONFIG_LSU_TEST
int mem32_invalid_access(void)
#endif

int main( int argc, char* argv[] )
{
	int r = 0,err = 0;
	u32 status = 0;

	r = r;
	err = err;
	status = status;
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
		err |= 0x02;
		serial_puts("mem32_rw,error code ");
		print_u32(r);
		serial_puts("\n");
	} else {
		serial_puts("mem32_rw, test success !\n");
	}

	r = mem16_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x04;
		serial_puts("mem16_rw,error code ");
		print_u32(r);
		serial_puts("\n");
	} else {
		serial_puts("mem16_rw, test success !\n");
	}

	r = mem8_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x08;
		serial_puts("mem8_rw,error code ");
		print_u32(r);
		serial_puts("\n");
	} else {
		serial_puts("mem8_rw, test success !\n");
	}

	r = mem32_bit_1_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x10;
		serial_puts("mem32_bit_1_rw,error code ");
		print_u32(r);
		serial_puts("\n");
	} else {
		serial_puts("mem32_bit_1_rw, test success !\n");
	}

	r = mem32_bit_0_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x20;
		serial_puts("mem32_bit_0_rw,error code ");
		print_u32(r);
		serial_puts("\n");
	} else {
		serial_puts("mem32_bit_0_rw, test success !\n");
	}
#endif
#ifdef CONFIG_IRQ_TEST
	if(irq_simple_test())
		err |= 0x40;
#ifdef CONFIG_SUPPORT_NESTED_IRQ
	if(irq_nesting_test())
		err |= 0x80;
	if(irq_preemption_test())
		err |= 0x100;
#endif

#endif
#ifdef CONFIG_MATH_TEST
	if(add_test())
		err |= 0x200;
	if(sub_test())
		err |= 0x400;
	if(mul_test())
		err |= 0x800;
	if(div_test())
		err |= 0x1000;
	if(shift_left_test())
		err |= 0x2000;
	if(shift_right_test())
		err |= 0x4000;
#endif
#ifdef CONFIG_TIMER_TEST
	if(timer_test())
		err |= 0x8000;
#endif
#ifdef CONFIG_SERIAL_TEST
	if(serial_test())
		err |= 0x10000;
#endif
#ifdef CONFIG_WDOG_TEST
	if(wdog_test())
		err |= 0x20000;
#endif
#ifdef CONFIG_REGS_TEST
	if(dump_all_regs())
		err |= 0x40000;
#endif
#ifdef CONFIG_LSU_TEST
	if(mem32_invalid_access() != 0)
		err |= 0x80000;
#endif
	if(err) {
		serial_puts("\n main, error code ");
		print_u32(err);
		serial_puts("\n");
		call_exit(err);
		return -1;
	}
	serial_puts("\n main, test success !!!\n");
	call_exit(err);
	return 0;
}
