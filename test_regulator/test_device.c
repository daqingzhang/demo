#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/i2c.h>
#include <media/soc_camera.h>

#include <asm/io.h>
#include <linux/spi/spi.h>
#include <asm/mach-types.h>
#include <asm/mach/map.h>
#include <mach/hardware.h>
#include <mach/iomap.h>
#include <mach/irqs.h>
#include <plat/devices.h>
#include <plat/ap_clk.h>
#include <mach/board.h>
#include <mach/ifc.h>
#include <mach/gpio_id.h>
#include <linux/mmc/host.h>
#include <linux/i2c-gpio.h>

#include <mach/regulator.h>
#include <linux/regulator/consumer.h>

static struct resource test_resource[] = {
	{
		.start = 0x1000,
		.end   = 0x2000,
		.flags = IORESOURCE_MEM,
	},
	{
		.start = 0x30,
		.end   = 0x30,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device rda_regu_test = {
	.name = "rda_regu_test",
	.id = 1,
	.resource = test_resource,
	.num_resources = ARRAY_SIZE(test_resource),
};

static struct platform_device *devices[] = {
	&rda_regu_test,
};

static int __init rda_test_dev_init(void)
{
	int retval;

	retval = platform_device_register(devices[0]);
	if(retval) {
		pr_err("%s, failed to register platdev %d\n",
			__func__,retval);
		return retval;
	}
	pr_info("%s, register plat device okay\n",__func__);
	return 0;
}

arch_initcall(rda_test_dev_init);
