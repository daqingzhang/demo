#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/printk.h>
#include <asm/uaccess.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#include <linux/ioctl.h>

#define RDAQUEUE_MAX_SIZE (1024 * 8)
#define RDAQUEUE_DEVS	4

struct rda_queue {
	void *pdata;
	u32 i_idx;
	u32 o_idx;
	u32 size;
};

struct rda_queue_dev {
	dev_t dev_num;
	const char *name;
	struct rda_queue *q;
	struct rda_queue_ops *qops;
	struct cdev cdev;
	spinlock_t lock;
	//struct device dev;
};

struct rda_queue_tag {
	const char *name;
	const char *proc_name;
	struct rda_queue_dev *dev;
};

static struct rda_queue_tag rda_qtag[] = {
	{.name = "rdaqueue0",.proc_name = "driver/rdaqueue0",.dev = NULL},
	{.name = "rdaqueue1",.proc_name = "driver/rdaqueue1",.dev = NULL},
	{.name = "rdaqueue2",.proc_name = "driver/rdaqueue2",.dev = NULL},
	{.name = "rdaqueue3",.proc_name = "driver/rdaqueue3",.dev = NULL},
};

static unsigned int total_rda_qdevs = 0;

struct rda_queue_ops {
	int (*init)(struct rda_queue *q,u32 size);
	int (*enqueue)(struct rda_queue *q, void *p,int size);
	int (*dequeue)(struct rda_queue *q, void *p,int size);
	int (*destroy)(struct rda_queue *q);
	void (*clear)(struct rda_queue *q);
	void (*show)(struct rda_queue *q);
};

static int queue_init(struct rda_queue *q,u32 size)
{
	u8 *buf;

	q->pdata = NULL;
	q->size  = 0;
	q->i_idx = 0;
	q->o_idx = 0;
	if(!size)
		return 0;

	buf = kzalloc(sizeof(unsigned char), GFP_KERNEL);
	if(!buf)
		return -ENOMEM;
	q->pdata = buf;
	q->size = size;
	pr_info("%s, q->size = %d bytes\n",__func__,size);
	return 0;
}

static int queue_enqueue(struct rda_queue *q, void *p, int size)
{
	int r;

	pr_info("%s, size = %d, q->size = %d, q->i_idx = %d, q->o_idx = %d\n",
		__func__,size, q->size,q->i_idx,q->o_idx);

	if(size > (q->size - q->i_idx))
		size = q->size - q->i_idx;
		//return -EINVAL;

	r = copy_from_user(q->pdata + q->i_idx, p, size);
	q->i_idx += size;

	pr_info("%s, %d bytes enqueued\n",__func__,size);
	return r;
}

static int queue_dequeue(struct rda_queue *q,void *p, int size)
{
	int r;

	pr_info("%s, size = %d, q->size = %d, q->i_idx = %d, q->o_idx = %d\n",
		__func__,size, q->size,q->i_idx,q->o_idx);

	if(size > q->i_idx)
		size = q->i_idx;
		//return -EINVAL;
	if(size > q->size)
		size = q->size;
		//return -EINVAL;

	r = copy_to_user(p, q->pdata + q->o_idx, size);

	q->o_idx += size;
	if(q->o_idx >= q->i_idx)
		q->i_idx = 0;
		q->o_idx = 0;
	pr_info("%s, %d bytes dequeued\n",__func__,size);
	return r;
}

static int queue_destroy(struct rda_queue *q)
{
	if(!q->size)
		return 0;
	if(q->pdata)
		kfree(q->pdata);
	return 0;
}

static void queue_clear(struct rda_queue *q)
{
	q->i_idx = 0;
	q->o_idx = 0;
}

static void queue_show(struct rda_queue *q)
{
	pr_info("%s, size = %d, i_idx = %d, o_idx = %d\n",
		__func__,q->size,q->i_idx,q->o_idx);
}

static struct rda_queue_ops queue_ops = {
	.init	 = queue_init,
	.enqueue = queue_enqueue,
	.dequeue = queue_dequeue,
	.destroy = queue_destroy,
	.clear   = queue_clear,
	.show    = queue_show,
};

