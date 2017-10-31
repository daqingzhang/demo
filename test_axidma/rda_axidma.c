/*
 * RDA DMAengine support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/delay.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/of_dma.h>
#include <linux/of_device.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/pm_runtime.h>

#include "virt-dma.h"
#include <plat/regs/global_macros.h>
#include <plat/regs/arm_axidma.h>
#include <plat/rda_axidma.h>

#define RDA_AXIDMA_CHAN_NUM	12

//#define CONFIG_RDA_AXIDMA_DEBUG

#ifdef CONFIG_RDA_AXIDMA_DEBUG
#define dma_debug pr_info
#else
#define dma_debug(...) do{}while(0)
#endif

struct rda_dmadev;

static const struct dma_slave_map rda_dma_slave_map[] = {
	{ RDA_AXIDMA_DEV_NAME, RDA_AXIDMA_CH0_NAME, (void *)RDA_DMA_CH0 },
	{ RDA_AXIDMA_DEV_NAME, RDA_AXIDMA_CH1_NAME, (void *)RDA_DMA_CH1 },
	{ RDA_AXIDMA_DEV_NAME, RDA_AXIDMA_CH2_NAME, (void *)RDA_DMA_CH2 },
	{ RDA_AXIDMA_DEV_NAME, RDA_AXIDMA_CH3_NAME, (void *)RDA_DMA_CH3 },
	{ RDA_AXIDMA_DEV_NAME, RDA_AXIDMA_CH4_NAME, (void *)RDA_DMA_CH4 },
	{ RDA_AXIDMA_DEV_NAME, RDA_AXIDMA_CH5_NAME, (void *)RDA_DMA_CH5 },
	{ RDA_AXIDMA_DEV_NAME, RDA_AXIDMA_CH6_NAME, (void *)RDA_DMA_CH6 },
	{ RDA_AXIDMA_DEV_NAME, RDA_AXIDMA_CH7_NAME, (void *)RDA_DMA_CH7 },
	{ RDA_AXIDMA_DEV_NAME, RDA_AXIDMA_CH8_NAME, (void *)RDA_DMA_CH8 },
	{ RDA_AXIDMA_DEV_NAME, RDA_AXIDMA_CH9_NAME, (void *)RDA_DMA_CH9 },
	{ RDA_AXIDMA_DEV_NAME, RDA_AXIDMA_CH10_NAME, (void *)RDA_DMA_CH10 },
	{ RDA_AXIDMA_DEV_NAME, RDA_AXIDMA_CH11_NAME, (void *)RDA_DMA_CH11 },
};

/* dma channel state */
enum rda_dma_ch_state {
	RDA_DMA_CH_IDLE,
	RDA_DMA_CH_RUNNING,
	RDA_DMA_CH_PAUSED,
};

/* dma scater gather */
struct rda_dma_sg {
	dma_addr_t src_addr;
	dma_addr_t dst_addr;
	size_t len;
	struct list_head node;
};

/* dma transfer */
struct rda_dma_txd {
	struct virt_dma_desc vd;
	struct list_head dsg_list;
	struct list_head *at;
	u8 width;
	bool cyclic;
};

/* dma channel registers */
struct rda_dma_ch_reg {
	unsigned int conf;
	unsigned int map;
	unsigned int saddr;
	unsigned int daddr;
	unsigned int count;
	unsigned int countp;
	unsigned int status;
	unsigned int sgaddr;
	unsigned int sgconf;
	unsigned int set;
	unsigned int clr;
	unsigned int rsv[5];
};

/* dma registers */
struct rda_dma_reg {
	unsigned int conf;
	unsigned int delay;
	unsigned int status;
	unsigned int irq_status;
	unsigned int arm_req_status;
	unsigned int arm_ack_status;
	unsigned int zsp_req_status0;
	unsigned int zsp_req_status1;
	unsigned int rsv[8];
	struct rda_dma_ch_reg ch[RDA_AXIDMA_CHAN_NUM];
};

/* dma common registers (pm suspend or resume) */
struct rda_dma_cmn_reg {
	unsigned int conf;
	unsigned int delay;
};

/* dma channel common registers (dma pause or resume) */
struct rda_dma_ch_cmn_reg {
	unsigned int conf;
	unsigned int saddr;
	unsigned int daddr;
	unsigned int count;
	unsigned int countp;
};

#define RDA_AXIDMA_DSG_NR 3

/* dma channel */
struct rda_dma_chan {
	int id;
	enum rda_dma_ch_state state;
	struct rda_dma_ch_reg *regs;
	struct rda_dma_ch_cmn_reg cmn_regs;
	struct rda_dma_txd txd;
	struct rda_dma_sg dsg[RDA_AXIDMA_DSG_NR];
	struct rda_dmadev *dev;
	struct virt_dma_chan vc;
	struct dma_tx_state ts;
	struct rda_dma_config config;
	spinlock_t lock;
};

