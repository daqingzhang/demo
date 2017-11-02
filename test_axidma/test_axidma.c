#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/completion.h>
#include <asm-generic/cacheflush.h>
#include <plat/devices.h>

//#define DMACLIENT_NAME "dmaclient"
#define DMACLIENT_NAME "rda-undef"

static struct platform_device dmaclient = {
	.name = DMACLIENT_NAME,
	.id = 1,
};

static int __init dmaclient_dev_init(void)
{
	pr_info("%s, register a axidma client\n", __func__);
	return platform_device_register(&dmaclient);
}

arch_initcall(dmaclient_dev_init);


struct dmaclient_dev {
	struct device *dev;
	struct dma_chan *chan;
	struct dma_async_tx_descriptor *tx;
	struct dma_slave_config conf;
	struct scatterlist dst_sg;
	struct scatterlist src_sg;
	struct completion comp;
};

#define DMABUF_SRC_SIZE 1024
#define DMABUF_DST_SIZE 1024

static struct dmaclient_dev dcdev;
static u8 dmabuf_src[DMABUF_SRC_SIZE];
static u8 dmabuf_dst[DMABUF_DST_SIZE];

static void dmaclient_callback_tx_done(void *data)
{
	struct dmaclient_dev *dc = data;

	pr_info("%s dc=%p\n", __func__, dc);

	if (!dc) {
		pr_info("data is null, exit\n");
		return;
	}
	complete(&dc->comp);
}

static void dmaclient_init(struct dmaclient_dev *dc, dma_addr_t dst, dma_addr_t src, u32 size, int width)
{
	struct dma_slave_config *conf = &dc->conf;

	conf->direction = DMA_MEM_TO_MEM;
	conf->src_maxburst = size;
	conf->dst_maxburst = size;
	conf->src_addr = src;
	conf->dst_addr = dst;
	conf->slave_id = 0;

	switch (width) {
	case 8:
		conf->src_addr_width = DMA_SLAVE_BUSWIDTH_8_BYTES;
		conf->dst_addr_width = DMA_SLAVE_BUSWIDTH_8_BYTES;
		break;
	case 4:
		conf->src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
		conf->dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
		break;
	case 2:
		conf->src_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES;
		conf->dst_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES;
		break;
	case 1:
	default:
		conf->src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
		conf->dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
		break;
	}
	dc->dst_sg.dma_address = dst;
	dc->dst_sg.length = size;

	dc->src_sg.dma_address = src;
	dc->src_sg.length = size;

	pr_info("%s, src=%x, dst=%x, size=%d, width=%d\n", __func__,
		src, dst, size, width);
}

static int dmaclient_setup_callback(struct dmaclient_dev *dc, dma_async_tx_callback cb)
{
	struct dma_async_tx_descriptor *ptx = dc->tx;

	if (!ptx) {
		pr_err("%s, tx is null\n", __func__);
		return -EINVAL;
	}
	ptx->callback = cb;
	ptx->callback_param = (void *)dc;
	return 0;
}

enum dmaclient_cmd {
	CMD_CLEAR_BUF,
	CMD_WRITE_BUF,
	CMD_DUMP_BUF,
};

static unsigned dmaclient_prep_buf(u8 *buf, int len, int cmd)
{
	int i;
	unsigned int ret = 0;

	switch(cmd) {
	case CMD_CLEAR_BUF:
		for(i = 0;i < len;i++)
			buf[i] = 0x5a;
		break;
	case CMD_WRITE_BUF:
		for(i = 0;i < len;i++)
			buf[i] = i;
		break;
	case CMD_DUMP_BUF:
		printk(KERN_INFO "\ndump dma buffer data at %p:\n", buf);
		for(i = 0;i < len;i += 16) {
			pr_info("# %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x #\n",
				buf[i+0],  buf[i+1],  buf[i+2],  buf[i+3],
				buf[i+4],  buf[i+5],  buf[i+6],  buf[i+7],
				buf[i+8],  buf[i+9],  buf[i+10], buf[i+11],
				buf[i+12], buf[i+13], buf[i+14], buf[i+15]);
		}
	default:
		break;
	}
	return ret;
} 

