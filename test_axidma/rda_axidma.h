#ifndef _RDA_AXIDMA_H_
#define _RDA_AXIDMA_H_

#include <linux/interrupt.h>
#include <linux/dmaengine.h>

#define RDA_AXIDMA_CLK_NAME "sys_ahb"
#define RDA_AXIDMA_DEV_NAME "90c0000.rda-axidma"

#define RDA_DMA_CH0_NAME  "c0"
#define RDA_DMA_CH1_NAME  "c1"
#define RDA_DMA_CH2_NAME  "c2"
#define RDA_DMA_CH3_NAME  "c3"
#define RDA_DMA_CH4_NAME  "c4"
#define RDA_DMA_CH5_NAME  "c5"
#define RDA_DMA_CH6_NAME  "c6"
#define RDA_DMA_CH7_NAME  "c7"
#define RDA_DMA_CH8_NAME  "c8"
#define RDA_DMA_CH9_NAME  "c9"
#define RDA_DMA_CH10_NAME "c10"
#define RDA_DMA_CH11_NAME "c11"

enum rda_dma_ch_id {
	RDA_DMA_CH0,
	RDA_DMA_CH1,
	RDA_DMA_CH2,
	RDA_DMA_CH3,
	RDA_DMA_CH4,
	RDA_DMA_CH5,
	RDA_DMA_CH6,
	RDA_DMA_CH7,
	RDA_DMA_CH8,
	RDA_DMA_CH9,
	RDA_DMA_CH10,
	RDA_DMA_CH11,
};

struct rda_dma_config {
	struct dma_slave_config cfg;
	bool ie_stop;	  // ie for transfer stopped
	bool ie_finish;	  // ie for channel's transfer accomplished
	bool ie_transmit; // ie for data transmited
	bool forced;
	irq_handler_t handler;
	void *data;
	unsigned short itv_cycles;//interval cycles number [0,0xffff]
};

#endif /* _RDA_AXIDMA_H_ */
