#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <string.h>

#define RDAQUEUE_MAX_DEVS (4)
#define RDAQUEUE_MAX_SIZE (1024)

struct rda_queue {
	void *pdata;
	u32 i_idx;
	u32 o_idx;
	u32 size;
};

struct rda_queue_dev {
	int major;
	int minor;
	const char *name;
	struct rda_queue *q;
	struct rda_queue_ops *ops;
	struct cdev cdev;
	struct device *dev;
};

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
	if(size > q->size)
		return -EINVAL;
	if(size > (q->size - q->i_idx)
		return -EINVAL;

	copy_from_user(q->pdata + p->i_idx, p, size);
	p->i_idx += size;
	return 0;
}

static int queue_dequeue(struct rda_queue *q,void *p, int size)
{
	if(size > q->size)
		return -EINVAL;
	if(size > q->i_idx)
		return -EINVAL;

	copy_to_user(p, q->pdata + q->o_idx, size);

	q->o_idx += size;
	if(q->o_idx >= q->i_idx)
		q->i_idx = 0;
		q->o_idx = 0;
	return 0;
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

struct rda_queue_ops rda_qops = {
	.init	 = queue_init,
	.enqueue = queue_enqueue,
	.dequeue = queue_dequeue,
	.destroy = queue_destroy,
	.clear   = queue_clear,
	.show    = queue_show,
};

static int rda_qdev_open(struct inode *node, struct file *fp)
{

}

static ssize_t rda_qdev_read(struct file *fp, char __user *buf, size_t len,loff_t *pos)
{

}

static ssize_t rda_qdev_write(struct file *fp, const char __user *pdat, size_t len, loff_t *pos)
{

}

int rda_qdev_release(struct inode *node, struct file *fp)
{

}

static struct file_operations fops = {
	.owner = THIS_MODULE;
	.open  = rda_qdev_open,
	.read  = rda_qdev_read,
	.write = rda_qdev_write,
	.release = rda_qdev_release,
};