/* dma device */
struct rda_dmadev {
	spinlock_t lock;
	struct dma_device ddev;
	struct clk *dma_clk;
	void __iomem *base;
	struct rda_dma_reg *regs;
	struct rda_dma_cmn_reg cmn_regs;
	unsigned int num_chan;
	struct rda_dma_chan *chan;
	int irq;
	spinlock_t irq_lock;
};

#define axidma_writel(val, reg) writel((unsigned int)(val), (volatile void __iomem *)&(reg))
#define axidma_readl(reg) readl((const volatile void __iomem *)&(reg))

static unsigned int axidma_ch_get_status(struct rda_dma_chan *c)
{
#define CONFIG_RDA_AXIDMA_STATE_MSK 0x1fffff

	return (axidma_readl(c->regs->status) & CONFIG_RDA_AXIDMA_STATE_MSK);
}

static void axidma_ch_clear_status(struct rda_dma_chan *c, unsigned int mask)
{
	axidma_writel(mask, c->regs->status);
}

static void axidma_ch_irq_set(struct rda_dma_chan *c, int irq)
{
	unsigned int conf = axidma_readl(c->regs->conf);

	conf |= (irq & (ARM_AXIDMA_IRQ_T | ARM_AXIDMA_IRQ_F));

	axidma_writel(conf, c->regs->conf);
}

static unsigned int axidma_ch_get_count(struct rda_dma_chan *c)
{
	return axidma_readl(c->regs->count);
}

static irqreturn_t rda_dma_irq(int irq, void *devid)
{
	struct rda_dmadev *rda_dma = devid;
	unsigned int status, ch;

	spin_lock(&rda_dma->irq_lock);

	status = axidma_readl(rda_dma->regs->irq_status);

	for(ch = 0; ch < rda_dma->num_chan; ch++) {
		if((1 << ch) & status) {
			struct rda_dma_chan *c = &rda_dma->chan[ch];
			unsigned int ch_status = axidma_ch_get_status(c);
			unsigned int mask;

			mask = (ch_status & (ARM_AXIDMA_COUNTP_FINISH
						| ARM_AXIDMA_COUNT_FINISH
						| ARM_AXIDMA_SG_FINISH));
			axidma_ch_clear_status(c, mask);

			if(ch_status & (ARM_AXIDMA_COUNT_FINISH
					| ARM_AXIDMA_COUNTP_FINISH)) {
				struct rda_dma_config *conf = &c->config;

				if(conf->handler)
					conf->handler(c->id, conf->data);
			}
			if(ch_status & ARM_AXIDMA_COUNT_FINISH)
				c->state = RDA_DMA_CH_IDLE;
		}
	}
	spin_unlock(&rda_dma->irq_lock);
	return IRQ_HANDLED;
}

static struct rda_dma_chan *to_rda_dma_chan(struct dma_chan *c)
{
	return container_of(c, struct rda_dma_chan, vc.chan);
}

static struct rda_dma_config *to_rda_dma_config(struct dma_slave_config *c)
{
	return container_of(c, struct rda_dma_config, cfg);
}

struct rda_dma_txd *to_rda_dma_txd(struct dma_async_tx_descriptor *tx)
{
	return container_of(tx, struct rda_dma_txd, vd.tx);
}

/*
 * DMA Device Operations
 */

static int rda_dma_alloc_chan_resources(struct dma_chan *chan)
{
	dma_debug("%s, done\n",__func__);
	return 0;
}

static void rda_dma_free_chan_resources(struct dma_chan *chan)
{
	vchan_free_chan_resources(to_virt_chan(chan));

	dma_debug("%s, done\n",__func__);
}

static int rda_dma_config_chan(struct rda_dma_chan *c, struct rda_dma_config *config);

static int rda_dma_slave_config(struct dma_chan *chan,
			struct dma_slave_config *config)
{
	int ret = 0;
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	struct rda_dma_config *conf = to_rda_dma_config(config);
	unsigned long flags;

	dma_debug("%s, c(%p), chan(%p), conf(%p)\n", __func__, c, chan, conf);

	spin_lock_irqsave(&c->vc.lock, flags);

	if(!conf) {
		dma_debug("%s, conf is null\n", __func__);
		ret = -ENODEV;
		goto out;
	}
	c->config = *conf;
	rda_dma_config_chan(c, conf);

	dma_debug("ie [s=%d, t=%d, f=%d], forced=%d, src=%x, dst=%x, len=%d\n",
		conf->ie_stop, conf->ie_transmit, conf->ie_finish, conf->forced,
		config->src_addr, config->dst_addr, config->src_maxburst);
out:
	spin_unlock_irqrestore(&c->vc.lock, flags);
	return ret;
}

