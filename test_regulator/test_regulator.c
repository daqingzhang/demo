#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/of_regulator.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#define RDA_TEST_REGU_NAME "vddsim0"
#define RDA_TEST_CNT 10

static int rda_regulator_test(struct platform_device *pdev)
{
	int retval,cnt = RDA_TEST_CNT;
	struct regulator *regu;

	pr_info("%s\n", __func__);

	regu = regulator_get(NULL, RDA_TEST_REGU_NAME);
	if(IS_ERR(regu)) {
		pr_err("failed to get regulator %s, %d\n",
				RDA_TEST_REGU_NAME, (int)(regu));
		return -ENODEV;
	}

	while(cnt) {
		bool enabled;
		int vol;

		enabled = regulator_is_enabled(regu);
		if(enabled) {
			retval = regulator_disable(regu);
			pr_info("disable regulator %s ,%d\n",
					RDA_TEST_REGU_NAME,retval);
		} else {
			retval = regulator_enable(regu);
			pr_info("enable regulator %s ,%d\n",
					RDA_TEST_REGU_NAME,retval);
		}

		vol = regulator_get_voltage(regu);
		pr_info("regulator %s voltage is %d\n",RDA_TEST_REGU_NAME,vol);

		if(enabled) {
			vol += 10;
			retval = regulator_set_voltage(regu,vol,vol);
		}

		msleep(200);
		cnt--;
	}
	pr_info("%s done\n", __func__);
	return retval;
}

static struct platform_driver rda_test_driver = {
	.probe = rda_regulator_test,
	.driver = {
		.name = "rda_regu_test",
		.owner = THIS_MODULE,
	},
};

static int __init regu_test_init(void)
{
	return platform_driver_register(&rda_test_driver);
}

module_init(regu_test_init);

MODULE_DESCRIPTION("rda regulator test");
MODULE_AUTHOR("RDA");
MODULE_LICENSE("GPL");
