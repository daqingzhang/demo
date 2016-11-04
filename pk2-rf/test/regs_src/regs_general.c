#include <regs_general.h>

int reg32_hw_reset_test(REG_DEF_S *reg)
{
	unsigned int reg_value;

	if(reg->rst_test_en) {//reg reset test needed
		reg_value = *(unsigned int *)(reg->address);
		if(reg_value != reg->reset_val)
			PRINTF("reg_value: ",reg_value);
			PRINTF("rst_value: ",reg->reset_val);
			return -1;
	}
	return 0;
}

int reg32_rw_test(REG_DEF_S *reg)
{
	unsigned int reg_value, reg_r_after_w;

	if(!reg->rw_test_en){//reg all bits no need to test
		return 0;
	} else{
		reg_value = *(unsigned int *)(reg->address);
		*(unsigned int *)(reg->address) = ~reg_value;
		mdelay(10);
		reg_r_after_w = *(unsigned int *)(reg->address);
		if((reg_r_after_w & reg->rw_test_en) != (((reg_value & ~reg->rw_en) & (~reg_value & reg->rw_en)) & reg->rw_test_en)){
			PRINTF("rd->wr->rd: reg_value: ", reg_value);
			PRINTF("rd->wr->rd: ret_r_after_w: ", reg_r_after_w);
			return -1;
		}

		*(unsigned int *)(reg->address) = reg_value;
		mdelay(10);
		reg_r_after_w = *(unsigned int *)(reg->address);
		if((reg_r_after_w & reg->rw_test_en) != (reg_value & reg->rw_test_en)){
			PRINTF("wr->rd: reg_value: ", reg_value);
			PRINTF("wr->rd: ret_r_after_w: ", reg_r_after_w);
			return -1;
		}
	}
	return 0;
}

int reg16_hw_reset_test(REG_DEF_S *reg)
{
	unsigned short reg_value;

	if(reg->rst_test_en){//reg reset test needed
		reg_value = *(unsigned short *)(reg->address);
		if(reg_value != reg->reset_val) {
			PRINTF("reg_value: ",reg_value);
			PRINTF("rst_value: ",reg->reset_val);
			return -1;
		}
	}
	return 0;
}

int reg16_rw_test(REG_DEF_S *reg)
{
	unsigned short reg_value, reg_r_after_w;

	if(!reg->rw_test_en){//reg all bits no need to test
		return 0;
	} else {
		// rd->wr->rd
		reg_value = *(unsigned short *)(reg->address);
		*(unsigned short *)(reg->address) = ~reg_value;
		mdelay(10);
		reg_r_after_w = *(unsigned short *)(reg->address);
		if((reg_r_after_w & reg->rw_test_en) != (((reg_value & ~reg->rw_en) | (~reg_value & reg->rw_en)) & reg->rw_test_en)){
			PRINTF("rd->wr->rd: reg_value: ", reg_value);
			PRINTF("rd->wr->rd: ret_r_after_w: ", reg_r_after_w);
			return -1;
		}
		// wr->rd
		*(unsigned short *)(reg->address) = reg_value;
		mdelay(10);
		reg_r_after_w = *(unsigned short *)(reg->address);
		if((reg_r_after_w & reg->rw_test_en) != (reg_value & reg->rw_test_en)){
			PRINTF("wr->rd: reg_value: ", reg_value);
			PRINTF("wr->rd: ret_r_after_w: ", reg_r_after_w);
			return -1;
		}
	}
	return 0;
}

void PRINTF(const char *s,u32 val)
{
	serial_puts(s);
	print_u32(val);
	serial_puts("\n");
}