static struct rda_dma_sg *rda_dma_dsg_get(struct rda_dma_chan *ch)
{
	struct rda_dma_sg *dsg = &ch->dsg[0], *empty = NULL;
	int i;

	for(i = 0;i < RDA_AXIDMA_DSG_NR; i++) {
		if (dsg->len == 0) {
			empty = dsg;
			break;
		}
		dsg++;
	}
	return empty;
}

static void rda_dma_dsg_put(struct rda_dma_sg *dsg)
{
	dsg->len = 0;
}

static struct dma_async_tx_descriptor *rda_dma_prep_dma_memcpy(
					struct dma_chan *chan,
					dma_addr_t dst,
					dma_addr_t src,
					size_t len,
					unsigned long flags)
{
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	struct rda_dma_sg *dsg;
	struct rda_dma_txd *txd = &c->txd;

	dsg = rda_dma_dsg_get(c);
	if (!dsg) {
		dma_debug("%s, get dsg failed\n", __func__);
		return NULL;
	}
	dsg->src_addr = src;
	dsg->dst_addr = dst;
	dsg->len = len;

	list_add_tail(&dsg->node, &txd->dsg_list);

	dma_debug("%s, chan(%p), c(%p), txd(%p) "
		"dsg(%p), dst=%x, src=%x, len=%d\n",
		__func__, chan, c, txd, dsg, dst, src, len);

	return vchan_tx_prep(&c->vc, &txd->vd, flags);
}

struct dma_async_tx_descriptor *rda_dma_prep_slave_sg(
		struct dma_chan *chan, struct scatterlist *sgl,
		unsigned int sg_len, enum dma_transfer_direction direction,
		unsigned long flags, void *context)
{
	struct dma_async_tx_descriptor *tx = NULL;

	if (direction == DMA_DEV_TO_DEV) {
		dma_debug("%s, direction error\n", __func__);
		return NULL;
	} else {
		struct rda_dma_chan *c = to_rda_dma_chan(chan);
		struct dma_slave_config *cfg = &c->config.cfg;
		size_t len;

		if(cfg->src_maxburst < cfg->dst_maxburst)
			len = cfg->src_maxburst;
		else
			len = cfg->dst_maxburst;

		c->ts.used = len;
		c->ts.residue = len;
		c->ts.last = 0;
		tx = rda_dma_prep_dma_memcpy(chan, cfg->dst_addr, cfg->src_addr,
						len, flags);
	}
	return tx;
}

struct dma_async_tx_descriptor *rda_dma_prep_dma_sg(
		struct dma_chan *chan,
		struct scatterlist *dst_sg, unsigned int dst_nents,
		struct scatterlist *src_sg, unsigned int src_nents,
		unsigned long flags)
{
	struct dma_async_tx_descriptor *tx;
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	size_t len;
	dma_addr_t src, dst;

	if ((dst_sg != NULL) && (src_sg != NULL)) {
		dst = dst_sg[0].dma_address;
		src = src_sg[0].dma_address;
		if (dst_sg[0].length < src_sg[0].length)
			len = dst_sg[0].length;
		else
			len = src_sg[0].length;
	} else {
		struct dma_slave_config *cfg = &c->config.cfg;

		dst = cfg->dst_addr;
		src = cfg->src_addr;
		if(cfg->src_maxburst < cfg->dst_maxburst)
			len = cfg->src_maxburst;
		else
			len = cfg->dst_maxburst;
	}
	c->ts.used = len;
	c->ts.residue = len;
	c->ts.last = 0;

	tx = rda_dma_prep_dma_memcpy(chan, dst, src, len, flags);
	return tx;
}

#ifdef CONFIG_RDA_AXIDMA_DEBUG
static void rda_dma_dump_regs(struct rda_dma_chan *c)
{
	struct rda_dma_ch_reg *regs = c->regs;
	u32 t[6];

	t[0] = axidma_readl(regs->saddr);
	t[1] = axidma_readl(regs->daddr);
	t[2] = axidma_readl(regs->countp);
	t[3] = axidma_readl(regs->count);
	t[4] = axidma_readl(regs->conf);
	t[5] = axidma_readl(regs->map);

	dma_debug("saddr(%8x)  = %8x\n",(u32)&regs->saddr, t[0]);
	dma_debug("daddr(%8x)  = %8x\n",(u32)&regs->daddr, t[1]);
	dma_debug("countp(%8x) = %8x\n",(u32)&regs->countp,t[2]);
	dma_debug("count(%8x)  = %8x\n",(u32)&regs->count, t[3]);
	dma_debug("conf(%8x)   = %8x\n",(u32)&regs->conf,  t[4]);
	dma_debug("map(%8x)    = %8x\n",(u32)&regs->map,   t[5]);
}
#endif

