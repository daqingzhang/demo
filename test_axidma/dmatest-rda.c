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

/*
 * DMA Host Device
 */

struct resource dmahost_resource[] = {
	{
		.start = 0x1000,
		.end   = 0x2000,
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

#define CONFIG_DMAHOST_BUF_SIZE 1024

static unsigned char dmahost_srcbuf[CONFIG_DMAHOST_BUF_SIZE];
static unsigned char dmahost_dstbuf[CONFIG_DMAHOST_BUF_SIZE];
static int dmahost_irq_done = 0;

static int dmahost_buf_set(void *src, void *dst, unsigned len, int cmd)
{
	int ret = 0;
	int i;

	pr_info("%s, src(%p), dst(%p), len=%d\n", __func__, src, dst, len);

	switch(cmd) {
	case 0:// init buffer
		pr_info("init buffer\n");
		memset(src, 0x0, len);
		memset(dst, 0x0, len);
		break;
	case 1:// fill data
	{
		unsigned char *ps = src;

		pr_info("fill data\n");
		for(i = 0;i < len;i++)
			*ps++ = i & 0xff;
	}
		break;
	case 2:// check result
	{
		unsigned char *ps = src;
		unsigned char *pd = dst;

		pr_info("check result\n");
		for(i = 0;i < len; i++) {
			if(*ps != *pd) {
				pr_info("data error, pd[%d] = %2x, should be %2x\n",
						i, *pd, *ps);
				ret++;
			}
			ps++;
			pd++;
		}
	}
		break;
	default:
		break;
	}

	return ret;
}

static irqreturn_t dmahost_irq_handler(int id, void *data)
{
	int *flag = data;

	*flag = 1;
	pr_info("%s, id = %d\n", __func__, id);
	return IRQ_HANDLED;
}

static int dmahost_probe(struct platform_device *pdev)
{
	int retval;
	volatile int wait = 10;
	struct dma_chan *rxchan;
	unsigned char *src_addr = dmahost_srcbuf;
	unsigned char *dst_addr = dmahost_dstbuf;
	unsigned int bufsize = CONFIG_DMAHOST_BUF_SIZE;
	struct dma_async_tx_descriptor *tx;

	struct rda_dma_config config = {
		.cfg = {
			.direction = DMA_MEM_TO_MEM,
			.src_addr = (unsigned int)src_addr,
			.dst_addr = (unsigned int)dst_addr,
			.src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE,
			.dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE,
			.src_maxburst = bufsize,
			.dst_maxburst = bufsize,
		},
		.ie_stop = false,
		.ie_finish = true,
		.ie_transmit = false,
		.handler = dmahost_irq_handler,
		.data = &dmahost_irq_done,
	};

	dmahost_buf_set(src_addr, dst_addr, bufsize, 0);
	dmahost_buf_set(src_addr, dst_addr, bufsize, 1);
#if 0
	struct device *mdev;

	mdev = bus_find_device_by_name(&platform_bus_type, NULL, "dmahost.1");
	pr_info("mdev(%p), pdev(%p), pdev->dev(%x)\n", mdev, pdev, (unsigned int)&pdev->dev);

	mdev = bus_find_device_by_name(&platform_bus_type, NULL, "8816000.rda-uart1");
	pr_info("mdev(%p), pdev(%p), pdev->dev(%x)\n", mdev, pdev, (unsigned int)&pdev->dev);

	mdev = bus_find_device_by_name(&platform_bus_type, NULL, "8817000.rda-uart2");
	pr_info("mdev(%p), pdev(%p), pdev->dev(%x)\n", mdev, pdev, (unsigned int)&pdev->dev);

#endif
	rxchan = dma_request_chan(&pdev->dev, "rxtx");
	if(IS_ERR(rxchan)) {
		pr_err("%s, request rx chan failed\n", __func__);
		retval = PTR_ERR(rxchan);
		return retval;
	}
	pr_info("%s, rxchan = %p\n", __func__, rxchan);

	retval = dmaengine_slave_config(rxchan, &config.cfg);
	if(retval) {
		pr_err("%s, config dma failed, %d\n", __func__, retval);
		return retval;
	}

//	tx = dmaengine_prep_slave_single(rxchan, 0, 0, DMA_MEM_TO_MEM, 0);
	tx = dmaengine_prep_slave_sg(rxchan, 0, 0, DMA_MEM_TO_MEM, 0);
	if(!tx) {
		pr_err("%s, get dma desc failed\n", __func__);
		return -EFAULT;
	}
	pr_info("%s, tx = %p\n", __func__, tx);

	dmaengine_submit(tx);

	dma_async_issue_pending(rxchan);

	while(1) {
		enum dma_status state;

		state = dma_async_is_tx_complete(rxchan, 0, 0, 0);
		if(state == DMA_COMPLETE) {
			pr_info("%s, dma trans done\n", __func__);
			break;
		}
		pr_info("%s, dma state %d\n", __func__, state);

		msleep(100);

		if(!wait) {
			pr_info("%s, wait timeout\n", __func__);
			break;
		}
		wait--;
	}

	dmaengine_terminate_all(rxchan);

	dmahost_buf_set(src_addr, dst_addr, bufsize, 2);

	pr_info("%s, test done\n", __func__);
	return 0;
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
