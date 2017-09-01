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
#include <linux/dma/rda_axidma.h>

#define RDA_AXIDMA_CHAN_NUM	12

//#define CONFIG_RDA_AXIDMA_DEBUG

#ifdef CONFIG_RDA_AXIDMA_DEBUG
#define dma_debug pr_info
#else
#define dma_debug(...) do{}while(0)
#endif

struct rda_dmadev;

static struct dma_slave_map rda_dma_slave_map[] = {
	{ "rda-dmatest", 	"rx-tx",(void *)RDA_DMA_CH0 },
	{ "rda-arm_uart.0",	"rx",	(void *)RDA_DMA_CH1 },
	{ "rda-arm_uart.0",	"tx",	(void *)RDA_DMA_CH2 },
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

struct rda_dma_cmn_reg {
	unsigned int conf;
	unsigned int delay;
};

/* dma channel */
struct rda_dma_chan {
	int id;
	const char *name;
	struct virt_dma_chan vc;
	struct rda_dma_ch_reg *regs;
	struct rda_dma_config config;
	struct rda_dma_txd *at;
	struct rda_dmadev *dev;
	enum rda_dma_ch_state state;
	bool forced;
	spinlock_t lock;
};

/* dma device */
struct rda_dmadev {
	spinlock_t lock;
	struct dma_device ddev;
	struct clk *dma_clk;
	void __iomem *base;
	struct rda_dma_reg *regs;
	struct rda_dma_cmn_reg *cmn_regs;
	unsigned int num_chan;
	struct rda_dma_chan *chan;
	int irq;
	spinlock_t irq_lock;
};

#define axidma_writel(val, reg) writel((unsigned int)(val), (volatile void __iomem *)&(reg))
#define axidma_readl(reg) readl((const volatile void __iomem *)&(reg))

#define RDA_AXIDMA_STATE_MASK 0x1FFFFF
static unsigned int axidma_ch_get_status(struct rda_dma_chan *c)
{
	return (axidma_readl(c->regs->status) & RDA_AXIDMA_STATE_MASK);
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
			dma_debug("%s, ch %d clear bit %x\n",__func__, ch, mask);

			if(ch_status & ARM_AXIDMA_COUNT_FINISH) {
				struct rda_dma_config *conf = &c->config;

				if(conf->handler)
					conf->handler(c->id, conf->data);
			}
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

#if 0
static struct rda_dmadev *to_rda_dma_dev(struct dma_device *d)
{
	return container_of(d, struct rda_dmadev, ddev);
}
#endif

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

static int rda_dma_slave_config(struct dma_chan *chan,
			struct dma_slave_config *config)
{
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	struct rda_dma_config *conf = to_rda_dma_config(config);
	unsigned long flags;

	dma_debug("%s, chan %p, config %p, c %p, conf %p\n",
			__func__, chan, config, c, conf);

	spin_lock_irqsave(&c->vc.lock, flags);

	c->config = *conf;

	spin_unlock_irqrestore(&c->vc.lock, flags);

	dma_debug("ie_stop: %d, ie_trans: %d, ie_finish: %d, forced: %d\n",
		c->config.ie_stop, c->config.ie_transmit, c->config.ie_finish,
		c->config.forced);

	return 0;
}

static struct rda_dma_txd *rda_dma_alloc_txd(void)
{
	struct rda_dma_txd *txd;

	txd = kzalloc(sizeof(struct rda_dma_txd), GFP_NOWAIT);
	if(txd) {
		INIT_LIST_HEAD(&txd->dsg_list);
	}
	return txd;
}

static void rda_dma_free_txd(struct rda_dma_txd *txd)
{
	kfree(txd);
}

static struct dma_async_tx_descriptor *rda_dma_prep_dma_memcpy(
					struct dma_chan *chan,
					dma_addr_t dst,
					dma_addr_t src,
					size_t len,
					unsigned long flags)
{
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	struct rda_dma_txd *txd;
	struct rda_dma_sg *dsg;

	txd = rda_dma_alloc_txd();
	if(!txd)
		return NULL;

	dsg = kzalloc(sizeof(struct rda_dma_sg), GFP_NOWAIT);
	if(!dsg) {
		rda_dma_free_txd(txd);
		return NULL;
	}
	list_add_tail(&dsg->node, &txd->dsg_list);

//	c->at = txd;

	dsg->src_addr = src;
	dsg->dst_addr = dst;
	dsg->len = len;

	txd->width = 8;// {8,16,32} bits

	dma_debug("%s, txd %p, dsg %p, src %x, dst %x, len %d, width %d\n",
		__func__, txd, dsg, dsg->src_addr,
		dsg->dst_addr, dsg->len, txd->width);

	return vchan_tx_prep(&c->vc, &txd->vd, flags);
}

struct dma_async_tx_descriptor *rda_dma_prep_slave_sg(
		struct dma_chan *chan, struct scatterlist *sgl,
		unsigned int sg_len, enum dma_transfer_direction direction,
		unsigned long flags, void *context)
{
	return NULL;
}

static int rda_dma_config_chan(struct rda_dma_chan *c, struct rda_dma_config *config)
{
	int requested = 1;
	unsigned int chan_id = config->cfg.slave_id;
	unsigned int map = (chan_id << 8) | chan_id;
	unsigned int conf;

	conf = axidma_readl(c->regs->conf);

	conf &= ~(ARM_AXIDMA_DATA_TYPE(0x3));
	switch(config->cfg.src_addr_width) {
	case DMA_SLAVE_BUSWIDTH_2_BYTES:
		conf |= ARM_AXIDMA_DATA_TYPE(1);// 16 bits
		break;
	case DMA_SLAVE_BUSWIDTH_4_BYTES:
		conf |= ARM_AXIDMA_DATA_TYPE(2);// 32 bits
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

	if(requested) {
		conf |= ARM_AXIDMA_SYN_IRQ;
	} else
		conf &= ~ARM_AXIDMA_SYN_IRQ;

	axidma_writel(conf, c->regs->conf);
	axidma_writel(map, c->regs->map);
	return 0;
}

static int rda_dma_config_transfer(struct rda_dma_chan *c, struct rda_dma_sg *dsg)
{
	unsigned int pcount;

	if(dsg->len < 256)
		pcount = 1;
	else if(dsg->len < 65535)
		pcount = 256;
	else
		pcount = 512;

	axidma_writel(pcount, c->regs->countp);
	axidma_writel(dsg->len, c->regs->count);
	axidma_writel(dsg->src_addr, c->regs->saddr);
	axidma_writel(dsg->dst_addr, c->regs->daddr);
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
	struct rda_dma_config *config = &c->config;

	list_del(&txd->vd.node);

	c->at = txd;

	txd->at = txd->dsg_list.next;

	dsg = list_entry(txd->at, struct rda_dma_sg, node);

	dma_debug("%s, c %p, config %p, txd %p, dsg %p, vd %p\n",
		__func__, c, config, txd, dsg, vd);

	rda_dma_config_chan(c, config);
	rda_dma_config_transfer(c, dsg);
	rda_dma_start_transfer(c, config);

	return 0;
}

static void rda_dma_issue_pending(struct dma_chan *chan)
{
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	unsigned long flags;

	dma_debug("%s, chan %p, rda chan %p\n",__func__,chan, c);

	spin_lock_irqsave(&c->vc.lock, flags);

	if(vchan_issue_pending(&c->vc)) {
		if(c->state != RDA_DMA_CH_RUNNING) {
			rda_dma_transmit(c);
			c->state = RDA_DMA_CH_RUNNING;
		}
	}
	spin_unlock_irqrestore(&c->vc.lock, flags);
}

#define CONFIG_RDA_DMA_HW_STATUS
static enum dma_status rda_dma_tx_status(struct dma_chan *chan,
				    dma_cookie_t cookie,
				    struct dma_tx_state *txstate)
{
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
#ifndef CONFIG_RDA_DMA_HW_STATUS
	struct rda_dma_txd *txd;
	struct rda_dma_sg *dsg;
	struct virt_dma_desc *vd;
#endif
	unsigned long flags;
	enum dma_status ret;
	size_t bytes = 0;

	dma_debug("%s\n",__func__);

	spin_lock_irqsave(&c->vc.lock, flags);
	ret = dma_cookie_status(chan, cookie, txstate);

	if(c->state == RDA_DMA_CH_RUNNING) {
		unsigned ch_status = axidma_ch_get_status(c);

		if((ch_status & ARM_AXIDMA_RUN) == 0) {
			ret = DMA_COMPLETE;
			dma_debug("status %x, tx complete\n", ch_status);
		}
	}
	if(ret == DMA_COMPLETE || !txstate) {
		spin_unlock_irqrestore(&c->vc.lock, flags);
		return ret;
	}
#ifndef CONFIG_RDA_DMA_HW_STATUS
	vd = vchan_find_desc(&c->vc, cookie);
	if(vd) {
		txd = to_rda_dma_txd(&vd->tx);
		list_for_each_entry(dsg, &txd->dsg_list, node)
			bytes += dsg->len;
	} else {
		txd = c->at;

		dsg = list_entry(txd->at, struct rda_dma_sg, node);
		list_for_each_entry_from(dsg, &txd->dsg_list, node)
			bytes += dsg->len;

		bytes += axidma_readl(c->regs->count);
	}
#else
	bytes += axidma_readl(c->regs->count);
#endif
	spin_unlock_irqrestore(&c->vc.lock, flags);

	dma_set_residue(txstate, bytes);
	dma_debug("residue %d bytes\n",bytes);

	return ret;
}

static int rda_dma_hard_sync(struct rda_dma_chan *c, unsigned int tm)
{
	unsigned int status;
	int cnt = tm / 20;

	while(1) {
		status = axidma_ch_get_status(c);
		if(!(status & ARM_AXIDMA_RUN))
			break;
		if(!cnt)
			return -EBUSY;
		msleep(20);
		cnt--;
	}
	c->state = RDA_DMA_CH_IDLE;
	dma_debug("%s, done\n",__func__);
	return 0;
}

static int rda_dma_terminate_all(struct dma_chan *chan)
{
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	unsigned long flags;
	int retval;

	spin_lock_irqsave(&c->vc.lock, flags);

	if(c->state != RDA_DMA_CH_IDLE) {
		unsigned int conf = axidma_readl(c->regs->conf);

		conf &= ~ARM_AXIDMA_START;
		axidma_writel(conf, c->regs->conf);
		axidma_writel(0, c->regs->count);
		axidma_writel(0, c->regs->countp);

		retval = rda_dma_hard_sync(c, 2000);
	}

	spin_unlock_irqrestore(&c->vc.lock, flags);

	dma_debug("%s, c %p, %s\n",__func__, c,
			(retval ? "failed" : "done"));
	return retval;
}

static void rda_dma_synchronize(struct dma_chan *chan)
{
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	unsigned long flags;
	int retval;

	spin_lock_irqsave(&c->vc.lock, flags);

	if(c->state == RDA_DMA_CH_RUNNING)
		retval = rda_dma_hard_sync(c, 2000);

	spin_unlock_irqrestore(&c->vc.lock, flags);
	dma_debug("%s, %s\n",__func__,
		(retval ? "failed" : "done"));
}

static int rda_dma_setup_regs(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(!res) {
		dev_err(&pdev->dev, "get res failed\n");
		return -EINVAL;
	}

	rda_dma->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(rda_dma->base)) {
		dev_err(&pdev->dev, "ioremap failed\n");
		return PTR_ERR(rda_dma->base);
	}
	rda_dma->regs = (struct rda_dma_reg *)(rda_dma->base);

	rda_dma->cmn_regs = devm_kzalloc(&pdev->dev,
				sizeof(struct rda_dma_cmn_reg), GFP_KERNEL);
	if(!rda_dma->cmn_regs) {
		dev_err(&pdev->dev, "alc cmn regs failed\n");
		return -ENOMEM;
	}
	dma_debug("%s, reg base %x, cmn regs %p\n", __func__,
		(unsigned int)(rda_dma->base), rda_dma->cmn_regs);
	return 0;
}

static void rda_dma_del_regs(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	devm_kfree(&pdev->dev, rda_dma->cmn_regs);
	devm_iounmap(&pdev->dev, rda_dma->base);
}

static bool rda_dma_filter_fn(struct dma_chan *chan, void *param);

#define RDA_DMA_BUSWIDTHS	(BIT(DMA_SLAVE_BUSWIDTH_1_BYTE) | \
				 BIT(DMA_SLAVE_BUSWIDTH_2_BYTES) | \
				 BIT(DMA_SLAVE_BUSWIDTH_4_BYTES))
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
	dma->device_issue_pending	= rda_dma_issue_pending;
	dma->device_tx_status		= rda_dma_tx_status;
	dma->device_terminate_all 	= rda_dma_terminate_all;
	dma->device_synchronize 	= rda_dma_synchronize;

