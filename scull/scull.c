#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include <linux/wait.h>

#define CONFIG_SCULL_DEBUG

enum
{
	SCULL_TYPE_N = 0,
	SCULL_TYPE_RO,
	SCULL_TYPE_WO,
	SCULL_TYPE_WR,
};

struct scull_data_block
{
	const char *label;
	unsigned int id;
	void *pdata;
	unsigned int size;
};

struct scull_device
{
	spinlock_t		lock;
	struct cdev		chr_dev;
	struct file_operations 	fops;
	struct scull_data_block *blocks;
	const char *name;
	int id;
	int num;//block number
	int type;
};

#define SCULL_MAJOR	260
#define SCULL_BLOCK_MAX_SIZE	2048
#define SCULL_BLOCK_MAX_NUM	2

static int scull_open(struct inode *node,struct file *filp)
{
	struct scull_device *pdev;
	//unsigned long flags;

	pdev = container_of(node->i_cdev,struct scull_device,chr_dev);
	printk(KERN_ALERT "scull open\n");

//	spin_lock_irqsave(&(pdev->lock),flags);
	filp->private_data = pdev;
//	spin_unlock_irqrestore(&(pdev->lock),flags);
	return 0;
}

static int scull_release(struct inode *node, struct file *filp)
{
	filp->private_data = 0;
	printk(KERN_ALERT "scull release\n");
	return 0;
}

static ssize_t scull_read(struct file *filp, char __user *buf,
				size_t len,loff_t *offs)
{
	struct scull_device *pdev = filp->private_data;
	struct scull_data_block *ptr_blk = pdev->blocks;
	unsigned long pos = *offs,count = len;

//	spin_lock_irqsave(&(pdev->lock),flags);

	if(pos >= ptr_blk->size)
		return 0;//count is 0
	if(count > ptr_blk->size - pos)
		count = ptr_blk->size - pos;

	if(copy_to_user(buf, ptr_blk->pdata + pos, count))
		return -EFAULT;
	else
		*offs = pos + count;

//	spin_unlock_irqrestore(&(pdev->lock),flags);
	printk(KERN_ALERT "%s, %s read %d bytes from device, offs = %d\n",
			__func__,
			pdev->name,
			(int)count,
			(int)(*offs));
	return count;
}

static ssize_t scull_write(struct file *filp, const char __user *data,
				size_t len,loff_t *offs)
{
	struct scull_device *pdev = filp->private_data;
	struct scull_data_block *ptr_blk = pdev->blocks;
	unsigned long pos = *offs, count = len;

//	spin_lock_irqsave(&(pdev->lock),flags);

	if(pos >= ptr_blk->size)
		return 0;
	if(count > ptr_blk->size - pos)
		count = ptr_blk->size - pos;

	if(copy_from_user(ptr_blk->pdata + pos, data, count))
		return -EFAULT;
	else
		*offs = pos + count;

//	spin_unlock_irqrestore(&(pdev->lock),flags);
	printk(KERN_ALERT "%s, %s write %d bytes to device, offs = %d\n",
			__func__,
			pdev->name,
			(int)count,
			(int)(*offs));
	return count;
}
#if 0
static ssize_t sleepy_read(struct file *filp, char __user *buf,
				size_t count,loff_t *pos)
{
	struct scull_device *ptr_scull = filp->private_data;

	printk(KERN_ALERT "sleepy_read, wait event,suspend task\n");
	wait_event_interruptible(ptr_scull->wqueue,ptr_scull->flag != 0);
	ptr_scull->flag = 0;
	printk(KERN_ALERT "sleepy_read, evnet come,resume task\n");
	return 0;
}

static ssize_t sleepy_write(struct file *filp, char __user *data,
				size_t count,loff_t *pos)
{
	struct scull_devie *ptr_scull = filp->private_data;

	printk(KERN_ALERT "sleepy write, wake up task\n");
	scull_device->flag = 1;
	wake_up_interruptible(&ptr_scull->wqueue);
	return count;
}
#endif
enum SCULL_DEV_LLSEEK_CMD
{
	LLS_SET_POS = 0,
	LLS_CUR_POS,
	LLS_END_POS,
};

static loff_t scull_llseek(struct file *filp, loff_t offs, int cmd)
{
	loff_t newpos;

	switch(cmd) {
	case LLS_SET_POS:
		newpos = offs;
		break;
	case LLS_CUR_POS:
		newpos = filp->f_pos + offs;
		break;
	case LLS_END_POS:
		newpos = SCULL_BLOCK_MAX_SIZE - 1 + offs;
		break;
	default:
		return -EINVAL;
	}
	if((newpos < 0) || (newpos > SCULL_BLOCK_MAX_SIZE))
		return -EINVAL;

	filp->f_pos = newpos;

	printk(KERN_ALERT "scull llseek f_pos = %d\n",(int)newpos);
	return newpos;
}

