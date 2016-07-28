#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include <linux/platform_device.h>
#include <linux/errno.h>

#define scull_debug(...) do{}while(0)

#define SCULL_DATA_BUFFER_SIZE (2*1024)
#define SCULL_MAJOR	260

struct scull_queue
{
	unsigned int head;
	unsigned int next;
	unsigned int size;
	void *pdata;
};

struct scull_device
{
	const char *name;
	int minor;
	int major;
	int type;
	struct cdev chr_dev;
	struct scull_queue *queue;
	spinlock_t lock;
};

static int scull_open(struct inode *node, struct file *f)
{
	scull_debug("%s, scull open\n",__func__);
	return 0;
}

static loff_t scull_seek(struct file *f, loff_t offs, int cmd)
{
	scull_debug("%s, scull seek\n",__func__);
	return 0;
}

static int scull_flush(struct file *f, fl_owner_t id)
{
	scull_debug("%s, scull flush\n",__func__);
	return 0;
}

static ssize_t scull_read(struct file *f, char __user *pbuf, size_t len, loff_t *poffs)
{
	scull_debug("%s, scull read\n",__func__);
	return 0;
}

static ssize_t scull_write(struct file *f, const char __user *pdata, size_t len, loff_t *poffs)
{
	scull_debug("%s, scull module write\n",__func__);
	return 0;
}

static struct file_operations scull_fops = {
	.open   = scull_open,
	.flush  = scull_flush,
	.read   = scull_read,
	.write  = scull_write,
	.llseek = scull_seek,
};

static struct scull_device scull_dev[] = {
	[0] = {
		.name	= "scull0",
		.minor	= 0,
		.major	= SCULL_MAJOR,
		.type	= 0,
		.queue	= (void *)0,
		.chr_dev = {
			.ops = &scull_fops,
		},
	},
	[1] = {
		.name	= "scull1",
		.minor	= 1,
		.major	= SCULL_MAJOR,
		.type	= 0,
		.queue	= (void *)0,
		.chr_dev = {
			.ops = &scull_fops,
		},
	},
};

static int scull_setup_cdev(struct scull_device *pdev)
{
	int err, devno = MKDEV(SCULL_MAJOR,pdev->minor);
	struct cdev *ptr_chr = &pdev->chr_dev;
	const struct file_operations *fops = pdev->chr_dev.ops;

	cdev_init(ptr_chr,fops);
	ptr_chr->owner = THIS_MODULE;
	err = cdev_add(ptr_chr,devno,1);
	if(err)
		goto end_setup;
	scull_debug("%s setup cdev done\n",pdev->name);
	return 0;
end_setup:
	scull_debug("%s setup cdev failed, %d\n",pdev->name,err);
	cdev_del(&pdev->chr_dev);
	return -EFAULT;
}

static void scull_free_cdev(struct scull_device *pdev)
{
	cdev_del(&pdev->chr_dev);
	scull_debug("%s free cdev done\n",pdev->name);
}

static int scull_setup_queue(struct scull_device *pdev)
{
	return 0;
}

static void scull_free_queue(struct scull_device *pdev)
{

}

static __init int scull_module_init(void)
{
	int i,r;
	for(i = 0;i < ARRAY_SIZE(scull_dev);i++) {
		spin_lock_init(&scull_dev[i].lock);
		r = scull_setup_queue(&scull_dev[i]);
		if(r) {
			scull_debug("%s, setup queue error, %d\n",__func__,r);
			goto end_setup_queue;
		}
		r = scull_setup_cdev(&scull_dev[i]);
		if(r) {
			scull_debug("%s, setup cdev error, %d\n",__func__,r);
			goto end_setup_cdev;
		}
		printk(KERN_ALERT "%s, scull  init %d\n",__func__,i);
	}
	printk(KERN_ALERT "%s, scull module initialized\n",__func__);

//	scull_debug("%s, scull module initialized\n",__func__);
	return 0;
end_setup_cdev:
	scull_free_queue(&scull_dev[i]);
end_setup_queue:
	return -EFAULT;
}

static __exit void scull_module_exit(void)
{
	int i;
	for(i = 0;i < ARRAY_SIZE(scull_dev);i++) {
		scull_free_cdev(&scull_dev[i]);
		scull_free_queue(&scull_dev[i]);
	}
//	scull_debug("%s, scull module exit\n",__func__);
	printk(KERN_ALERT "%s, scull module exit\n",__func__);
}

module_init(scull_module_init);
module_exit(scull_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DaqingZhang");
MODULE_DESCRIPTION("Scull Module");
