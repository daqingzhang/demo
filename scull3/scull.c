#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <plat/rda_debug.h>
#include <plat/rda_scull.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#ifdef RDA_DBG_SCULL
#define scull_debug rda_dbg_scull
#else
#define scull_debug(...) do{}while(0)
#endif

struct scull_queue
{
	unsigned int head;
	unsigned int next;
	unsigned int size;
	void *qdata;
};

struct scull_device
{
	int type;
	int major;
	int minor;
	spinlock_t lock;
	struct cdev chr_dev;
	struct scull_queue queue;
};

/*
 * SCULL Device Driver
 */
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

static int scull_release(struct inode *inode, struct file *f)
{
	scull_debug("%s, scull proc release\n",__func__);
	return 0;
}


static struct file_operations scull_fops = {
	.open   = scull_open,
	.flush  = scull_flush,
	.read   = scull_read,
	.write  = scull_write,
	.llseek = scull_seek,
	.release = scull_release,
};

#define SCULL_DEV_NR 2
static struct scull_device scull_dev[SCULL_DEV_NR];
static const char *scull_proc_name[] = {
	"driver/scull0",
	"driver/scull1",
};

/*
 * SCULL Platform Private Driver
 */
static int scull_setup_cdev(struct scull_device *pdev,struct file_operations *ops)
{
	int err, devno = MKDEV(pdev->major,pdev->minor);
	struct cdev *ptr_chr = &pdev->chr_dev;

	pdev->chr_dev.ops = ops;
	cdev_init(ptr_chr,ops);
	ptr_chr->owner = THIS_MODULE;
	err = cdev_add(ptr_chr,devno,1);
	if(err)
		goto end_setup;
	scull_debug("scull setup cdev %d, %d done\n",pdev->major,pdev->minor);
	return 0;
end_setup:
	scull_debug("scull setup cdev failed, %d\n",err);
	cdev_del(&pdev->chr_dev);
	return -EFAULT;
}
#if 0
static void scull_free_cdev(struct scull_device *pdev)
{
	cdev_del(&pdev->chr_dev);
	scull_debug("scull free cdev %d, %d done\n",pdev->major, pdev->minor);
}
#endif
static int scull_setup_queue(struct scull_device *pdev,int queue_size)
{
	struct scull_queue *q = &pdev->queue;
	void *pbuf;

	if(queue_size <= 0) {
		scull_debug("queue size is invalid\n");
		return -EINVAL;
	}
	pbuf = kzalloc(queue_size, GFP_KERNEL);
	if(!pbuf) {
		scull_debug("alloc queue buffer failed\n");
		return -EFAULT;
	}
	q->qdata = pbuf;
	q->size  = queue_size;
	q->head  = 0;
	q->next  = 0;

	return 0;
}

static void scull_free_queue(struct scull_device *pdev)
{
	struct scull_queue *q = &pdev->queue;
	void *qdata = q->qdata;

	kfree(qdata);
	q->size = 0;
	scull_debug("free queue buffer done\n");
}

/*
 * SCULL Proc Driver
 */
#if 0
void *scull_seq_start(struct seq_file *sfile, loff_t *pos)
{
	scull_debug("%s\n",__func__);
	if(*pos >= SCULL_DEV_NR)
		return NULL;
	return (scull_dev + *pos);
}

void scull_seq_stop(struct seq_file *sfile, void *v)
{
	scull_debug("%s\n",__func__);
}

void *scull_seq_next(struct seq_file *sfile, void *v, loff_t *pos)
{
	scull_debug("%s\n",__func__);
	*pos = *pos + 1;
	if(*pos >= SCULL_DEV_NR)
		return NULL;
	return (scull_dev + *pos);
}

static int scull_seq_show(struct seq_file *sfile, void *v);

static struct seq_operations scull_seq_ops = {
	.start = scull_seq_start,
	.stop  = scull_seq_stop,
	.next  = scull_seq_next,
	.show  = scull_seq_show,
};
#endif

static int scull_seq_show(struct seq_file *sfile, void *v)
{
	//struct scull_device *scull = (struct scull_device *)v;
	struct scull_device *scull = sfile->private;
	struct scull_queue *q = &scull->queue;
	unsigned int *pdat = q->qdata;
	int i,len;
	unsigned long flags;

	scull_debug("%s\n",__func__);
	spin_lock_irqsave(&scull->lock,flags);
	seq_printf(sfile, "major = %d, minor = %d, type = %d, head = %d, next = %d, size = %d\n",
			scull->major,scull->minor,scull->type,
			q->head,q->next,q->size);
	//len = q->size;
	len = 32;
	for(i = 0;i < len;i++) {
		seq_printf(sfile, "%x ",pdat[i]);
		if((i + 1) % 8 == 0)
			seq_printf(sfile,"\n");
	}
	spin_unlock_irqrestore(&scull->lock,flags);
	return 0;
}