/*
 *********************************************************************
 * seq file drivers
 *********************************************************************
 */
static int rda_seq_show(struct seq_file *m, void *v)
{
	struct rda_queue_dev *qdev = m->private;
	int major,minor;

	seq_printf(m, "%s\n",__func__);

	if(qdev) {
		major = MAJOR(qdev->dev_num);
		minor = MINOR(qdev->dev_num);
		seq_printf(m,"%s, major = %d, minor = %d\n",
				qdev->name,major,minor);
		seq_printf(m,"data: i_idx = %d, o_idx = %d, size = %d\n",
				qdev->q->i_idx,qdev->q->o_idx,qdev->q->size);
	}
	return 0;
}

static int proc_qdev_open(struct inode *node, struct file *fp)
{
	struct rda_queue_dev *dev = PDE_DATA(node);//get the proc directory entry's data
	return single_open(fp,rda_seq_show,dev);
}

static struct file_operations qdev_proc_ops = {
	.owner	= THIS_MODULE,
	.open	= proc_qdev_open,
	.read	= seq_read,
	.llseek	= seq_lseek,
	.release= single_release,
};

static int proc_qdev_create(const char *name,void *data)
{
	struct proc_dir_entry *entry;
	entry = proc_create_data(name,0,NULL,&qdev_proc_ops,data);
	if(!entry)
		return -1;
	return 0;
}

static int proc_qdev_remove(void)
{
	int i;
	for(i = 0;i < ARRAY_SIZE(rda_qtag);i++)
		remove_proc_entry(rda_qtag[i].proc_name,NULL);
	return 0;
}

static int qdev_open(struct inode *node, struct file *fp)
{
	struct cdev *cdev = node->i_cdev;
	struct rda_queue_dev *qdev = container_of(cdev,struct rda_queue_dev,cdev);

	fp->private_data = qdev;
	pr_info("%s, done\n",__func__);
	return 0;
}

static ssize_t qdev_read(struct file *fp, char __user *buf, size_t count,loff_t *fpos)
{
	struct rda_queue_dev *qdev = fp->private_data;
	struct rda_queue *q = qdev->q;
	int ret;
	unsigned long flags = 0;

	pr_info("%s, count = %d, fpos = %d\n",__func__,count,(int)(*fpos));
	spin_lock_irqsave(&qdev->lock,flags);
	if(count > q->size)
		count = q->size;

	ret = copy_to_user(buf,q->pdata,count);
	if(ret) {
		pr_err("%s, copy data failed, %d\n",__func__,ret);
		return -EFAULT;
	}
	*fpos = count;
	spin_unlock_irqrestore(&qdev->lock,flags);
	return count;
}

static ssize_t qdev_write(struct file *fp, const char __user *pdata, size_t count, loff_t *fpos)
{
	struct rda_queue_dev *qdev = fp->private_data;
	struct rda_queue *q = qdev->q;
	int ret = 0;
	unsigned long flags = 0;

	pr_info("%s, count = %d, fpos = %d\n",__func__,count,(int)(*fpos));
	spin_lock_irqsave(&qdev->lock,flags);
	if(count > q->size)
		count = q->size;

	ret = copy_from_user(q->pdata, pdata, count);
	if(ret) {
		pr_err("%s, copy data error, %d\n",__func__,ret);
		return -EFAULT;
	}
	*fpos = count;
	spin_unlock_irqrestore(&qdev->lock,flags);
	return count;
}

#if 0
static ssize_t qdev_read(struct file *fp, char __user *buf, size_t len,loff_t *pos)
{
	struct rda_queue_dev *qdev = fp->private_data;
	int ret = -EFAULT;

	if(qdev->qops->dequeue)
		ret = qdev->qops->dequeue(qdev->q,buf,len);
	if(!ret)
		*pos += len;
	pr_info("%s, read %d bytes, %d\n",__func__,len,ret);
	return ret;
}