static int rda_dma_config_chan(struct rda_dma_chan *c, struct rda_dma_config *config)
{
	int requested = 1;
	unsigned int map = (c->id << 8) | c->id;
	unsigned int conf;
	unsigned int delay = config->itv_cycles;
	struct rda_dma_reg *dma_regs = c->dev->regs;

	conf = axidma_readl(c->regs->conf);

	conf &= ~(ARM_AXIDMA_DATA_TYPE(0x3));
	switch(config->cfg.src_addr_width) {
	case DMA_SLAVE_BUSWIDTH_2_BYTES:
		conf |= ARM_AXIDMA_DATA_TYPE(1);// 16 bits
		break;
	case DMA_SLAVE_BUSWIDTH_4_BYTES:
		conf |= ARM_AXIDMA_DATA_TYPE(2);// 32 bits
		break;
	case DMA_SLAVE_BUSWIDTH_8_BYTES:
		conf |= ARM_AXIDMA_DATA_TYPE(3);// 64 bits
		break;
	case DMA_SLAVE_BUSWIDTH_1_BYTE:
		conf |= ARM_AXIDMA_DATA_TYPE(0);// 8 bits
	default:
		break;
	}

	switch(config->cfg.direction) {
	case DMA_MEM_TO_DEV:
		conf &= ~ARM_AXIDMA_SADDR_FIX;
		conf |= ARM_AXIDMA_DADDR_FIX;
		break;
	case DMA_DEV_TO_MEM:
		conf |= ARM_AXIDMA_SADDR_FIX;
		conf &= ~ARM_AXIDMA_DADDR_FIX;
		break;
	case DMA_MEM_TO_MEM:
	default:
		conf &= ~ARM_AXIDMA_SADDR_FIX;
		conf &= ~ARM_AXIDMA_DADDR_FIX;
		requested = 0;
		break;
	}

	if(config->ie_transmit)
		conf |= ARM_AXIDMA_IRQ_T;
	else
		conf &= ~ARM_AXIDMA_IRQ_T;

	if(config->ie_finish)
		conf |= ARM_AXIDMA_IRQ_F;
	else
		conf &= ~ARM_AXIDMA_IRQ_F;

	conf &= ~ARM_AXIDMA_REQ_SEL;

	if (requested)
		conf |= ARM_AXIDMA_SYN_IRQ;
	else
		conf &= ~ARM_AXIDMA_SYN_IRQ;

	if(delay)
		axidma_writel(delay, dma_regs->delay);

	axidma_writel(conf, c->regs->conf);
	axidma_writel(map, c->regs->map);
	dma_debug("config axidma reg: delay=%x, conf=%x, map=%x\n",
			delay, conf, map);
	return 0;
}

static int rda_dma_config_transfer(struct rda_dma_chan *c, struct rda_dma_sg *dsg)
{
	unsigned int pcount;

	pcount = dsg->len;

	if (dsg->len > 0xffff)
		pcount = 4096;

	dma_debug("%s, dsg(%p), src=%x, dst=%x, len=%d\n", __func__,
			dsg, dsg->src_addr, dsg->dst_addr, dsg->len);

	axidma_writel(dsg->src_addr, c->regs->saddr);
	axidma_writel(dsg->dst_addr, c->regs->daddr);
	axidma_writel(pcount, c->regs->countp);
	axidma_writel(dsg->len, c->regs->count);
	return 0;
}

static int rda_dma_start_transfer(struct rda_dma_chan *c, struct rda_dma_config *config)
{
	unsigned int conf;

	axidma_ch_clear_status(c, ARM_AXIDMA_COUNTP_FINISH
					| ARM_AXIDMA_COUNT_FINISH
					| ARM_AXIDMA_SG_FINISH);

	conf = axidma_readl(c->regs->conf);
	if(config->forced)
		conf |= ARM_AXIDMA_FORCE_TRANS;
	conf |= ARM_AXIDMA_START;

	axidma_writel(conf, c->regs->conf);
	return 0;
}

static int rda_dma_transmit(struct rda_dma_chan *c)
{
	struct virt_dma_desc *vd = vchan_next_desc(&c->vc);
	struct rda_dma_txd *txd = to_rda_dma_txd(&vd->tx);
	struct rda_dma_sg *dsg;

	list_del(&txd->vd.node);

	if (list_empty(&txd->dsg_list)) {
		dma_debug("%s, empty list\n", __func__);
		return 0;
	}

	dsg = list_last_entry(&txd->dsg_list, struct rda_dma_sg, node);
	list_del(&dsg->node);

	rda_dma_config_transfer(c, dsg);
#ifdef CONFIG_RDA_AXIDMA_DEBUG
	rda_dma_dump_regs(c);
#endif
	rda_dma_start_transfer(c, &c->config);
	rda_dma_dsg_put(dsg);

	dma_debug("%s, c(%p),txd(%p), dsg(%p), vd(%p)\n",
		__func__, c, txd, dsg, vd);
	return 0;
}

