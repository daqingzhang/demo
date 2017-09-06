#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/dmaengine.h>
#include <linux/dma/rda_axidma.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/completion.h>
#include <linux/io.h>
#include <asm-generic/cacheflush.h>

/*
 * DMA Host Device
 */

#define CONFIG_DH_MEM_SIZE	(240 * 1024)	//240KB
#define CONFIG_DH_MEM_START	(0x00800100)
#define CONFIG_DH_MEM_END	(CONFIG_DH_MEM_START + CONFIG_DH_MEM_SIZE - 1)

struct resource dmahost_resource[] = {
	{
		.start = CONFIG_DH_MEM_START,
		.end   = CONFIG_DH_MEM_END,
		.flags = IORESOURCE_MEM,
	},
	{
		.start = 0x11,
		.end   = 0x11,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device dmahost_device = {
	.name = "dmahost",
	.id   = 1,
	.resource = dmahost_resource,
	.num_resources = 2,
};

static int __init dmahost_dev_init(void)
{
	int retval;

	retval = platform_device_register(&dmahost_device);
	if(retval) {
		pr_err("%s, register dmahost failed, %d\n",
			__func__, retval);
		return retval;
	}
	pr_info("%s done\n", __func__);
	return retval;
}

arch_initcall(dmahost_dev_init);

/*
 * DMA Host Driver
 */

static int dmahost_buf_set(void *src, void *dst, unsigned len, int cmd);

#define CONFIG_DMAHOST_BUF_SIZE (256)
#define DMAHOST_WAIT_TM 5000

struct dmahost_dev {
	struct	device *dev;
	struct	dma_chan *chan;
	struct	dma_async_tx_descriptor *tx;
	struct	rda_dma_config config;
	struct	completion comp;
};

static irqreturn_t dmahost_irq_handler(int id, void *data)
{
	struct dmahost_dev *dma_host = data;
	struct dma_chan *chan = dma_host->chan;
	enum dma_status state;
	dma_cookie_t last = 0, used = 0;

	state = dma_async_is_tx_complete(chan, 0, &last, &used);
	if(state == DMA_COMPLETE) {
		complete(&dma_host->comp);
		//pr_info("%s, last=%d, used=%d\n", __func__, last, used);
	}
	return IRQ_HANDLED;
}

void *dmahost_setup_mem(struct platform_device *pdev)
{
	struct resource *res;
	void *virt_addr;
//	void __iomem *virt_addr;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(!res) {
		dev_err(&pdev->dev, "%s, get res failed\n", __func__);
		return NULL;
	}
#if 0
	virt_addr = devm_ioremap_resource(&pdev->dev, res);
//	virt_addr = devm_ioremap(&pdev->dev, res->start, CONFIG_DH_MEM_SIZE);
#else
	virt_addr = devm_memremap(&pdev->dev, res->start,
			CONFIG_DH_MEM_SIZE, MEMREMAP_WB);
#endif
	dev_info(&pdev->dev, "%s, start=%x, vir_addr=%x\n",
			__func__, (u32)(res->start), (u32)virt_addr);

	return virt_addr;
}

void *dmahost_setup_buf(struct platform_device *pdev, int len)
{
	void *buf_addr;

	buf_addr = devm_kzalloc(&pdev->dev, sizeof(char) * len, GFP_KERNEL);

	dev_info(&pdev->dev, "%s, buf_addr=%x\n", __func__, (u32)buf_addr);

	return buf_addr;
}

struct device *dmahost_find_device(const char *name)
{
	struct device *dev;

	dev = bus_find_device_by_name(&platform_bus_type, NULL, "dmahost.1");
	pr_info("dev(%p)\n",dev);

	dev = bus_find_device_by_name(&platform_bus_type, NULL, "8816000.rda-uart1");
	pr_info("dev(%p)\n",dev);

	dev = bus_find_device_by_name(&platform_bus_type, NULL, "8817000.rda-uart2");
	pr_info("dev(%p)\n",dev);

