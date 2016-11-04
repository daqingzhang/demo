#include <regs_general.h>

#define ADDR_BASE  0x00000000

static REG_DEF_S rtc_regs[] =
{
	INIT_REG_DEF_S(ADDR_BASE + 0x000340a0, 0x0100, 0xffff, 0x1, 0xffff, 0x1),
	INIT_REG_DEF_S(ADDR_BASE + 0x000340a4, 0x0000, 0xffff, 0x1, 0xffff, 0x1),
	INIT_REG_DEF_S(ADDR_BASE + 0x000340a8, 0x0000, 0xffff, 0x1, 0xffff, 0x1),
	INIT_REG_DEF_S(ADDR_BASE + 0x000340ac, 0x0054, 0xffff, 0x1, 0xffff, 0x1),
	INIT_REG_DEF_S(ADDR_BASE + 0x000340b0, 0x8040, 0xffff, 0x1, 0xffff, 0x1),
	INIT_REG_DEF_S(ADDR_BASE + 0x000340b4, 0x9100, 0xffff, 0x1, 0xffff, 0x1),
	INIT_REG_DEF_S(ADDR_BASE + 0x000340b8, 0x0500, 0xffff, 0x1, 0xffff, 0x1),
	INIT_REG_DEF_S(ADDR_BASE + 0x000340bc, 0x0001, 0xffff, 0x1, 0xffff, 0x1),
};

int rtc_reg_hw_reset_test(void)
{
	int i,err = 0;
	REG_DEF_S *ptr_regs = rtc_regs;

	for(i = 0; i < ARRAY_SIZE(rtc_regs); i++) {
		if(reg16_hw_reset_test(ptr_regs) != 0) {
			PRINTF("rtc_reg_hw_reset_test, error addr = ", ptr_regs->address);
			//return -1;
			err++;
		}
		ptr_regs++;
	}
	if(!err)
		serial_puts("rtc_reg_hw_reset_test, test success !\n");
	return 0;
}

int rtc_reg_rw_test(void)
{
	int i,err = 0;
	REG_DEF_S *ptr_regs = rtc_regs;

	for(i=0; i < ARRAY_SIZE(rtc_regs); i++) {
		if(reg16_rw_test(ptr_regs) != 0) {
			PRINTF("rtc_reg_rw_test, error addr =  ", ptr_regs->address);
			//return -1;
			err++;
		}
		ptr_regs++;
	}
	if(!err)
		serial_puts("rtc_reg_rw_test, test success !\n");
	return 0;
}

int rtc_reg_test(void)
{
	int r;

	r = rtc_reg_hw_reset_test();
	r += rtc_reg_rw_test();

	return r;
}
