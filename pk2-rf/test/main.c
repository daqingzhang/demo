// See LICENSE for license details.
#include <util.h>
#include <sysctrl.h>
#include <gpio.h>

#define CALLEXIT_ADDR	 0x0000B000
#define CALLEXIT_PASS	 0x900dc0de

void board_init(int flag)
{
	/* disable global IRQ */
	core_irq_disable();

	/* configure system clock */
	sysctrl_set_system_clock(CONFIG_SYSCLK_VALUE);

	/* don't bypass watchdog */
	sysctrl_bypass_watchdog(0);

	/* enable hardware error response */
	sysctrl_hwerr_response(1);

	/* reset hardware enable */
	sysctrl_soft_rst1_en(SYSCTRL_MASK_RST1_TIMER0
				| SYSCTRL_MASK_RST1_WDOG
				| SYSCTRL_MASK_RST1_TIMER1
				| SYSCTRL_MASK_RST1_TIMER2);
	nop();
	nop();
	nop();
	nop();
	/* reset hardware disable */
	sysctrl_soft_rst1_dis(SYSCTRL_MASK_RST1_TIMER0
				| SYSCTRL_MASK_RST1_WDOG
				| SYSCTRL_MASK_RST1_TIMER1
				| SYSCTRL_MASK_RST1_TIMER2);
	nop();
	nop();
	nop();
	nop();

	/* initial pin-mux */

	/* initial GPIOs,input */
	gpio_set_direction(0xFFFFFFFF,1);

	/* initial UART */
	serial_init();

	/* enable IRQs */
	irq_clr_pending(0xFFFFFFFF);
	core_irq_enable();
	//irq_enable(0xFFFFFFFF);
}

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

void print_result(const char *s, u32 err)
{
	if(err) {
		serial_puts(s);
		serial_puts(", error code: ");
		print_u32(err);
		serial_puts(" ###\n");
	} else {
		serial_puts(s);
		serial_puts(", test success !\n");
	}
}

void print_dat32(const char *str, u32 dat)
{
	serial_puts(str);
	print_u32(dat);
	serial_puts("\n");
}

void do_read_register(void);

int main( int argc, char* argv[] )
{
	int r = 0,err = 0;
	u32 status = 0;
	u32 id = 0;

	r = r;
	err = err;
	status = status;

	serial_puts("main, test start\n");

	id = core_get_mcpuid();
	print_dat32("mcpuid: ",id);
	if((id & 0xc0000000) == 0)
		serial_puts("RV32I,");
	else
		serial_puts("none RV32I,");
	if(id & 0x00000100) // bit8
		serial_puts("I,");
	if(id & 0x00001000) // bit12
		serial_puts("M,");
	if(id & 0x00800000) // bit23
		serial_puts("X");
	serial_puts("\n");

	id = core_get_mpid();
	print_dat32("mpid: ",id);

	id = core_get_mhartid();
	print_dat32("mhartid: ",id);

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

#ifdef CONFIG_IRQ_TEST
	if(irq_simple_test())
		err |= 0x40;
#ifdef CONFIG_SUPPORT_NESTED_IRQ
	if(irq_nesting_test())
		err |= 0x80;
	if(irq_preemption_test())
		err |= 0x100;
#endif /* CONFIG_SUPPORT_NESTED_IRQ */
#endif /* CONFIG_IRQ_TEST */

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
#ifdef CONFIG_REGS_TEST
	if(dump_all_regs())
		err |= 0x40000;
#endif
#ifdef CONFIG_MEM32_TEST
	r = mem32_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x02;
	}
	print_result("mem32_rw",r);

	r = mem16_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x04;
	}
	print_result("mem16_rw",r);

	r = mem8_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x08;
	}
	print_result("mem8_rw",r);
#if 1
	r = mem8_rw(CONFIG_SRAM0_END - 0X1000,0x1000);
	if(r) {
		err |= 0x100000;
	}
	print_result("mem8_rw",r);

	r = mem8_rw(CONFIG_SRAM1_BASE + 0X1000,0x2000);
	if(r) {
		err |= 0x200000;
	}
	print_result("mem8_rw",r);
#endif
	r = mem32_bit_1_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x10;
	}
	print_result("mem32_bit_1_rw",r);

	r = mem32_bit_0_rw(MEM32_ADDR,MEM32_SIZE);
	if(r) {
		err |= 0x20;
	}
	print_result("mem32_bit_0_rw",r);

#endif
#ifdef CONFIG_WDOG_TEST
	if(wdog_test())
		err |= 0x20000;
#endif
#ifdef CONFIG_LSU_TEST
	if(mem32_invalid_access() != 0)
		err |= 0x80000;
#endif
#if 0
	do_read_register();
#endif
	if(err) {
		serial_puts("\nmain, error code ");
		print_u32(err);
		serial_puts(" ###\n");
		call_exit(err);
		return -1;
	}
	serial_puts("\nmain, test success !!!\n");
	call_exit(err);
	return 0;
}

#define CPU_DEBUGA_BASE (RDA_DBUG_BASE + 0x0000)
#define CPU_DEBUGB_BASE	(RDA_DBUG_BASE + 0x2000)
#define CPU_CSR_BASE	(RDA_DBUG_BASE + 0x4000)
#define CPU_GPR_BASE	(RDA_DBUG_BASE + 0x0400)
#define CPU_FPR_BASE	(RDA_DBUG_BASE + 0x0500)

void do_read_register(void)
{
	u32 reg,addr;
	u32 i;
//	u32 offs = 0;

	serial_puts("do_read_register\n");
#if 1
	for(i = 0; i < 4;i++) {
		addr = CPU_DEBUGA_BASE + i * 4;
		serial_puts("addr = ");
		print_u32(addr);

		serial_puts(", val = ");
		reg = readl(addr);
		print_u32(reg);
		serial_puts("\n");
	}
	i = 0;
	while(1) {

		addr = CPU_DEBUGA_BASE + 0x2000;
		reg = readl(addr);

		serial_puts("addr = ");
		print_u32(addr);
		serial_puts(", val = ");
		reg = readl(addr);
		print_u32(reg);
		serial_puts("\n");

		addr = CPU_DEBUGA_BASE + 0x2004;
		reg = readl(addr);

		serial_puts("addr = ");
		print_u32(addr);
		serial_puts(", val = ");
		reg = readl(addr);
		print_u32(reg);
		serial_puts("\n");

		mdelay(500);
		mdelay(500);

		i++;
		if( (i%32) == 0 ) {
			// to halt cpu
			addr = CPU_DEBUGA_BASE + 0x0; //DEBUG_CTRL register
			reg = (1 << 16);

			serial_puts("will write reg ");
			print_u32(addr);
			serial_puts(" = ");
			print_u32(reg);
			serial_puts("\n");

			*(volatile unsigned int *)addr = reg;

			serial_puts("after cpu unhalt\n");
			print_u32(i);
		}
		serial_puts(">>>\n");
	}
#endif

#if 0
	while(1) {

		reg = core_get_mcpuid();
		serial_puts("mcpuid = ");
		print_u32(reg);
		serial_puts("\n");

		reg = core_get_mpid();
		serial_puts("mpid = ");
		print_u32(reg);
		serial_puts("\n");

		reg = core_get_mhartid();
		serial_puts("mhartid = ");
		print_u32(reg);
		serial_puts("\n");

		mdelay(500);
		mdelay(500);
	}
#endif
}