	return dev;
}

//#define CONFIG_DMA_PAUSE
#define CONFIG_DMA_PERF

#ifdef CONFIG_DMA_PERF
extern cycle_t rda_hwtimer_read(struct clocksource *cs);
#define get_hwtick(a) rda_hwtimer_read(0)
#endif

static int dmahost_probe(struct platform_device *pdev)
{
	int retval;
	struct dmahost_dev *dh;
	struct dma_chan *chan;
	struct rda_dma_config *conf;
	dma_addr_t src_paddr, dst_paddr;
	u8 *src_addr, *dst_addr, *pt;
	u32 bufsize;
	u32 wait;

#ifdef CONFIG_DMA_PERF
	u32 tick1,tick2,tick3;
#endif
	pr_info("%s, ************ test 1\n", __func__);

	dh = devm_kzalloc(&pdev->dev, sizeof(struct dmahost_dev), GFP_KERNEL);
	if(!dh) {
		pr_err("%s, alloc dmahost data failed\n", __func__);
		return -ENOMEM;
	}
#ifndef CONFIG_DMA_PERF
	src_addr = dmahost_setup_buf(pdev, CONFIG_DMAHOST_BUF_SIZE);
	dst_addr = dmahost_setup_buf(pdev, CONFIG_DMAHOST_BUF_SIZE);
	bufsize  = CONFIG_DMAHOST_BUF_SIZE;

	dmahost_buf_set(src_addr, dst_addr, bufsize, 0);// clear buffer
	dmahost_buf_set(src_addr, dst_addr, bufsize, 1);// fill buffer

	flush_cache_all();//flush cache for next test
#else
	/*
	 * memcpy test - to test time costing of memcpy
	 * dst: SRAM memory space, cached, buffered
	 * src: DRAM memosy space
	 * dst <==> src
	 */

	dst_addr = dmahost_setup_mem(pdev);
	src_addr = dmahost_setup_buf(pdev, CONFIG_DH_MEM_SIZE);
	bufsize  = CONFIG_DH_MEM_SIZE;

	dmahost_buf_set(src_addr, 0, bufsize, 0); //clear src buffer
	dmahost_buf_set(dst_addr, 0, bufsize, 0); //clear dst buffer
	dmahost_buf_set(src_addr, 0, bufsize, 1); //fill data to src buffer

	/* disable IRQ */
	local_irq_disable();

	/* DRAM ==> SRAM */
	bufsize = 180 * 0x400;
	tick1 = get_hwtick(0);
	memcpy(dst_addr, src_addr, bufsize);
	tick3 = get_hwtick(0);
	flush_cache_all();
	tick2 = get_hwtick(0);
	pr_info("memcpy, from DRAM to SRAM %d KB, cost %d ticks\n", (bufsize/0x400), tick2 - tick1);
	pr_info("flush_cache_all cost %d ticks\n", tick2 - tick3);

	/* DRAM <== SRAM */
	tick1 = get_hwtick(0);
	memcpy(src_addr, dst_addr, bufsize);
	tick3 = get_hwtick(0);
	flush_cache_all();
	tick2 = get_hwtick(0);
	pr_info("memcpy, from SRAM to DRAM %d KB, cost %d ticks\n", (bufsize/0x400), tick2 - tick1);
	pr_info("flush_cache_all cost %d ticks\n", tick2 - tick3);

	/* DRAM ==> SRAM */
	bufsize = 200 * 0x400;
	tick1 = get_hwtick(0);
	memcpy(dst_addr, src_addr, bufsize);
	flush_cache_all();
	tick2 = get_hwtick(0);
	pr_info("memcpy, from DRAM to SRAM %d KB, cost %d ticks\n", (bufsize/0x400), tick2 - tick1);

	/* DRAM <== SRAM */
	tick1 = get_hwtick(0);
	memcpy(src_addr, dst_addr, bufsize);
	flush_cache_all();
	tick2 = get_hwtick(0);
	pr_info("memcpy, from SRAM to DRAM %d KB, cost %d ticks\n", (bufsize/0x400), tick2 - tick1);

	/* DRAM ==> SRAM */
	bufsize = 220 * 0x400;
	tick1 = get_hwtick(0);
	memcpy(dst_addr, src_addr, bufsize);
	flush_cache_all();
	tick2 = get_hwtick(0);
	pr_info("memcpy, from DRAM to SRAM %d KB, cost %d ticks\n", (bufsize/0x400), tick2 - tick1);

	/* DRAM <== SRAM */
	tick1 = get_hwtick(0);
	memcpy(src_addr, dst_addr, bufsize);
	flush_cache_all();
	tick2 = get_hwtick(0);
	pr_info("memcpy, from SRAM to DRAM %d KB, cost %d ticks\n", (bufsize/0x400), tick2 - tick1);

	/* enable IRQ */
	local_irq_enable();

	dmahost_buf_set(src_addr, dst_addr, bufsize, 2);
	flush_cache_all();

	bufsize = 200 * 0x400;
	pr_info("%s, memory copy test done\n", __func__);
#endif
	/* retrive a dma channel */
	chan = dma_request_chan(&pdev->dev, "rxtx");
	if(IS_ERR(chan)) {
		pr_err("%s, request chan failed\n", __func__);
		retval = PTR_ERR(chan);
		return retval;
	}
	pr_info("%s, chan = %p\n", __func__, chan);

	/* remap buffer's memory space for dma device */
	src_paddr = dma_map_single(&pdev->dev, (void *)src_addr,
						bufsize, DMA_FROM_DEVICE);
	dst_paddr = dma_map_single(&pdev->dev, (void *)dst_addr,
						bufsize, DMA_TO_DEVICE);

	pr_info("%s, src_paddr(%p), dst_paddr(%p)\n",
			__func__, src_paddr, dst_paddr);
	dh->chan = chan;
	dh->dev = &pdev->dev;

	/* configure dma channel */
	conf = &dh->config;

	conf->cfg.direction = DMA_MEM_TO_MEM;
	conf->cfg.src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
	conf->cfg.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
//	conf->cfg.src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
//	conf->cfg.dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
	conf->cfg.src_maxburst = bufsize;
	conf->cfg.dst_maxburst = bufsize;
//	conf->cfg.src_addr = src_paddr;
//	conf->cfg.dst_addr = dst_paddr;
	conf->cfg.src_addr = dst_paddr;
	conf->cfg.dst_addr = src_paddr;

	conf->handler = dmahost_irq_handler;
	conf->data = dh;
	conf->ie_stop = false;
	conf->ie_finish = true;
	conf->ie_transmit = false;
	conf->forced = false;
	conf->itv_cycles = 1;

	init_completion(&dh->comp);

	retval = dmaengine_slave_config(chan, &conf->cfg);
	if(retval) {
		pr_err("%s, config dma failed, %d\n", __func__, retval);
		return retval;
	}

	/* get dma descriptor data */
	dh->tx = dmaengine_prep_slave_sg(chan, 0, 0, DMA_MEM_TO_MEM, 0);
	if(!dh->tx) {
		pr_err("%s, get dma desc failed\n", __func__);
		return -EFAULT;
	}
	pr_info("%s, tx = %p\n", __func__, dh->tx);

	/* submit descriptor */
	dmaengine_submit(dh->tx);

	wait = msecs_to_jiffies(DMAHOST_WAIT_TM);

	/* start dma transfer */
	dma_async_issue_pending(chan);

#ifdef CONFIG_DMA_PERF
	tick1 = get_hwtick(0);
#endif

#ifdef CONFIG_DMA_PAUSE
	dmaengine_pause(chan);
	msleep(100);
	dmaengine_resume(chan);
	mdelay(1);

	dmaengine_pause(chan);
	msleep(100);
	dmaengine_resume(chan);
#endif

	retval = wait_for_completion_timeout(&dh->comp, wait);
	if(retval <= 0) {
		pr_err("%s, wait comp timeout %d\n", __func__, retval);
	} else {
		pr_info("%s, wait comp okay\n", __func__);
	}
#ifdef CONFIG_DMA_PERF
	tick2 = get_hwtick(0);
	pr_info("%s, dma memcpy %d bytes, cost %d ticks\n", __func__,
			bufsize, (tick2 - tick1));
#endif
	dma_unmap_single(&pdev->dev, src_paddr, bufsize, DMA_FROM_DEVICE);
	dma_unmap_single(&pdev->dev, dst_paddr, bufsize, DMA_TO_DEVICE);

	/* check result */
	retval = dmahost_buf_set(src_addr, dst_addr, bufsize, 2);
	if(retval) {
		pr_err("%s, buffer data error %d\n", __func__, retval);
	} else {
		pr_info("%s, buffer data okay\n", __func__);
	}

#if 0

	pr_info("%s, ************ test 2\n", __func__);

	dmahost_buf_set(src_addr, dst_addr, bufsize, 0);
	dmahost_buf_set(src_addr, dst_addr, bufsize, 1);

	src_paddr = dma_map_single(&pdev->dev, (void *)src_addr,
						bufsize, DMA_FROM_DEVICE);
	dst_paddr = dma_map_single(&pdev->dev, (void *)dst_addr,
						bufsize, DMA_TO_DEVICE);

	conf->cfg.src_addr = src_paddr;
	conf->cfg.dst_addr = dst_paddr;

	retval = dmaengine_slave_config(chan, &conf->cfg);
	if(retval) {
		pr_err("%s, config dma failed, %d\n", __func__, retval);
		return retval;
	}

	dh->tx = dmaengine_prep_slave_sg(chan, 0, 0, DMA_MEM_TO_MEM, 0);
	if(!dh->tx) {
		pr_err("%s, get dma desc failed\n", __func__);
		return -EFAULT;
	}
	pr_info("%s, tx = %p\n", __func__, dh->tx);

	dmaengine_submit(dh->tx);

	dma_async_issue_pending(chan);

	reinit_completion(&dh->comp);
	wait = msecs_to_jiffies(DMAHOST_WAIT_TM);
	retval = wait_for_completion_timeout(&dh->comp, wait);
	if(retval <= 0) {
		pr_err("%s, wait comp timeout %d\n", __func__, retval);
	} else {
		pr_info("%s, wait comp okay\n", __func__);
	}

	dma_unmap_single(&pdev->dev, src_paddr, bufsize, DMA_FROM_DEVICE);
	dma_unmap_single(&pdev->dev, dst_paddr, bufsize, DMA_TO_DEVICE);

	retval = dmahost_buf_set(src_addr, dst_addr, bufsize, 2);
	if(retval) {
		pr_err("%s, buffer data error %d\n", __func__, retval);
	} else {
		pr_info("%s, buffer data okay\n", __func__);
	}
#endif
	pr_info("%s, test done\n", __func__);
	return 0;
}

static int dmahost_buf_set(void *src, void *dst, unsigned len, int cmd)
{
	int ret = 0;
	int i;

	pr_info("%s, src(%p), dst(%p), len=%d\n", __func__, src, dst, len);

	switch(cmd) {
	case 0:// init buffer
		pr_info("clear buffer\n");
		if(src)
			memset(src, 0x0, len);
		if(dst)
			memset(dst, 0x0, len);
		break;
	case 1:// fill data
	{
		unsigned char *ps = src;
		if(ps) {
			pr_info("fill data\n");
			for(i = 0;i < len;i++) {
				*ps = i & 0xff;
				ps++;
			//	pr_info("ps[%d] = %x\n", i, *ps);
			}
		}
	}
		break;
	case 2:// check result
	{
		unsigned char *pd = dst;
		unsigned char *ps = src;

		pr_info("check result ...");
		for(i = 0;i < len; i++) {
			if(*pd != *ps) {
				ret++;
			}
			pd++;
			ps++;
		}
		pr_info("error %d\n", ret);
	}
		break;
	default:
		break;
	}
	return ret;
}


static struct platform_driver dmahost_driver = {
	.probe = dmahost_probe,
	.driver = {
		.name = "dmahost",
		.owner = THIS_MODULE,
	},
};

static int __init dmahost_init(void)
{
	return platform_driver_register(&dmahost_driver);
}

static void __exit dmahost_exit(void)
{
	return platform_driver_unregister(&dmahost_driver);
}

module_init(dmahost_init);
module_exit(dmahost_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("dma host driver");
MODULE_AUTHOR("daqingzhang <daqingzhang@rdamicro.com>");
