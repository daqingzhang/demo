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
#include <plat/rda_axidma.h>

#define DMACLIENT_NAME "dmaclient"

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
	struct rda_dma_config conf;
	struct scatterlist dst_sg;
	struct scatterlist src_sg;
	struct completion comp;
};

#define DMABUF_SRC_SIZE 512
#define DMABUF_DST_SIZE 512

static struct dmaclient_dev dcdev;
static u8 dmabuf_src[DMABUF_SRC_SIZE];
static u8 dmabuf_dst[DMABUF_DST_SIZE];

static irqreturn_t dmaclient_irq_handler(int id, void *data)
{
	struct dmaclient_dev *dc = data;
	enum dma_status s;
	dma_cookie_t last = 0, used = 0;

	pr_info("%s \n", __func__);

	s = dma_async_is_tx_complete(dc->chan, 0, &last, &used);
	if (s == DMA_COMPLETE) {
		pr_info("last=%d, used=%d\n", last, used);
		complete(&dc->comp);
	} else {
		pr_info("state=%d\n", s);
	}
	return IRQ_HANDLED;
}

void dmaclient_init(struct dmaclient_dev *dc, dma_addr_t dst, dma_addr_t src, u32 size)
{
	struct rda_dma_config *conf = &dc->conf;

	conf->cfg.direction = DMA_MEM_TO_MEM;
	conf->cfg.src_addr_width = DMA_SLAVE_BUSWIDTH_8_BYTES;
	conf->cfg.dst_addr_width = DMA_SLAVE_BUSWIDTH_8_BYTES;
	conf->cfg.src_maxburst = size;
	conf->cfg.dst_maxburst = size;
	conf->cfg.src_addr = src;
	conf->cfg.dst_addr = dst;

	conf->handler = dmaclient_irq_handler;
	conf->data = (void *)dc;
	conf->ie_finish = true;
	conf->ie_stop = false;
	conf->ie_transmit = false;
	conf->itv_cycles = 1;

	dc->dst_sg.dma_address = dst;
	dc->dst_sg.length = size;

	dc->src_sg.dma_address = src;
	dc->src_sg.length = size;
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
			buf[i] = 0;
		break;
	case CMD_WRITE_BUF:
		for(i = 0;i < len;i++)
			buf[i] = i;
		break;
	case CMD_DUMP_BUF:
		for(i = 0;i < len;i += 8) {
			pr_info("%2x %2x %2x %2x %2x %2x %2x %2x\n",
				buf[i+0], buf[i+1], buf[i+2], buf[i+3],
				buf[i+4], buf[i+5], buf[i+6], buf[i+7]);
		}
		break;
	default:
		break;
	}
	return ret;
} 

static int dmaclient_probe(struct platform_device *pdev)
{
	int ret, tx_cnt = 1, len = DMABUF_DST_SIZE;
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

	// find axidma device
	dev = bus_find_device_by_name(&platform_bus_type, NULL, RDA_AXIDMA_DEV_NAME);
	if (!dev) {
		pr_err("axidma dev is null\n");
		return -ENODEV;
	} else {
		pr_info("axidma dev=%p\n", dev);
	}
	pdc->dev = dev;
	init_completion(&pdc->comp);

	// request dma channel
	chan = dma_request_chan(dev, RDA_AXIDMA_CH1_NAME);
	if (IS_ERR(chan)) {
		pr_err("request chan failed, %d\n", (int)PTR_ERR(chan));
		return -ENODEV;
	}
	pdc->chan = chan;

	// prepare data
	dmaclient_prep_buf(vir_src, len, CMD_WRITE_BUF);
	dmaclient_prep_buf(vir_dst, len, CMD_CLEAR_BUF);
	flush_cache_all();

	// map dma memory
	src = dma_map_single(&pdev->dev, vir_src, len, DMA_BIDIRECTIONAL);
	dst = dma_map_single(&pdev->dev, vir_dst, len, DMA_BIDIRECTIONAL);

	// init dmaclient
	dmaclient_init(pdc, dst, src, len);

	// configure dma channel
	ret = dmaengine_slave_config(chan, &pdc->conf.cfg);
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

dma_start:
	dmaengine_submit(tx);

	dma_async_issue_pending(chan);

	wait = msecs_to_jiffies(DMACLIENT_TIMEOUT_MS);
	ret = wait_for_completion_timeout(&pdc->comp, wait);
	if (ret > 0) {
		pr_info("dma complete\n");
	} else {
		pr_err("dma timeout\n");
	}

	// unmap dma memory
	dma_unmap_single(&pdev->dev, src, len, DMA_BIDIRECTIONAL);
	dma_unmap_single(&pdev->dev, dst, len, DMA_BIDIRECTIONAL);

	// diaplay buffer's data
	dmaclient_prep_buf(vir_src, len, CMD_DUMP_BUF);
	dmaclient_prep_buf(vir_dst, len, CMD_DUMP_BUF);

#if 1
	if(tx_cnt > 0) {
		pr_info("start dma again, tx_cnt %d\n", tx_cnt);
		tx_cnt--;

		//delay sometime
		msleep(100);

		// prepare data
		dmaclient_prep_buf(vir_src, len, CMD_WRITE_BUF);
		dmaclient_prep_buf(vir_dst, len, CMD_CLEAR_BUF);
		flush_cache_all();

		// map dma memory
		src = dma_map_single(&pdev->dev, vir_src, len, DMA_BIDIRECTIONAL);
		dst = dma_map_single(&pdev->dev, vir_dst, len, DMA_BIDIRECTIONAL);

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

module_init(dmaclient_drv_init);
module_exit(dmaclient_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("daqingzhang");
MODULE_DESCRIPTION("axidma client driver");