static void rda_dma_issue_pending(struct dma_chan *chan)
{
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	unsigned long flags;

	dma_debug("%s, c(%p) chan(%p)\n",__func__,c, chan);

	spin_lock_irqsave(&c->vc.lock, flags);

	if(vchan_issue_pending(&c->vc)) {
		if(c->state != RDA_DMA_CH_RUNNING) {
			rda_dma_transmit(c);
			c->state = RDA_DMA_CH_RUNNING;
		}
	}
	spin_unlock_irqrestore(&c->vc.lock, flags);
}

static enum dma_status rda_dma_tx_status(struct dma_chan *chan,
				    dma_cookie_t cookie,
				    struct dma_tx_state *txstate)
{
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	enum dma_status ret = DMA_ERROR;
	unsigned long flags;

	spin_lock_irqsave(&c->vc.lock, flags);

	if(c->state == RDA_DMA_CH_RUNNING) {
		unsigned ch_status = axidma_ch_get_status(c);
		size_t bytes = axidma_ch_get_count(c);

		if((ch_status & ARM_AXIDMA_RUN) || bytes)
			ret = DMA_IN_PROGRESS;
		else
			ret = DMA_COMPLETE;

		c->ts.residue = bytes;
		c->ts.last = c->ts.used - bytes;
		if(txstate)
			*txstate = c->ts;
		//dma_debug("%s, ch_status=%x\n", __func__, ch_status);
	}
	spin_unlock_irqrestore(&c->vc.lock, flags);
	return ret;
}

static int rda_dma_hard_sync(struct rda_dma_chan *c, unsigned int tm)
{
	unsigned int status, bytes;
	unsigned long timeout;

	timeout = jiffies + msecs_to_jiffies(tm);
	bytes = axidma_ch_get_count(c);
	status = axidma_ch_get_status(c);

	while ((status & ARM_AXIDMA_RUN) || bytes) {
		if(time_after(jiffies, timeout)) {
			dma_debug("%s, timeout\n", __func__);
			return -EBUSY;
		}
		bytes = axidma_ch_get_count(c);
		status = axidma_ch_get_status(c);
	}

	c->state = RDA_DMA_CH_IDLE;
	dma_debug("%s, done\n",__func__);
	return 0;
}

static void rda_dma_del_handler(struct rda_dma_chan *c)
{
	c->config.handler = NULL;
}

static void rda_dma_hard_stop(struct rda_dma_chan *c)
{
	unsigned int conf = axidma_readl(c->regs->conf);

	conf &= ~ARM_AXIDMA_START;
	axidma_writel(conf, c->regs->conf);
	axidma_writel(0, c->regs->count);
	axidma_writel(0, c->regs->countp);
}

static int rda_dma_terminate_all(struct dma_chan *chan)
{
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	unsigned long flags;

	spin_lock_irqsave(&c->vc.lock, flags);

	if(c->state != RDA_DMA_CH_IDLE) {

		rda_dma_hard_stop(c);
		rda_dma_del_handler(c);

		c->state = RDA_DMA_CH_IDLE;
	}

	spin_unlock_irqrestore(&c->vc.lock, flags);

	dma_debug("%s,done\n",__func__);
	return 0;
}

static void rda_dma_synchronize(struct dma_chan *chan)
{
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	unsigned long flags;
	int retval;

#define CONFIG_RDA_AXIDMA_TM 2000

	spin_lock_irqsave(&c->vc.lock, flags);

	if(c->state == RDA_DMA_CH_RUNNING)
		retval = rda_dma_hard_sync(c, CONFIG_RDA_AXIDMA_TM);

	spin_unlock_irqrestore(&c->vc.lock, flags);

	if(retval)
		pr_err("rda dma sync failed\n");
}

static int rda_dma_pause(struct dma_chan *chan)
{

	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	unsigned long flags;

	spin_lock_irqsave(&c->vc.lock, flags);
	if(c->state == RDA_DMA_CH_RUNNING) {
		struct rda_dma_ch_reg *regs = c->regs;
		struct rda_dma_ch_cmn_reg *cmn_regs = &c->cmn_regs;

		cmn_regs->saddr  = axidma_readl(regs->saddr);
		cmn_regs->daddr  = axidma_readl(regs->daddr);
		cmn_regs->count  = axidma_readl(regs->count);
		cmn_regs->countp = axidma_readl(regs->countp);
		cmn_regs->conf   = axidma_readl(regs->conf);

		rda_dma_hard_stop(c);

		c->state = RDA_DMA_CH_PAUSED;
	}
	spin_unlock_irqrestore(&c->vc.lock, flags);

	return 0;
}

