/*
 * Watchdog driver for the RDA8910
 *
 * 2017 (c) RDAMicro, Inc. This file is licensed under the
 * terms of the GNU General Public License version 2. This program is
 * licensed "as is" without any warranty of any kind, whether express
 * or implied.
 *
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/watchdog.h>
#include <linux/reboot.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/moduleparam.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>
#include <linux/of.h>
#include <plat/regs/global_macros.h>
#include <plat/regs/timer.h>
#include <plat/regs/sys_ctrl.h>

//#define DEBUG

#ifdef DEBUG
#define DBG pr_info
#else
#define DBG(...)
#endif

#define RDA_WDT_DEFAULT_CLOCK	32000
#define RDA_WDT_DEFAULT_TIMEOUT 10
#define RDA_WDT_DEFAULT_COUNT	0xffffff
#define RDA_WDT_MIN_COUNT	(RDA_WDT_DEFAULT_CLOCK / 2)
#define RDA_SYSCTRL_REG_CFG_RESERVE 0x1b4

struct rda_wdt_dev {
	void __iomem *base;
	void __iomem *sysctrl;
	struct device *dev;
	HWP_TIMER_T *regs;
	bool rda_wdt_users;
	struct mutex lock;
};

/* WDT private functions */
static void watchdog_rst_sys_enable(struct rda_wdt_dev *wdev)
{
	void __iomem *reg;
	u32 val;

	DBG("%s\n", __func__);

	reg = wdev->sysctrl + RDA_SYSCTRL_REG_CFG_RESERVE;
	val = readl(reg) | SYS_CTRL_WD_1_RST_EN;
	writel(val, reg);
}

static void watchdog_rst_ddr_disable(struct rda_wdt_dev *wdev)
{
	void __iomem *reg;
	u32 val;

	DBG("%s\n", __func__);

	reg = wdev->sysctrl + RDA_SYSCTRL_REG_CFG_RESERVE;
	val = readl(reg) & (~SYS_CTRL_DMC_PHY_RST_EN);
	writel(val, reg);
}

static inline void watchdog_reload(struct rda_wdt_dev *wdev)
{
	writel(TIMER_RELOAD, &wdev->regs->WDTimer_Ctrl);
}

static inline void watchdog_enable(struct rda_wdt_dev *wdev)
{
	writel(TIMER_START, &wdev->regs->WDTimer_Ctrl);
}

static inline void watchdog_disable(struct rda_wdt_dev *wdev)
{
	writel(TIMER_STOP, &wdev->regs->WDTimer_Ctrl);
}

static inline void watchdog_set_timer(struct rda_wdt_dev *wdev,
				      unsigned int count)
{
	if (count < RDA_WDT_MIN_COUNT)
		count = RDA_WDT_DEFAULT_COUNT;

	writel(TIMER_WDLOADVAL(count), &wdev->regs->WDTimer_LoadVal);
}

static inline unsigned int watchdog_is_running(struct rda_wdt_dev *wdev)
{
	unsigned int r = 0;

	if (readl(&wdev->regs->WDTimer_Ctrl) & TIMER_WDENABLED)
		r = 1;
	return r;
}

/* WDT operations functions */
static int rda_wdt_start(struct watchdog_device *wdd)
{
	struct rda_wdt_dev *wdev = watchdog_get_drvdata(wdd);

	DBG("%s\n", __func__);

	mutex_lock(&wdev->lock);
	wdev->rda_wdt_users = true;
	watchdog_disable(wdev);
	watchdog_set_timer(wdev, wdd->timeout * RDA_WDT_DEFAULT_CLOCK);
	watchdog_enable(wdev);
	mutex_unlock(&wdev->lock);
	return 0;
}

static int rda_wdt_stop(struct watchdog_device *wdd)
{
	struct rda_wdt_dev *wdev = watchdog_get_drvdata(wdd);

	DBG("%s\n", __func__);

	mutex_lock(&wdev->lock);
	watchdog_disable(wdev);
	wdev->rda_wdt_users = false;
	mutex_unlock(&wdev->lock);
	return 0;
}

static int rda_wdt_ping(struct watchdog_device *wdd)
{
	struct rda_wdt_dev *wdev = watchdog_get_drvdata(wdd);

	DBG("%s\n", __func__);

	mutex_lock(&wdev->lock);
	if (wdev->rda_wdt_users)
		watchdog_reload(wdev);
	mutex_unlock(&wdev->lock);
	return 0;
}

static int rda_wdt_set_timeout(struct watchdog_device *wdd, unsigned int sec)
{
	struct rda_wdt_dev *wdev = watchdog_get_drvdata(wdd);

	DBG("%s\n", __func__);

	mutex_lock(&wdev->lock);
	if (wdev->rda_wdt_users) {
		watchdog_disable(wdev);
		watchdog_set_timer(wdev, sec * RDA_WDT_DEFAULT_CLOCK);
		watchdog_enable(wdev);
		wdd->timeout = sec;
		DBG("%s set timeout %d\n", __func__, sec);
	}
	mutex_unlock(&wdev->lock);
	return 0;
}