static int scull_proc_open(struct inode *inode, struct file *f)
{
	//return seq_open(f, &scull_seq_ops);
	struct scull_device *dev = PDE_DATA(inode);
	return single_open(f,scull_seq_show,dev);
}

static struct file_operations scull_proc_ops = {
	.owner = THIS_MODULE,
	.open  = scull_proc_open,
	.read  = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static void scull_create_proc(struct scull_device *dev)
{
//	proc_create(scull_proc_name[dev->minor],0,NULL,&scull_proc_ops);

	proc_create_data(scull_proc_name[dev->minor], 	//name
				0644,			//mode
				NULL,			//parent
				&scull_proc_ops,	//ops
				(void *)dev);		//data
	scull_debug("%s, scull create proc done\n",__func__);
}

/*
 * SCULL Platform Public Driver
 */
static int scull_probe(struct platform_device *pdev)
{
	struct resource *mem;
	int r;
	struct scull_platdata *pdata;
	struct scull_device *pscull;

	mem = platform_get_resource(pdev,IORESOURCE_MEM,0);
	if(!mem) {
		dev_err(&pdev->dev,"no scull memory resource\n");
		return -ENODEV;
	}
	pdata = (struct scull_platdata *)pdev->dev.platform_data;
	if(!pdata) {
		dev_err(&pdev->dev, "no scull platform data\n");
		return -ENODEV;
	}
#if 0
	pscull = kzalloc(sizeof(struct scull_device),GFP_KERNEL);
#else
	if(pdata->minor >= SCULL_DEV_NR) {
		dev_err(&pdev->dev, "invalid scull minor value\n");
		return -EFAULT;
	}
	pscull = &scull_dev[pdata->minor];
#endif
	spin_lock_init(&pscull->lock);
	pscull->type  = pdata->type;
	pscull->major = pdata->major;
	pscull->minor = pdata->minor;

	r = scull_setup_queue(pscull,pdata->size);
	if(r) {
		dev_err(&pdev->dev, "scull setup queue failed, %d\n",r);
		goto end_setup_queue;
	}
	r = scull_setup_cdev(pscull,&scull_fops);
	if(r) {
		dev_err(&pdev->dev, "scull setup cdev failed, %d\n",r);
		goto end_setup_cdev;
	}
	platform_set_drvdata(pdev,pscull);

	scull_create_proc(pscull);
	dev_info(&pdev->dev, "scull probe done, major: %d, minor: %d, size: %d\n",
		pscull->major,pscull->minor,pscull->queue.size);
	return 0;

end_setup_cdev:
	scull_free_queue(pscull);
end_setup_queue:
	return r;
}

static int scull_remove(struct platform_device *pdev)
{
//	struct scull_platdata *pdata = pdev->dev.platform_data;
//	struct scull_device *pscull = pdata->priv;

//	scull_free_cdev(pscull);
//	scull_free_queue(pscull);
//	kfree(pscull);
	scull_debug("%s, scull remove done\n",__func__);
	return 0;
}

static void scull_shutdown(struct platform_device *pdev)
{
	scull_debug("%s, scull shutdown done\n",__func__);
}

static int scull_suspend(struct platform_device *pdev, pm_message_t state)
{
	scull_debug("%s, scull suspend done\n",__func__);
	return 0;
}

static int scull_resume(struct platform_device *pdev)
{
	scull_debug("%s, scull resume done\n",__func__);
	return 0;
}

static struct platform_driver scull_driver = {
	.probe		= scull_probe,
	.remove		= scull_remove,
	.suspend	= scull_suspend,
	.shutdown	= scull_shutdown,
	.resume		= scull_resume,
	.driver	= {
		.name  = "rda-scull",
		.owner = THIS_MODULE,
	},
};

static __init int scull_module_init(void)
{
	int r;
	r = platform_driver_register(&scull_driver);
	return r;
}

static __exit void scull_module_exit(void)
{
	platform_driver_unregister(&scull_driver);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DaqingZhang");
MODULE_DESCRIPTION("Scull Module");

module_init(scull_module_init);
module_exit(scull_module_exit);