static int rda_dma_resume(struct dma_chan *chan)
{
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	unsigned long flags;

	spin_lock_irqsave(&c->vc.lock, flags);
	if(c->state == RDA_DMA_CH_PAUSED) {
		struct rda_dma_ch_reg *regs = c->regs;
		struct rda_dma_ch_cmn_reg *cmn_regs = &c->cmn_regs;

		axidma_writel(cmn_regs->saddr, regs->saddr);
		axidma_writel(cmn_regs->daddr, regs->daddr);
		axidma_writel(cmn_regs->count, regs->count);
		axidma_writel(cmn_regs->countp, regs->countp);
		axidma_writel(cmn_regs->conf, regs->conf);

		c->state = RDA_DMA_CH_RUNNING;
	}
	spin_unlock_irqrestore(&c->vc.lock, flags);
	return 0;
}

static int rda_dma_setup_regs(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "get res failed\n");
		return -EINVAL;
	}

	rda_dma->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(rda_dma->base)) {
		dev_err(&pdev->dev, "ioremap failed\n");
		return PTR_ERR(rda_dma->base);
	}

	rda_dma->regs = (struct rda_dma_reg *)(rda_dma->base);

	dma_debug("%s, reg base %x\n", __func__, (u32)(rda_dma->base));
	return 0;
}

static void rda_dma_del_regs(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	devm_iounmap(&pdev->dev, rda_dma->base);
}

static bool rda_dma_filter_fn(struct dma_chan *chan, void *param);

#define RDA_DMA_BUSWIDTHS	(BIT(DMA_SLAVE_BUSWIDTH_1_BYTE) | \
				 BIT(DMA_SLAVE_BUSWIDTH_2_BYTES) | \
				 BIT(DMA_SLAVE_BUSWIDTH_4_BYTES) |	\
				 BIT(DMA_SLAVE_BUSWIDTH_8_BYTES))
#define RDA_DMA_DIR	(BIT(DMA_MEM_TO_DEV) | \
			 BIT(DMA_MEM_TO_MEM) | \
			 BIT(DMA_DEV_TO_MEM))

static int rda_dma_setup_ddev(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	struct dma_device *dma = &rda_dma->ddev;

	INIT_LIST_HEAD(&dma->channels);
	dma->src_addr_widths = RDA_DMA_BUSWIDTHS;
	dma->dst_addr_widths = RDA_DMA_BUSWIDTHS;
	dma->directions = RDA_DMA_DIR;
	dma->residue_granularity = DMA_RESIDUE_GRANULARITY_BURST;
	dma->dev = &pdev->dev;
	dma->dev_id = 1;

	dma_cap_set(DMA_MEMCPY,		dma->cap_mask);//mem2mem
	dma_cap_set(DMA_SLAVE,		dma->cap_mask);//dev2mem
	dma_cap_set(DMA_PRIVATE,	dma->cap_mask);//private

	dma->device_alloc_chan_resources= rda_dma_alloc_chan_resources;
	dma->device_free_chan_resources = rda_dma_free_chan_resources;
	dma->device_config 		= rda_dma_slave_config;
	dma->device_prep_dma_memcpy 	= rda_dma_prep_dma_memcpy;
	dma->device_prep_slave_sg	= rda_dma_prep_slave_sg;
	dma->device_prep_dma_sg		= rda_dma_prep_dma_sg;
	dma->device_issue_pending	= rda_dma_issue_pending;
	dma->device_tx_status		= rda_dma_tx_status;
	dma->device_terminate_all 	= rda_dma_terminate_all;
	dma->device_synchronize 	= rda_dma_synchronize;
	dma->device_pause		= rda_dma_pause;
	dma->device_resume		= rda_dma_resume;

	dma->filter.map = rda_dma_slave_map;
	dma->filter.mapcnt = ARRAY_SIZE(rda_dma_slave_map);
	dma->filter.fn = rda_dma_filter_fn;

	dma_debug("%s, dma(%p) done\n", __func__, dma);
	return 0;
}

static void rda_dma_del_ddev(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	dma_debug("%s, done\n",__func__);
}

static void rda_dma_desc_free(struct virt_dma_desc *vd)
{
	dma_debug("%s, done\n",__func__);
}

