#ifndef _REG_GENERAL_H_
#define _REG_GENERAL_H_
#include <util.h>

typedef struct {
    unsigned int address;
    unsigned int reset_val;
    unsigned int rw_en;
    unsigned int rst_test_en;
    unsigned int rw_test_en;
    unsigned int dump_for_sdata;
} REG_DEF_S;

#define INIT_REG_DEF_S(_addr, _rst_val, _rw_en, _rst_test_en, _rw_test_en,_dump) \
{				\
	.address = _addr,	\
	.reset_val = _rst_val,	\
	.rw_en = _rw_en,	\
	.rst_test_en = _rst_test_en, \
	.rw_test_en = _rw_test_en, \
	.dump_for_sdata = _dump,	\
}

int reg32_hw_reset_test(REG_DEF_S *reg);
int reg32_rw_test(REG_DEF_S *reg);
int reg16_hw_reset_test(REG_DEF_S *reg);
int reg16_rw_test(REG_DEF_S *reg);

void PRINTF(const char *s,u32 val);

#endif