static unsigned int rda_wdt_status(struct watchdog_device *wdd)
{
	struct rda_wdt_dev *wdev = watchdog_get_drvdata(wdd);
	unsigned int status;

	mutex_lock(&wdev->lock);
	status = watchdog_is_running(wdev);
	mutex_unlock(&wdev->lock);

	return status;
}

static const struct watchdog_ops rda_wdt_ops = {
	.owner = THIS_MODULE,
	.start = rda_wdt_start,
	.stop = rda_wdt_stop,
	.ping = rda_wdt_ping,
	.set_timeout = rda_wdt_set_timeout,
};

static const struct watchdog_info rda_wdt_info = {
	.options = WDIOF_KEEPALIVEPING | WDIOF_SETTIMEOUT,
	.identity = "rda watchdog",
};

/* WDT device attributes */
static ssize_t wdt_timeout_show(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	struct watchdog_device *wdd = dev_get_drvdata(dev);
	unsigned int tm;

	tm = wdd->timeout;
	return sprintf(buf, "%d\n", tm);
}

static ssize_t wdt_timeout_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	struct watchdog_device *wdd = dev_get_drvdata(dev);
	unsigned int sec, i;
	unsigned char tmp[10];

	for(i = 0;i < count;i++)
		tmp[i] = buf[i] - 0x30;

	if (count == 1)
		sec = tmp[0];
	else if (count > 1)
		sec = tmp[0] * 10 + tmp[1];
	else
		sec = wdd->timeout;

	DBG("sec = %d\n", sec);

	rda_wdt_set_timeout(wdd, sec);

	return sprintf((char *)buf, "%d\n", sec);
}
static DEVICE_ATTR_RW(wdt_timeout);

static ssize_t wdt_status_show(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	struct watchdog_device *wdd = dev_get_drvdata(dev);
	unsigned int status;

	status = rda_wdt_status(wdd);
	return sprintf(buf, "%d\n", status);
}

static ssize_t wdt_status_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	struct watchdog_device *wdd = dev_get_drvdata(dev);

	if (count > 0) {
		int run = buf[0] - 0x30;

		DBG("run = %d\n", run);
		if (run) {
			rda_wdt_start(wdd);
		} else {
			rda_wdt_stop(wdd);
		}
	}
	return 0;
}
static DEVICE_ATTR_RW(wdt_status);

static struct attribute *rda_wdt_attrs[] = {
	&dev_attr_wdt_status.attr,
	&dev_attr_wdt_timeout.attr,
	NULL,
};

static const struct attribute_group rda_wdt_group = {
	.name = "rda_wdt",
	.attrs = rda_wdt_attrs,
};

static int rda_wdt_init_sysfs(struct device *dev)
{
	int error = 0;

	error = sysfs_create_group(&dev->kobj, &rda_wdt_group);
	if (error)
		printk(KERN_ERR "%s failed %d\n", __func__, error);
	return error;
}

static int rda_wdt_probe(struct platform_device *pdev)
{
	struct watchdog_device *wdd;
	struct rda_wdt_dev *wdev;
	struct resource *res;
	int ret;
	u32 tmsec, rstddr, rstsys;

	DBG("%s\n", __func__);

	if (!pdev->dev.of_node) {
		dev_err(&pdev->dev, "of node not find\n");
		return -ENODEV;
	}

	ret =
	    of_property_read_u32_array(pdev->dev.of_node, "timeout_sec", &tmsec,
				       1);
	if (ret)
		tmsec = RDA_WDT_DEFAULT_TIMEOUT;

	ret =
	    of_property_read_u32_array(pdev->dev.of_node, "rstddr", &rstddr, 1);
	if (ret)
		rstddr = 1;

	ret =
	    of_property_read_u32_array(pdev->dev.of_node, "rstsys", &rstsys, 1);
	if (ret)
		rstsys = 0;

	dev_info(&pdev->dev, "tmsec=%d, rstddr=%d, rstsys=%d\n",
			tmsec, rstddr, rstsys);

	wdd =
	    devm_kzalloc(&pdev->dev, sizeof(struct watchdog_device),
			 GFP_KERNEL);
	if (!wdd) {
		dev_err(&pdev->dev, "alloc wdd failed\n");
		return -ENOMEM;
	}

	wdev = devm_kzalloc(&pdev->dev, sizeof(struct rda_wdt_dev), GFP_KERNEL);
	if (!wdev) {
		dev_err(&pdev->dev, "alloc rda wdev failed\n");
		ret = -ENOMEM;
		goto free_wdd;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "get res failed\n");
		ret = -EINVAL;
		goto free_wdev;

	}

	wdev->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(wdev->base)) {
		dev_err(&pdev->dev, "remap res failed\n");
		ret = -EINVAL;
		goto free_wdev;
	}

	wdev->sysctrl = devm_ioremap(&pdev->dev, REG_SYS_CTRL_BASE, 0x1000);
	if (IS_ERR(wdev->sysctrl)) {
		dev_err(&pdev->dev, "remap sysctrl failed\n");
		ret = -EINVAL;
		goto free_mem;
	}

	wdev->regs = (HWP_TIMER_T *) (wdev->base);
	wdev->dev = &pdev->dev;
	wdev->rda_wdt_users = false;
	mutex_init(&wdev->lock);

	wdd->info = &rda_wdt_info;
	wdd->ops = &rda_wdt_ops;
	wdd->timeout = tmsec;

	watchdog_set_drvdata(wdd, wdev);
	watchdog_set_nowayout(wdd, false);

	watchdog_disable(wdev);

	if (rstsys)
		watchdog_rst_sys_enable(wdev);

	if (!rstddr)
		watchdog_rst_ddr_disable(wdev);

	ret = watchdog_register_device(wdd);
	if (ret) {
		dev_err(&pdev->dev, "remap res failed\n");
		ret = -EFAULT;
		goto free_mem;
	}

	platform_set_drvdata(pdev, wdd);
	rda_wdt_init_sysfs(&pdev->dev);
	dev_info(&pdev->dev, "RDA wdt init done\n");
	return 0;