#define ERR_SCULL_INI_DATA	2
#define ERR_SCULL_INI_CDEV	3

#define SCULL_DEV_FOPS_INIT(_owner,_open,_read,_write,_llseek,_release)	\
{				\
	.owner	= _owner,	\
	.open	= _open,	\
	.read	= _read,	\
	.write	= _write,	\
	.llseek	= _llseek,	\
	.release= _release,	\
}

struct scull_device scull_dev[] = {
	[0] = {
		.name = "scull0",
		.id   = 0,
		.num  = SCULL_BLOCK_MAX_NUM,
		.type = SCULL_TYPE_WR,
		.fops = SCULL_DEV_FOPS_INIT(
			THIS_MODULE,
			scull_open,
			scull_read,
			scull_write,
			scull_llseek,
			scull_release),
	},
	[1] = {
		.name = "scull1",
		.id    = 1,
		.num  = SCULL_BLOCK_MAX_NUM,
		.type = SCULL_TYPE_WR,
		.fops = SCULL_DEV_FOPS_INIT(
			THIS_MODULE,
			scull_open,
			scull_read,
			scull_write,
			scull_llseek,
			scull_release),
	},
};

static int scull_setup_cdev(struct scull_device *scull)
{
	int err, devno = MKDEV(SCULL_MAJOR,scull->id);
	struct cdev *ptr_chr = &scull->chr_dev;
	struct file_operations *ptr_fops = &scull->fops;

	cdev_init(ptr_chr,ptr_fops);
	ptr_chr->owner = THIS_MODULE;
	ptr_chr->ops = &scull->fops;
	err = cdev_add(ptr_chr,devno,1);
	if(err)
		goto end_setup;

	printk(KERN_ALERT "%s setup cdev done\n",scull->name);
	return 0;
end_setup:
	printk(KERN_ALERT "%s setup cdev failed, %d\n",scull->name,err);
	cdev_del(&scull->chr_dev);
	return -1;
}

static void scull_free_cdev(struct scull_device *scull)
{
	cdev_del(&scull->chr_dev);
	printk(KERN_ALERT "%s free cdev done\n",scull->name);
}

static int scull_setup_block(struct scull_device *scull)
{
	int i;
	struct scull_data_block *ptr_blocks;

	ptr_blocks = kmalloc((scull->num) * sizeof(struct scull_data_block),GFP_KERNEL);
	if(ptr_blocks == NULL)
		return -1;

	for(i = 0;i < scull->num;i++) {
		ptr_blocks[i].id    = i+1;
		ptr_blocks[i].size  = SCULL_BLOCK_MAX_SIZE;
		ptr_blocks[i].pdata  = kmalloc(ptr_blocks[i].size,GFP_KERNEL);
		if(ptr_blocks[i].pdata == NULL)
			return -1;
		memset(ptr_blocks[i].pdata,0,ptr_blocks[i].size);

		printk(KERN_ALERT "%s setup block %d, addr = %x, size = %d\n",
			scull->name,i,
			(unsigned int)(ptr_blocks[i].pdata),ptr_blocks[i].size);
	}
	scull->blocks = ptr_blocks;

	printk(KERN_ALERT "%s setup %d blocks, addr = %x",
		scull->name,scull->num,(unsigned int)(ptr_blocks));
	return 0;
}

static int scull_free_data(struct scull_device *scull)
{
	struct scull_data_block *ptr_blk = scull->blocks;
	int i;
	for(i = 0;i < scull->num;i++) {
		kfree(ptr_blk[i].pdata);
		printk(KERN_ALERT "free %s block %d done\n",
			scull->name,ptr_blk[i].id);
	}
	kfree(ptr_blk);
	printk(KERN_ALERT "free %s all block done\n",scull->name);
	return 0;
}

static int __init scull_dev_init(void)
{
	int i;
	for(i = 0;i < ARRAY_SIZE(scull_dev);i++) {
		/* init spin lock */
		spin_lock_init(&scull_dev[i].lock);
		/* init data */
		if(scull_setup_block(&scull_dev[i]))
			goto end_init_data;
		/* init cdev */
		if(scull_setup_cdev(&scull_dev[i]))
			goto end_init_cdev;
	}
	printk(KERN_ALERT "scull module initialized\n");
	return 0;

end_init_cdev:
	scull_free_data(&scull_dev[i]);
	return -ERR_SCULL_INI_CDEV;

end_init_data:
	return -ERR_SCULL_INI_DATA;
}

static void __exit scull_dev_exit(void)
{
	int i;
	for(i = 0;i < ARRAY_SIZE(scull_dev);i++) {
		scull_free_cdev(&scull_dev[i]);
		scull_free_data(&scull_dev[i]);
	}
	printk(KERN_ALERT "scull module exit\n");
}

module_init(scull_dev_init);
module_exit(scull_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TariqZhang");
MODULE_VERSION("V1.0");
MODULE_DESCRIPTION("scull device module");
