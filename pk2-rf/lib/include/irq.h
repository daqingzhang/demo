#ifndef __IRQ_H__
#define __IRQ_H__
#include <regs/reg_irqs.h>

#define IRQ_MASK_ENABLE  0xffffffff
#define IRQ_MASK_DISABLE 0xffffffff

#define IRQ_MASK_REG00_WR	(1 << 0)
#define IRQ_MASK_REG02_WR	(1 << 1)
#define IRQ_MASK_REG04_WR	(1 << 2)
#define IRQ_MASK_REG06_WR	(1 << 3)
#define IRQ_MASK_TIMER0_UDF 	(1 << 4)
#define IRQ_MASK_TIMER1_UDF 	(1 << 5)
#define IRQ_MASK_TIMER2_UDF	(1 << 6)
#define IRQ_MASK_AON		(1 << 7)
#define IRQ_MASK_REG10_WR	(1 << 8)
#define IRQ_MASK_REG12_WR	(1 << 9)
#define IRQ_MASK_REG14_WR	(1 << 10)
#define IRQ_MASK_REG16_WR	(1 << 11)
#define IRQ_MASK_REG18_WR	(1 << 12)
#define IRQ_MASK_REG1A_WR	(1 << 13)
#define IRQ_MASK_REG1C_WR	(1 << 14)
#define IRQ_MASK_REG1E_WR	(1 << 15)
#define IRQ_MASK_REG20_WR	(1 << 16)
#define IRQ_MASK_REG22_WR	(1 << 17)
#define IRQ_MASK_REG24_WR	(1 << 18)
#define IRQ_MASK_REG26_WR	(1 << 19)
#define IRQ_MASK_REG28_WR	(1 << 20)
#define IRQ_MASK_REG2A_WR	(1 << 21)
#define IRQ_MASK_REG2C_WR	(1 << 22)
#define IRQ_MASK_REG2E_WR	(1 << 23)
#define IRQ_MASK_RSV1		(1 << 24)
#define IRQ_MASK_RSV2		(1 << 25)
#define IRQ_MASK_RSV3		(1 << 26)
#define IRQ_MASK_RSV4		(1 << 27)
#define IRQ_MASK_UART_RX_DONE	(1 << 28)
#define IRQ_MASK_UART_TX_DONE	(1 << 29)
#define IRQ_MASK_UART_OVR	(1 << 30)
#define IRQ_MASK_WDOG_UDF 	(1 << 31)

void irq_enable(u32 mask);
void irq_disable(u32 mask);
void irq_set_pending(u32 mask);
void irq_clr_pending(u32 mask);
u32  irq_get_status(u32 mask);
u32  irq_get_enable_status(void);

void core_irq_enable(void);
void core_irq_disable(void);
void core_ecall(void);
void core_set_mstatus(unsigned int status);
u32  core_get_mstatus(void);

#endif
