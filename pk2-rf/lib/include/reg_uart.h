#ifndef __REG_UART_H__
#define __REG_UART_H__
#include <config.h>

typedef volatile struct {
	__REG32_RW_ data;	//0x00
	__REG32_RW_ state;	//0x04
	__REG32_RW_ ctrl;	//0x08
	__REG32_RW1 intclr;	//0x0c
	__REG32_RW_ bauddiv;	//0x10
}hwp_uart_t;

#define HWP_UART	((hwp_uart_t *)(RDA_UART_BASE))

/* data */
#define UART_DATA_MASK	(0x000000FF)

/* state */
#define UART_STATE_RX_OVR (1 << 3)
#define UART_STATE_TX_OVR (1 << 2)
#define UART_STATE_RX_FUL (1 << 1)
#define UART_STATE_TX_FUL (1 << 0)
#define UART_STATE_MASK	  (0x0000000F)

/* ctrl */
#define UART_CTRL_TX_HS_MODE_EN	(1 << 6)
#define UART_CTRL_RX_OVR_INTEN	(1 << 5)
#define UART_CTRL_TX_OVR_INTEN	(1 << 4)
#define UART_CTRL_RX_INTEN	(1 << 3)
#define UART_CTRL_TX_INTEN	(1 << 2)
#define UART_CTRL_RX_EN		(1 << 1)
#define UART_CTRL_TX_EN		(1 << 0)
#define UART_CTRL_MASK		(0x0000007F)

/* intclr */
#define UART_INTCLR_RX_OVR	(1 << 3)
#define UART_INTCLR_TX_OVR	(1 << 2)
#define UART_INTCLR_RX		(1 << 1)
#define UART_INTCLR_TX		(1 << 0)
#define UART_INTCLR_MASK	(0x0000000F)

/* bauddiv */
#define UART_BAUDDIV_MASK	(0x000FFFFF)

#endif /* __REG_UART_H__ */
