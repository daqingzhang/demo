#ifndef __SRAM_RW_H__
#define __SRAM_RW_H__

#define DATA_SRAM_BASE (CONFIG_SRAM1_BASE + 0x1000)
#define DATA_SRAM_SIZE ((16 - 4) * (1024))
#define DATA_SRAM_BASE2 (0x00013000)

#define MEM32_ADDR (CONFIG_STACK_TOP - 0x1000)
#define MEM32_SIZE (0x400)

int mem32_rw(unsigned int addr, unsigned int size);
int mem16_rw(unsigned int addr, unsigned int size);
int mem8_rw(unsigned int addr, unsigned int size);
int mem32_bit_1_rw(unsigned int addr, unsigned int size);
int mem32_bit_0_rw(unsigned int addr, unsigned int size);

int mem32_bit_0_rw_first512B(void);
int mem32_bit_1_rw_first512B(void);
int mem32_bit_0_rw_last512B(void);
int mem32_bit_1_rw_last512B(void);

#endif