	dma->filter.map = rda_dma_slave_map;
	dma->filter.mapcnt = ARRAY_SIZE(rda_dma_slave_map);
	dma->filter.fn = rda_dma_filter_fn;

	dma_debug("%s, done\n", __func__);
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
	int i;
	struct rda_dma_reg *regs;

	rda_dma->num_chan = RDA_AXIDMA_CHAN_NUM;

	rda_dma->chan = devm_kzalloc(&pdev->dev,
				sizeof(struct rda_dma_chan) * rda_dma->num_chan,
				GFP_KERNEL);
	if(!rda_dma->chan)
		return -ENOMEM;

	regs = rda_dma->regs;

	for(i = 0;i < rda_dma->num_chan;i++) {
		struct rda_dma_chan *ch = &rda_dma->chan[i];

		ch->id = i;
		ch->state = RDA_DMA_CH_IDLE;
		ch->regs = &regs->ch[i];
		ch->dev = rda_dma;
		spin_lock_init(&ch->lock);

		vchan_init(&ch->vc, &rda_dma->ddev);
		ch->vc.desc_free = rda_dma_desc_free;
		dma_debug("%s,init dma chan(%p): id=%d, state=%d, regs=%p\n",
				__func__, ch, ch->id, ch->state, ch->regs);
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
		dev_err(&pdev->dev, "failed to get IRQ: %d\n", irq);
		return -EINVAL;
	}
	rda_dma->irq = irq;

