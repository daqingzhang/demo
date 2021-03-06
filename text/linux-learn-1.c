1. Platform Device
==================================================================

#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform.h>
#include <linux/interrupt.h>

struct resource xxx_resource[] = {
	{
		.start = phyaddr,
		.end   = phyaddr + physize - 1,
		.flags = IORESOURCE_MEM,
	},
	{
		.start = INT_RTC_TIMER,
		.end   = INT_RTC_TIMER,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device xxx_device {
	.name = "xxx",
	.id = x,
	.resource = xxx_resource,
	.num_resources = ARRAY_SIZE(xxx_resource),
};

int platform_device_register(struct platform_device *pdev);
void platform_device_unregister(struct platform_device *pdev);

/**
 * @pdev: platform device
 * @type: resource type
 *	IORESOURCE_MEM
 *	IORESOURCE_IRQ
 * @num: resource index
 */
struct resource *platform_get_resource(struct platform_device *pdev,
					unsigned int type,
					unsigned int num);
struct resource *platform_get_resource_by_name(struct platform_device *dev,
						unsigned int type,
						const char *name);
/*
 * example:
 	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(!res) {
		pr_err("error \n");
	}

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 1);
	if(!res) {
		pr_err("error \n");
	}
 */
int platform_get_irq(struct platfor_device *dev, unsigned int num);
int platform_get_irq_byname(struct platform_device *dev, const char *name);

int platform_add_devices(struct platform_device **devs, int num);

static struct platform_device *devlist[] = {
	&a,
	&b,
};

//platform_add_devices(devlist,ARRAY_SIZE(devlist);

2. Platform Driver
==================================================================
static int xxx_probe(struct platform_device *pdev)
{
}

static int xxx_remove(struct platform_device *pdev)
{
	...
}

static struct platform_driver xxx_driver = {
	.probe = xxx_proble,
	.remove = xxx_remove,
	.driver = {
		.name = "xxx",
		.owner = THIS_MODULE,
		.of_match_table = xxx_of_match,
	},
};

int platform_driver_register(struct platform_driver *drv);
void platform_driver_unregister(struct platform_driver *drv);

void platform_set_drvdata(const struct paltform_device *pdev, void *data);
void *platform_get_drvdata(const struct platform_device *pdev);



3. Driver Info
==================================================================
MODULE_AUTHOR("xxx <xxx@yyy.com>");
MODULE_DESCRIPTION("xxx");
MODULE_LICENSE("GPL");



4. Memory Access
==================================================================

#include <linux/io.h>
//arch/arm/include/asm/io.h
// #include <asm/io.h>

/*
 * devm_ioremap - remap a physicall memory space to a virtual io memory space
 */
void __iomem *ioremap(unsigned long phyaddr, unsigned long size);
void __iomem *devm_ioremap(struct device *dev, resource_size_t phyaddr, unsigned long size);
void __iomem *devm_ioremap_resource(struct device *dev, struct resource *res);

void iounmap(void __iomem *addr);
void devm_iounmap(struct device *dev, void __iomem *addr);

/*
 * devm_memremap - remap a physical memory space to virtual memory space
 * offset	physical address
 * size		memory size
 * flags	MEMREMAP_WB,
 *		MEMREMAP_WT,
 *		MEMREMAP_WC,
 */
void *devm_memremap(struct device *dev, resource_size_t offset, size_t size, unsigned long flags);
void devm_memunmap(struct device *dev, void *addr);

void *memremap(resource_size_t paddr, size_t size, unsigned long flags);
void memunmap(void *vaddr);

u8  readb(const volatile void __iomem *vaddr);
u16 readw(const volatile void __iomem *vaddr);
u32 readl(const volatile void __iomem *vaddr);

void writeb(u8 b, volatile const __iomem *vaddr);
void writeb(u16 b, volatile const __iomem *vaddr);
void writel(u32 b, volatile const __iomem *vaddr);



5. Memory Allocate
==================================================================
#include <linux/slab.h>

/*
 * flags:
 *	GFP_KERNEL, allocate normal kernel ram, may sleep
 *	GFP_NOWAIT, allocate kernel ram, not sleep
 */

void *kmalloc(size_t size, gfp_t flags);
void *kzalloc(size_t size, gfp_t flags);
void  kfree(const void *vaddr);

void *devm_kzalloc(size_t size, gfp_t flags);
void devm_kfree(const void *vaddr);



6. DMA client
==================================================================
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>

(1) get a dma channel by dev and channel name
struct dma_chan *dma_request_chan(struct device *dev, const char *name);

(2) configure dma channel
int dmaengine_slave_config(struct dma_chan *chan, struct dma_slave_config *cfg);

(3) get dma tx descriptor
struct dma_async_tx_descriptor *dmaengine_prep_slave_sg(struct dma_chan *chan,
				struct scatterlist *sgl, unsigned int sg_len,
				enum dma_transfer_direction dir, unsigned int flags);

(4) submit tx descriptor
dma_cookie_t dmaengine_submit(struct dma_async_tx_descriptor *tx);

(5) start dma transfer
void dma_async_issue_pending(struct dma_chan *chan);

(6) remap virtual memory space to physical memory for dma

/*
 * dir: DMA_FROM_DEVICE
 *	DMA_TO_DEVICE
 *	DMA_BIDIRECTIONAL
 */

dma_addr_t dma_map_single(struct device *dev, void *vaddr, size_t size, enum dma_data_direction dir);

(7) remap physical memory address to virtual memory space for cpu

void dma_unmap_single(struct device *dev, dma_addr_t paddr, size_t size, enum dma_data_direction dir);


(8) pause dma transmition
int dmaengine_pause(struct dma_chan *chan);

(9) resume dma transmition
int dmaengine_resume(struct dma_chan *chan);


7. container_of(ptr, type, member)
==================================================================

#define container_of(ptr, type, member) \
	(type *)((char *)(ptr) - (char *) &((type *)0)->member)<F12>


8. GPIO Driver
==================================================================
struct xxx_gpio_chip {
	int irq;
	struct gpio_chip *gc;
	struct xxx_regs *regs;
	struct irq_chip *irqc;
	void __iomem *base;
	spinlock_t lock;
};

// register a gpio chip
int gpiochip_add(struct gpio_chip *gc);

// register a gpio chip with private data
int gpiochip_add_data(struct gpio_chip *gc, void *data);

// find a gpio chip by a special function
struct gpio_chip *gpiochip_find(void *data,
				int (*match)(struct gpio_chip *chip, void *data));

// get gpio gpio chip's private data
void *gpiochip_get_data(struct gpio_chip *gc);

// remove a gpio chip
void gpiochip_remove(struct gpio_chip *gc);

9. GPIO Driver User
==================================================================
int gpio_request(unsigned gpio, const char *label);
int gpio_direction_input(unsigned gpio);
int gpio_direction_output(unsigned gpio, int value);
int gpio_set_debounce(unsigned gpio, unsigned debounce);
int gpio_get_value(unsigned gpio);
int gpio_set_value(unsigned gpio);
int gpio_to_irq(unsigned gpio);
void gpio_free(unsigned gpio);



10. Interrupt
==================================================================
int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
			const char *name, void *dev);

example:
	ret = request_irq(irq, xxx_irq_handler, IRQF_SHARED, "xxx_irq", prvdata);
	if(ret) {
		pr_err("error\n");
		return ret;
	}



11. Semaphore & Mutex & SpinLock
==================================================================
#include <linux/semaphore.h>

void sema_init(struct semaphore *sem, int val);

//return 0 if semaphore is acquired
//return -ETIME if timeout

int down_timeout(struct semaphore *sem, long jiffies);

//try to acquire the sema without waiting
//return 0 if semaphore is acquired
//return 1 if failed to acquire sema

int down_trylock(struct semaphore *sem);
void up(struct semaphore *sem);


void spin_lock_init(spinlock_t *lock);
void spin_lock_irqsave(spinlock_t *lock, unsigned long flags);
void spin_lock_irqrestore(spinlock_t *lock, unsigned long flags);

#include <linux/completion.h>

void init_completion(struct completion *comp);
void reinit_completion(struct completion *comp);
unsigned long wait_for_completion_timeout(struct completion *comp, unsigned long timeout);
void complete(struct completion *comp);