void dmaclient_request_chan_test(struct device *dev)
{
	struct dma_chan *chan;

	pr_info("request dma chan test 1\n");
	chan = dma_request_chan(dev, RDA_AXIDMA_CH_RX);
	if (IS_ERR(chan))
		pr_err("request chan failed, %d\n", (int)PTR_ERR(chan));
	else
		pr_info("chan=%p\n", chan);

	dma_release_channel(chan);
	pr_info("chan %p is released\n", chan);

	pr_info("request dma chan test 2\n");
	chan = dma_request_chan(dev, RDA_AXIDMA_CH_RX);
	if (IS_ERR(chan))
		pr_err("request chan failed, %d\n", (int)PTR_ERR(chan));
	else
		pr_info("chan=%p\n", chan);
}

static int dmaclient_probe(struct platform_device *pdev)
{
	int ret, tx_cnt = 3, len = DMABUF_DST_SIZE >> 1;
	int bus_width[] = {8,4,2,1};
	struct dma_chan *chan;
	struct dmaclient_dev *pdc = &dcdev;
	struct device *dev;
	struct dma_async_tx_descriptor *tx;
	unsigned int wait;
	dma_addr_t src, dst;
	u8 *vir_src = dmabuf_src;
	u8 *vir_dst = dmabuf_dst;

#define DMACLIENT_TIMEOUT_MS 500

	pr_info("%s\n", __func__);

#if 1
	vir_src = (u8 *)(((u32)dmabuf_src & (~0x7)) + 0x10);
	vir_dst = (u8 *)(((u32)dmabuf_dst & (~0x7)) + 0x10);
#endif

	init_completion(&pdc->comp);
#if 0
	// find axidma device
	dev = bus_find_device_by_name(&platform_bus_type, NULL, RDA_AXIDMA_DEV_NAME);
	if (!dev) {
		pr_err("axidma dev is null\n");
		return -ENODEV;
	}
#else
	dev = &pdev->dev;
#endif
	pdc->dev = dev;

	// print device name
	pr_info("dev name: %s, ch name: %s\n",
		dev_name(dev), RDA_AXIDMA_CH_RX);

	if (dev->init_name) {
		pr_info("dev->init_name: %s\n", dev->init_name);
	}
	if (dev->kobj.name) {
		pr_info("dev->kobj.name: %s\n", dev->kobj.name);
	}

	// request dma channel
	chan = dma_request_chan(dev, RDA_AXIDMA_CH_RX);
	if (IS_ERR(chan)) {
		pr_err("request chan failed, %d\n", (int)PTR_ERR(chan));
		return -ENODEV;
	}
	pdc->chan = chan;
	pr_info("axidma dev=%p, chan=%p\n", dev, chan);

#if 0
	dmaclient_request_chan_test(dev);
#endif
	// prepare data
	dmaclient_prep_buf(vir_src, len, CMD_WRITE_BUF);
	dmaclient_prep_buf(vir_dst, len, CMD_CLEAR_BUF);
	flush_cache_all();
	dmaclient_prep_buf(vir_src, len, CMD_DUMP_BUF);
	dmaclient_prep_buf(vir_dst, len, CMD_DUMP_BUF);
	pr_info("vir_src=%p, vir_dst=%p\n", vir_src, vir_dst);

	// map dma memory
	src = dma_map_single(&pdev->dev, vir_src, len, DMA_BIDIRECTIONAL);
	dst = dma_map_single(&pdev->dev, vir_dst, len, DMA_BIDIRECTIONAL);

	// init dmaclient
	dmaclient_init(pdc, dst, src, len, bus_width[tx_cnt % 4]);

	// configure dma channel
	ret = dmaengine_slave_config(chan, &pdc->conf);
	if (ret) {
		pr_err("config dma failed, %d\n", ret);
		goto fail_cfg;
	}

	// prepare dma channel
	tx = dmaengine_prep_slave_sg(chan, 0, 0, DMA_MEM_TO_MEM, 0);
	if (!tx) {
		pr_err("get tx failed\n");
		goto fail_cfg;
	}
	pdc->tx = tx;

	// setup callback function
	ret = dmaclient_setup_callback(pdc, dmaclient_callback_tx_done);
	if (ret) {
		pr_err("setup callback failed\n");
		goto fail_cfg;
	}

dma_start:
	dmaengine_submit(tx);

	dma_async_issue_pending(chan);

	wait = msecs_to_jiffies(DMACLIENT_TIMEOUT_MS);
	ret = wait_for_completion_timeout(&pdc->comp, wait);
	pr_info("dma %s\n", (ret > 0) ? ("complete") : ("timeout"));
	if (ret <= 0) {
		dmaengine_terminate_all(chan);
	} else {
		enum dma_status s;
		dma_cookie_t last = 0, used = 0;
		s = dma_async_is_tx_complete(chan, 0, &last, &used);
		pr_info("%s state %d, last=%d, used=%d\n",
			(s == DMA_COMPLETE) ? "good" : "bad", s, last, used);
	}
	// unmap dma memory
	dma_unmap_single(&pdev->dev, src, len, DMA_BIDIRECTIONAL);
	dma_unmap_single(&pdev->dev, dst, len, DMA_BIDIRECTIONAL);

	// diaplay buffer's data
	pr_info("after dma finish trasmition\n");
	dmaclient_prep_buf(vir_src, len, CMD_DUMP_BUF);
	dmaclient_prep_buf(vir_dst, len, CMD_DUMP_BUF);

#if 1
	if(tx_cnt > 0) {
		pr_info("start dma again, tx_cnt %d\n", tx_cnt);
		tx_cnt--;

		//delay sometime
		msleep(100);

		reinit_completion(&pdc->comp);

		// prepare data
		dmaclient_prep_buf(vir_src, len, CMD_WRITE_BUF);
		dmaclient_prep_buf(vir_dst, len, CMD_CLEAR_BUF);
		flush_cache_all();
		pr_info("before dma start transmition\n");
		dmaclient_prep_buf(vir_src, len, CMD_DUMP_BUF);
		dmaclient_prep_buf(vir_dst, len, CMD_DUMP_BUF);

		// map dma memory
		src = dma_map_single(&pdev->dev, vir_src, len, DMA_BIDIRECTIONAL);
		dst = dma_map_single(&pdev->dev, vir_dst, len, DMA_BIDIRECTIONAL);

		dmaclient_init(pdc, dst, src, len, bus_width[tx_cnt % 4]);
#if 1
		/*
		 * It's not necessary to configure dma channel again after this channel
		 * is configured.
		 */
		// configure dma channel
		ret = dmaengine_slave_config(chan, &pdc->conf);
		if (ret) {
			pr_err("config dma failed, %d\n", ret);
			goto fail_cfg;
		}
#endif
		tx = dmaengine_prep_dma_sg(chan, &pdc->dst_sg, 1, &pdc->src_sg, 1, 0);
		if (!tx) {
			pr_err("get tx failed\n");
			goto fail_cfg;
		}
		goto dma_start;
	}
#endif
	pr_info("%s test done\n", __func__);
	return 0;
fail_cfg:
	dma_unmap_single(&pdev->dev, src, len, DMA_BIDIRECTIONAL);
	dma_unmap_single(&pdev->dev, dst, len, DMA_BIDIRECTIONAL);
	return -EFAULT;
}

static int dmaclient_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver dmaclient_drv = {
	.probe = dmaclient_probe,
	.remove = dmaclient_remove,
	.driver = {
		.name = DMACLIENT_NAME,
		.owner = THIS_MODULE,
	},
};

static int __init dmaclient_drv_init(void)
{
	return platform_driver_register(&dmaclient_drv);
}

static void __exit dmaclient_drv_exit(void)
{
	platform_driver_unregister(&dmaclient_drv);
}

late_initcall(dmaclient_drv_init);
module_exit(dmaclient_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("daqingzhang");
MODULE_DESCRIPTION("axidma client driver");