static int rda_dma_setup_chan(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	int i = RDA_AXIDMA_CHAN_NUM;
	struct rda_dma_chan *ch;
	struct rda_dma_reg *regs;

	rda_dma->chan = devm_kzalloc(&pdev->dev,
			sizeof(struct rda_dma_chan) * i, GFP_KERNEL);
	if(!rda_dma->chan)
		return -ENOMEM;

	rda_dma->num_chan = i;
	regs = rda_dma->regs;

	while (i) {
		ch = &rda_dma->chan[--i];
		ch->id = i;
		ch->state = RDA_DMA_CH_IDLE;
		ch->regs = &regs->ch[i];
		ch->dev = rda_dma;
		spin_lock_init(&ch->lock);
		vchan_init(&ch->vc, &rda_dma->ddev);
		ch->vc.desc_free = rda_dma_desc_free;
		INIT_LIST_HEAD(&ch->txd.dsg_list);

		dma_debug("%s, dma chan[%d] at %p, state=%d, regs(%p)\n",
			__func__, ch->id, ch, ch->state, ch->regs);
	}
	return 0;
}

static void rda_dma_del_chan(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	devm_kfree(&pdev->dev,rda_dma->chan);
}

static void rda_dma_irq_init(struct rda_dmadev *rda_dma)
{
	struct rda_dma_chan *c = rda_dma->chan;
	int i;
	unsigned long flags;

	spin_lock_irqsave(&rda_dma->lock, flags);

	for(i = 0;i < rda_dma->num_chan;i++)
		axidma_ch_irq_set(&c[i], 0);

	spin_unlock_irqrestore(&rda_dma->lock, flags);
}

static int rda_dma_setup_irq(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	int irq,retval;

	irq = platform_get_irq(pdev, 0);
	if (irq <= 0) {
		dev_err(&pdev->dev, "get IRQ failed, %d\n", irq);
		return -EINVAL;
	}
	rda_dma->irq = irq;

	rda_dma_irq_init(rda_dma);

	retval = devm_request_irq(&pdev->dev, irq, rda_dma_irq,
			      IRQF_SHARED, "rda_dma_irq", rda_dma);
	if (retval) {
		dev_err(&pdev->dev, "request IRQ failed, %d\n", irq);
		return retval;
	}
	dma_debug("%s, irq = %d\n", __func__, irq);
	return 0;
}

static void rda_dma_del_irq(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	devm_free_irq(&pdev->dev,rda_dma->irq, NULL);
}

static int rda_dma_setup_clk(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	int ret = 0;
	struct clk *ck;

	ck = clk_get(&pdev->dev, RDA_AXIDMA_CLK_NAME);
	if (IS_ERR(ck)) {
		//ret = PTR_ERR(ck);
		dev_err(&pdev->dev, "get dma clk failed\n");
		return ret;
	}

	clk_prepare(ck);

	ret = clk_enable(ck);
	if (ret) {
		dev_err(&pdev->dev, "enable clk failed\n");
		return ret;
	}
	rda_dma->dma_clk = ck;
	dma_debug("%s, clk = %p\n", __func__, ck);
	return 0;
}

static void rda_dma_del_clk(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	struct clk *ck = rda_dma->dma_clk;
	if (ck) {
		clk_disable(ck);
		clk_put(ck);
	}
}

static void rda_dma_hw_init(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	struct rda_dma_reg *regs = rda_dma->regs;
	unsigned long flags;

	spin_lock_irqsave(&rda_dma->lock, flags);

	axidma_writel(ARM_AXIDMA_STOP, regs->conf);

	if(axidma_readl(regs->status) & ARM_AXIDMA_STOP_STATUS)
		axidma_writel(ARM_AXIDMA_STOP_STATUS, regs->status);

	axidma_writel(ARM_AXIDMA_OUTSTAND(2), regs->conf);
	axidma_writel(0x0001, regs->delay);
	spin_unlock_irqrestore(&rda_dma->lock, flags);
}