	rda_dma_irq_init(rda_dma);

	retval = devm_request_irq(&pdev->dev, irq, rda_dma_irq,
			      IRQF_SHARED, "rda_dma_irq", rda_dma);
	if (retval) {
		dev_err(&pdev->dev, "failed to request IRQ: %d\n", irq);
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

static void rda_dma_hw_init(struct platform_device *pdev,
				struct rda_dmadev *rda_dma)
{
	struct rda_dma_reg *regs = rda_dma->regs;
	unsigned long flags;

	spin_lock_irqsave(&rda_dma->lock, flags);

	axidma_writel(ARM_AXIDMA_STOP, regs->conf);
	msleep(2);// need delay ???

	if(axidma_readl(regs->status) & ARM_AXIDMA_STOP_STATUS)
		axidma_writel(ARM_AXIDMA_STOP_STATUS, regs->status);

	axidma_writel(ARM_AXIDMA_OUTSTAND(2), regs->conf);
	axidma_writel(0xffff, regs->delay);

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

	retval = dma_async_device_register(&rda_dma->ddev);
	if (retval) {
		goto fail_dma;
	}

	rda_dma_hw_init(pdev, rda_dma);

	platform_set_drvdata(pdev, rda_dma);
	dev_info(&pdev->dev, "done\n");
	return 0;
fail_dma:
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
	rda_dma_del_irq(pdev,rda_dma);
	rda_dma_del_chan(pdev,rda_dma);
	rda_dma_del_ddev(pdev,rda_dma);
	rda_dma_del_regs(pdev,rda_dma);
	devm_kfree(&pdev->dev, rda_dma);
	return 0;
}

static void rda_dma_save_regs(struct device *dev)
{
	struct rda_dmadev *rda_dma = dev_get_drvdata(dev);
	struct rda_dma_reg *regs = rda_dma->regs;
	struct rda_dma_cmn_reg *cmn_regs = rda_dma->cmn_regs;

	/* DMA channels' register don't need to be saved */
	cmn_regs->conf = axidma_readl(regs->conf);
	cmn_regs->delay = axidma_readl(regs->delay);
}

static void rda_dma_restore_regs(struct device *dev)
{
	struct rda_dmadev *rda_dma = dev_get_drvdata(dev);
	struct rda_dma_reg *regs = rda_dma->regs;
	struct rda_dma_cmn_reg *cmn_regs = rda_dma->cmn_regs;

	/* DMA channels' register don't need to be saved */
	cmn_regs->conf = axidma_readl(regs->conf);
	cmn_regs->delay = axidma_readl(regs->delay);
}

static int rda_dma_pm_suspend(struct device *dev)
{
	int ret;

	/* enable clock before accessing register */
	ret = pm_runtime_get_sync(dev);
	if(ret < 0) {
		dma_debug("%s, sync failed, %d\n", __func__, ret);
		return ret;
	}

	rda_dma_save_regs(dev);
	/* disable clock */
	pm_runtime_put(dev);
	dma_debug("%s, %s regs\n", __func__,
			(saved ? "saved" : "restored"));
	return ret;
}

static int rda_dma_pm_resume(struct device *dev)
{
	int ret;

	/* enable clock before accessing register */
	ret = pm_runtime_get_sync(dev);
	if(ret < 0) {
		dma_debug("%s, sync failed, %d\n", __func__, ret);
		return ret;
	}

	rda_dma_restore_regs(dev);
	/* disable clock */
	pm_runtime_put(dev);
	dma_debug("%s, %s regs\n", __func__,
			(saved ? "saved" : "restored"));
	return ret;
}

static const struct dev_pm_ops rda_dma_pm_ops = {
	SET_RUNTIME_PM_OPS(rda_dma_pm_suspend, rda_dma_pm_resume,
		NULL)
	SET_SYSTEM_SLEEP_PM_OPS(rda_dma_pm_suspend, rda_dma_pm_resume)
};

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
	if (chan->device->dev->driver == &rda_dma_driver.driver) {
		struct rda_dma_chan *c = to_rda_dma_chan(chan);
		struct rda_dmadev *dma = c->dev;
		unsigned req = *(unsigned *)param;

		if (req <= dma->num_chan) {
			c->id = req;
			return true;
		}
	}
	return false;
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("rda axidma driver");
MODULE_AUTHOR("daqingzhang <daqingzhang@rdamicro.com>");
