#ifndef __SRAM_RW_H__
#define __SRAM_RW_H__

int mem32_simple_rw(void);
int mem32_rw(void);
int mem16_rw(void);
int mem8_rw(void);
int mem32_bit_1_rw(void);
int mem32_bit_0_rw(void);

int mem32_bit_0_rw_first512B(void);
int mem32_bit_1_rw_first512B(void);
int mem32_bit_0_rw_last512B(void);
int mem32_bit_1_rw_last512B(void);

#endif