static ssize_t qdev_write(struct file *fp, const char __user *pdat, size_t len, loff_t *pos)
{
	struct rda_queue_dev *qdev = fp->private_data;
	int ret = -EFAULT;

	if(qdev->qops->enqueue)
		ret = qdev->qops->enqueue(qdev->q,(void *)pdat,len);
	if(!ret)
		*pos += len;
	pr_info("%s, write %d bytes, %d\n",__func__,len,ret);
	return ret;
}
#endif
static int qdev_release(struct inode *node, struct file *fp)
{
	pr_info("%s, done\n",__func__);
	return 0;
}

#define RDA_QDEV_MAGIC 'v'
#define QDEV_CMD_SHOW_NAME	_IO(RDA_QDEV_MAGIC,0)
#define QDEV_CMD_RD_SIZE	_IOR(RDA_QDEV_MAGIC,1,struct rda_queue_dev)
#define QDEV_CMD_RD_DEVNUM	_IOR(RDA_QDEV_MAGIC,2,struct rda_queue_dev)
#define QDEV_CMD_RD_DATA	_IOR(RDA_QDEV_MAGIC,3,struct rda_queue_dev)
#define QDEV_CMD_WR_DATA	_IOW(RDA_QDEV_MAGIC,4,struct rda_queue_dev)

static int qdev_do_ioctl(struct rda_queue_dev *dev, unsigned int cmd, unsigned long arg, int kernel)
{
	switch(cmd) {
	case QDEV_CMD_SHOW_NAME:
	{
		pr_info("%s, device name: %s\n",__func__,dev->name);
		break;
	}
	case QDEV_CMD_RD_SIZE:
	{
		void *buf = (void *)arg;
		unsigned long size = dev->q->size;
		int ret;

		pr_info("%s, read size\n",__func__);
		if(!buf) {
			pr_err("%s, arg is null\n",__func__);
			return -EINVAL;
		}
		ret = put_user(size,(unsigned long __user *)buf);
		break;
	}
	case QDEV_CMD_RD_DEVNUM:
	{
		void *buf = (void *)arg;
		unsigned long devnum = dev->dev_num;
		int ret;

		pr_info("%s, read devnum\n",__func__);
		if(!buf) {
			pr_err("%s, arg is null\n",__func__);
			return -EINVAL;
		}
		ret = put_user(devnum,(unsigned long __user *)buf);
		break;
	}
	case QDEV_CMD_RD_DATA:
	{
		struct rda_queue *q = dev->q;
		int size = q->size;
		void *buf = (void *)arg;
		int ret;

		pr_info("%s, read data\n",__func__);
		if(!buf) {
			pr_err("%s, arg is null\n",__func__);
			return -EINVAL;
		}
		if((!q->pdata) || (q->size == 0)) {
			pr_err("%s, data is null or size is zero\n",__func__);
			return -EINVAL;
		}
		ret = copy_to_user(buf,q->pdata,size);
		if(ret) {
			pr_err("%s, copy data error ,%d\n",__func__,ret);
			return -EINVAL;
		}
		break;
	}
	case QDEV_CMD_WR_DATA:
	{
		struct rda_queue *q = dev->q;
		int size = q->size;
		void *buf = (void *)arg;
		int ret = 0;

		pr_info("%s, write data\n",__func__);
		if(!buf) {
			pr_err("%s, arg is null\n",__func__);
			return -EINVAL;
		}
		if((!q->pdata) || (!q->size)) {
			pr_err("%s, data is null or size is zero\n",__func__);
			return -EINVAL;
		}
		ret = copy_from_user(q->pdata,buf,size);
		if(ret) {
			pr_err("%s, copy data error ,%d\n",__func__,ret);
			return -EINVAL;
		}
		break;
	}
	default:
		return -EINVAL;
	}
	return 0;
}