static int rda_dma_probe(struct platform_device *pdev)
{
	struct rda_dmadev *rda_dma;
	int retval;

	dma_debug("%s, start\n", __func__);

	rda_dma = devm_kzalloc(&pdev->dev,
			sizeof(struct rda_dmadev), GFP_KERNEL);
	if(!rda_dma) {
		dev_err(&pdev->dev, "alloc dma failed\n");
		return -ENOMEM;
	}

	spin_lock_init(&rda_dma->lock);
	spin_lock_init(&rda_dma->irq_lock);

	retval = rda_dma_setup_regs(pdev, rda_dma);
	if(retval) {
		dev_err(&pdev->dev, "setup regs failed\n");
		goto fail_reg;
	}

	retval = rda_dma_setup_ddev(pdev, rda_dma);
	if(retval) {
		dev_err(&pdev->dev, "setup ddev failed\n");
		goto fail_dev;
	}

	retval = rda_dma_setup_chan(pdev, rda_dma);
	if(retval) {
		dev_err(&pdev->dev, "setup chan failed\n");
		goto fail_chan;
	}

	retval = rda_dma_setup_irq(pdev, rda_dma);
	if(retval) {
		dev_err(&pdev->dev, "setup irq failed\n");
		goto fail_irq;
	}

	retval = rda_dma_setup_clk(pdev, rda_dma);
	if(retval) {
		dev_err(&pdev->dev, "setup clk failed\n");
		goto fail_clk;
	}

	retval = dma_async_device_register(&rda_dma->ddev);
	if (retval) {
		goto fail_dma;
	}

	rda_dma_hw_init(pdev, rda_dma);

	platform_set_drvdata(pdev, rda_dma);
	dev_info(&pdev->dev, "done\n");
	return 0;
fail_dma:
	rda_dma_del_clk(pdev, rda_dma);
fail_clk:
	rda_dma_del_irq(pdev,rda_dma);
fail_irq:
	rda_dma_del_chan(pdev,rda_dma);
fail_chan:
	rda_dma_del_ddev(pdev,rda_dma);
fail_dev:
	rda_dma_del_regs(pdev,rda_dma);
fail_reg:
	devm_kfree(&pdev->dev, rda_dma);
	return retval;
}

static int rda_dma_remove(struct platform_device *pdev)
{
	struct rda_dmadev *rda_dma = platform_get_drvdata(pdev);

	dma_async_device_unregister(&rda_dma->ddev);
	rda_dma_del_clk(pdev, rda_dma);
	rda_dma_del_irq(pdev,rda_dma);
	rda_dma_del_chan(pdev,rda_dma);
	rda_dma_del_ddev(pdev,rda_dma);
	rda_dma_del_regs(pdev,rda_dma);
	devm_kfree(&pdev->dev, rda_dma);
	return 0;
}

static int rda_dma_pm_suspend(struct device *dev)
{
	int ret = 0;
	struct rda_dmadev *rda_dma = dev_get_drvdata(dev);
	struct rda_dma_reg *regs = rda_dma->regs;
	struct rda_dma_cmn_reg *cmn_regs = &rda_dma->cmn_regs;

	cmn_regs->conf = axidma_readl(regs->conf);
	cmn_regs->delay = axidma_readl(regs->delay);

	clk_disable(rda_dma->dma_clk);

	dma_debug("%s\n", __func__);
	return ret;
}

static int rda_dma_pm_resume(struct device *dev)
{
	int ret = 0;

	struct rda_dmadev *rda_dma = dev_get_drvdata(dev);
	struct rda_dma_reg *regs = rda_dma->regs;
	struct rda_dma_cmn_reg *cmn_regs = &rda_dma->cmn_regs;

	ret = clk_enable(rda_dma->dma_clk);
	if (ret) {
		dev_err(dev, "%s, enable clk failed\n", __func__);
		return ret;
	}

	axidma_writel(cmn_regs->delay, regs->delay);
	axidma_writel(cmn_regs->conf, regs->conf);

	dma_debug("%s\n", __func__);
	return ret;
}

static SIMPLE_DEV_PM_OPS(rda_dma_pm_ops, rda_dma_pm_suspend, rda_dma_pm_resume);

static const struct of_device_id rda_dma_match[] = {
	{ .compatible = "rda,rda-axidma", },
	{},
};

static struct platform_driver rda_dma_driver = {
	.probe = rda_dma_probe,
	.remove = rda_dma_remove,
	.driver = {
		.name = "rda-axidma",
		.owner = THIS_MODULE,
		.of_match_table = rda_dma_match,
		.pm = &rda_dma_pm_ops,
	},
};

static int rda_dma_init(void)
{
	return platform_driver_register(&rda_dma_driver);
}
subsys_initcall(rda_dma_init);

static void __exit rda_dma_exit(void)
{
	platform_driver_unregister(&rda_dma_driver);
}
module_exit(rda_dma_exit);

static bool rda_dma_filter_fn(struct dma_chan *chan, void *param)
{
	dma_debug("%s, chan(%p), param = %d\n", __func__, chan, (int)param);

	if (chan->device->dev->driver == &rda_dma_driver.driver) {
		struct rda_dma_chan *c = to_rda_dma_chan(chan);
		struct rda_dmadev *dma = c->dev;
		unsigned req = (unsigned)param;

		dma_debug("%s, c(%p), dma(%p), req = %d\n",
			__func__, c, dma, req);

		if (req <= dma->num_chan)
			return true;
	}
	return false;
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("rda axidma driver");
MODULE_AUTHOR("daqingzhang <daqingzhang@rdamicro.com>");
