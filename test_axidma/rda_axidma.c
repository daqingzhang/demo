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

const static struct dma_slave_map rda_dma_slave_map[] = {
	{ "8816000.rda-uart1",	"rx",	(void *)RDA_DMA_CH0 },
	{ "8816000.rda-uart1",	"tx",	(void *)RDA_DMA_CH1 },
	{ "8817000.rda-uart2",	"rx",	(void *)RDA_DMA_CH2 },
	{ "8817000.rda-uart2",	"tx",	(void *)RDA_DMA_CH3 },
	{ "8818000.rda-uart3",	"rx",	(void *)RDA_DMA_CH4 },
	{ "8818000.rda-uart3",	"tx",	(void *)RDA_DMA_CH5 },
	{ "dmahost.1", 		"rxtx",	(void *)RDA_DMA_CH11 },
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

/* dma channel */
struct rda_dma_chan {
	int id;
	enum rda_dma_ch_state state;
	struct rda_dma_ch_reg *regs;
	struct rda_dma_ch_cmn_reg *cmn_regs;
	struct rda_dma_txd *at;
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
	struct rda_dma_cmn_reg *cmn_regs;
	unsigned int num_chan;
	struct rda_dma_chan *chan;
	int irq;
	spinlock_t irq_lock;
};

#define axidma_writel(val, reg) writel((unsigned int)(val), (volatile void __iomem *)&(reg))
#define axidma_readl(reg) readl((const volatile void __iomem *)&(reg))

static unsigned int axidma_ch_get_status(struct rda_dma_chan *c)
{
	return (axidma_readl(c->regs->status) & 0x1fffff);
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

static int rda_dma_slave_config(struct dma_chan *chan,
			struct dma_slave_config *config)
{
	int ret = 0;
	struct rda_dma_chan *c = to_rda_dma_chan(chan);
	struct rda_dma_config *conf = to_rda_dma_config(config);
	unsigned long flags;

	dma_debug("%s, c(%p), chan(%p), conf(%p),config(%p)\n",
			__func__, c, chan, conf, config);

	spin_lock_irqsave(&c->vc.lock, flags);

	if(!conf) {
		ret = -ENODEV;
		goto out;
	}
	c->config = *conf;

	dma_debug("ie [s=%d, t=%d, f=%d], forced=%d, src=%x, dst=%x, len=%d\n",
		conf->ie_stop, conf->ie_transmit, conf->ie_finish, conf->forced,
		config->src_addr, config->dst_addr, config->src_maxburst);
out:
	spin_unlock_irqrestore(&c->vc.lock, flags);
	return ret;
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

	if(!c->at) {
		txd = rda_dma_alloc_txd();
		if(!txd)
			return NULL;

		dsg = kzalloc(sizeof(struct rda_dma_sg), GFP_NOWAIT);
		if(!dsg) {
			rda_dma_free_txd(txd);
			return NULL;
		}
		list_add_tail(&dsg->node, &txd->dsg_list);
		c->at = txd;
	}
	txd = c->at;

	dsg = list_last_entry(&txd->dsg_list, struct rda_dma_sg, node);

	dsg->src_addr = src;
	dsg->dst_addr = dst;
	dsg->len = len;

	txd->width  = 8;
	txd->cyclic = false;

	dma_debug("%s, txd(%p), dsg(%p), src=%x, dst=%x, len=%d\n",
		__func__, txd, dsg, src, dst, len);

	return vchan_tx_prep(&c->vc, &txd->vd, flags);
}

struct dma_async_tx_descriptor *rda_dma_prep_slave_sg(
		struct dma_chan *chan, struct scatterlist *sgl,
		unsigned int sg_len, enum dma_transfer_direction direction,
		unsigned long flags, void *context)
{
	struct dma_async_tx_descriptor *tx = NULL;

	if(direction == DMA_DEV_TO_DEV) {
		dma_debug("%s, not support\n", __func__);
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

static void rda_dma_dump_regs(struct rda_dma_chan *c)
{
#ifdef CONFIG_RDA_AXIDMA_DEBUG
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
#endif
}

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

	if(delay)
		axidma_writel(delay, dma_regs->delay);

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
		pcount = 32;
	else
		pcount = 64;

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
	struct rda_dma_config *config = &c->config;

	list_del(&txd->vd.node);

	txd->at = txd->dsg_list.next;

	dsg = list_entry(txd->at, struct rda_dma_sg, node);

	dma_debug("%s, c %p, config %p, txd %p, dsg %p, vd %p\n",
		__func__, c, config, txd, dsg, vd);

	rda_dma_config_chan(c, config);
	rda_dma_config_transfer(c, dsg);
	rda_dma_dump_regs(c);
	rda_dma_start_transfer(c, config);
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

	spin_lock_irqsave(&c->vc.lock, flags);

	if(c->state == RDA_DMA_CH_RUNNING)
		retval = rda_dma_hard_sync(c, 2000);

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
		struct rda_dma_ch_cmn_reg *cmn_regs = c->cmn_regs;

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
		struct rda_dma_ch_cmn_reg *cmn_regs = c->cmn_regs;

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
	int i;
	struct rda_dma_reg *regs;
	struct rda_dma_ch_cmn_reg *cmn_regs;

	rda_dma->num_chan = RDA_AXIDMA_CHAN_NUM;

	rda_dma->chan = devm_kzalloc(&pdev->dev,
				sizeof(struct rda_dma_chan) * rda_dma->num_chan,
				GFP_KERNEL);
	if(!rda_dma->chan)
		return -ENOMEM;

	cmn_regs = devm_kzalloc(&pdev->dev,
			sizeof(struct rda_dma_ch_cmn_reg) * rda_dma->num_chan,
			GFP_KERNEL);
	if(!cmn_regs)
		return -ENOMEM;

	regs = rda_dma->regs;

	for(i = 0;i < rda_dma->num_chan;i++) {
		struct rda_dma_chan *c = &rda_dma->chan[i];

		c->id = i;
		c->state = RDA_DMA_CH_IDLE;
		c->regs = &regs->ch[i];
		c->cmn_regs = &cmn_regs[i];
		c->dev = rda_dma;
		spin_lock_init(&c->lock);

		vchan_init(&c->vc, &rda_dma->ddev);
		c->vc.desc_free = rda_dma_desc_free;
		dma_debug("%s, dma chan[%d] at %p, state=%d, regs(%p)\n",
			__func__, c->id, c, c->state, c->regs);
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

	axidma_writel(cmn_regs->delay, regs->delay);
	axidma_writel(cmn_regs->conf, regs->conf);
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

	dev_info(dev, "%s saved\n", __func__);
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

	dev_info(dev, "%s restored\n", __func__);
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