static long qdev_unlocked_ioctl(struct file *fp,unsigned int cmd, unsigned long arg)
{
	long ret;
	struct rda_queue_dev *dev = fp->private_data;

	if(!dev) {
		pr_err("%s, dev is null\n",__func__);
		return -EINVAL;
	}
	ret = qdev_do_ioctl(dev,cmd,arg,0);
	pr_info("%s, cmd = 0x%x, arg = 0x%x, ret = %d\n",
			__func__,cmd,(unsigned int)arg,(int)ret);
	return ret;
}

static struct file_operations rda_qdev_ops = {
	.owner	= THIS_MODULE,
	.open 	= qdev_open,
	.read 	= qdev_read,
	.write	= qdev_write,
	.release= qdev_release,
	.llseek = noop_llseek,
	.unlocked_ioctl = qdev_unlocked_ioctl,
};

static int __init rda_qdev_init(void)
{
	int ret = -ENODEV;
	int major,n,i;
	dev_t dev_num;
	struct device_node *np;
	struct rda_queue_dev *qdev;
	struct rda_queue *pq;

	np = of_find_compatible_node(NULL,NULL,"rda,rda-queue");
	if(!np) {
		pr_err("%s, np is null\n",__func__);
		goto out_err_0;
	}
	n = ARRAY_SIZE(rda_qtag);
	ret = alloc_chrdev_region(&dev_num,0,n,"rdaqueue");
	if(ret < 0) {
		pr_err("%s, alloc dev_num failed, %d\n",__func__,ret);
		goto out_err_0;
	}
	major = MAJOR(dev_num);

	for(i = 0;i < n;i++) {
		pq = kzalloc(sizeof(struct rda_queue),GFP_KERNEL);
		if(!pq) {
			pr_err("%s, kzalloc queue failed\n",__func__);
			goto out_err_1;
		}
		ret = queue_init(pq,RDAQUEUE_MAX_SIZE);
		if(ret) {
			pr_err("%s, queue init failed ,%d\n",__func__,ret);
			goto out_err_2;
		}
		qdev = kzalloc(sizeof(struct rda_queue_dev),GFP_KERNEL);
		if(!qdev) {
			pr_err("%s, kzalloc failed\n",__func__);
			goto out_err_3;
		}
		spin_lock_init(&qdev->lock);

		qdev->name	= rda_qtag[i].name;
		qdev->dev_num	= MKDEV(major,i + total_rda_qdevs);
		qdev->q		= pq;
		qdev->qops	= &queue_ops;

		cdev_init(&qdev->cdev,&rda_qdev_ops);
		ret = cdev_add(&qdev->cdev,qdev->dev_num,1);
		if(ret) {
			pr_err("%s, cdev add failed, %d\n",__func__,ret);
			goto out_err_4;
		}
		pr_info("%s, device number: %x\n",qdev->name,qdev->dev_num);
	
		if(proc_qdev_create(rda_qtag[i].proc_name,qdev)) {
			pr_info("%s, create proc failed\n",__func__);
		}

		rda_qtag[i].dev = qdev;
	}
	total_rda_qdevs += n;
		pr_info("%s, initialized %d queue devices done !\n",__func__,total_rda_qdevs);

	return 0;
out_err_4:
	kfree(qdev);
out_err_3:
	queue_destroy(pq);
out_err_2:
	kfree(pq);
out_err_1:
	unregister_chrdev_region(dev_num,n);
out_err_0:
	return ret;
}
//module_init(rda_qdev_init);
postcore_initcall(rda_qdev_init);

static void __exit rda_qdev_exit(void)
{
	int i,n = 0;
	struct rda_queue_dev *qdev;

	for(i = 0;i < ARRAY_SIZE(rda_qtag);i++) {
		qdev = rda_qtag[i].dev;
		if(!qdev)
			continue;
		unregister_chrdev_region(qdev->dev_num,1);
		cdev_del(&qdev->cdev);
		queue_destroy(qdev->q);
		kfree(qdev->q);
		kfree(qdev);
		n++;
	}
	total_rda_qdevs -= n;
	proc_qdev_remove();
}

module_exit(rda_qdev_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RDA queue device driver");
MODULE_AUTHOR("abc <abc@rdamicro.com>");