free_mem:
	devm_iounmap(&pdev->dev, wdev->base);
free_wdev:
	devm_kfree(&pdev->dev, wdev);
free_wdd:
	devm_kfree(&pdev->dev, wdd);
	return ret;
}

static int rda_wdt_remove(struct platform_device *pdev)
{
	struct watchdog_device *wdd = platform_get_drvdata(pdev);
	struct rda_wdt_dev *wdev = watchdog_get_drvdata(wdd);

	DBG("%s\n", __func__);

	rda_wdt_stop(wdd);
	watchdog_unregister_device(wdd);
	devm_iounmap(&pdev->dev, wdev->base);
	devm_kfree(&pdev->dev, wdev);
	devm_kfree(&pdev->dev, wdd);
	platform_set_drvdata(pdev, 0);

	return 0;
}

static void rda_wdt_shutdown(struct platform_device *pdev)
{
	struct watchdog_device *wdd = platform_get_drvdata(pdev);

	DBG("%s\n", __func__);

	rda_wdt_stop(wdd);
}

#ifdef CONFIG_PM
static int rda_wdt_resume(struct device *dev)
{
	struct watchdog_device *wdd = dev_get_drvdata(dev);
	struct rda_wdt_dev *wdev = watchdog_get_drvdata(wdd);

	DBG("%s\n", __func__);

	mutex_lock(&wdev->lock);
	if (wdev->rda_wdt_users) {
		watchdog_disable(wdev);
		watchdog_set_timer(wdev, wdd->timeout * RDA_WDT_DEFAULT_CLOCK);
		watchdog_enable(wdev);
		DBG("%s resumed, timeout %d\n", __func__, wdd->timeout);
	}
	mutex_unlock(&wdev->lock);

	return 0;
}

static int rda_wdt_suspend(struct device *dev)
{
	struct watchdog_device *wdd = dev_get_drvdata(dev);
	struct rda_wdt_dev *wdev = watchdog_get_drvdata(wdd);

	DBG("%s\n", __func__);

	mutex_lock(&wdev->lock);
	if (wdev->rda_wdt_users) {
		watchdog_disable(wdev);
		DBG("%s suspended\n", __func__);
	}
	mutex_unlock(&wdev->lock);

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(rda_wdt_pm_ops, rda_wdt_suspend, rda_wdt_resume);

static const struct of_device_id rda_wdt_match[] = {
	{
		.compatible = "rda,rda-wdt",
	},
	{},
};

static struct platform_driver rda_wdt_driver = {
	.probe = rda_wdt_probe,
	.remove = rda_wdt_remove,
	.shutdown = rda_wdt_shutdown,
	.driver = {
		.name = "rda-wdt",
		.of_match_table = rda_wdt_match,
		.owner = THIS_MODULE,
		.pm = &rda_wdt_pm_ops,
	},
};

static int __init rda_wdt_init_driver(void)
{
	return platform_driver_register(&rda_wdt_driver);
}

static void __exit rda_wdt_exit_driver(void)
{
	platform_driver_unregister(&rda_wdt_driver);
}

module_init(rda_wdt_init_driver);
module_exit(rda_wdt_exit_driver);

MODULE_AUTHOR("Daqing Zhang");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("rda watchdog driver");
MODULE_ALIAS("platform:rda_wdt");
