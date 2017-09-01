#ifndef _RDA_AXIDMA_H_
#define _RDA_AXIDMA_H_

#include <linux/interrupt.h>
#include <linux/dmaengine.h>

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
};

#endif /* _RDA_AXIDMA_H_ */
