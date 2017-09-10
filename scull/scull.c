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

#define SCULL_BLOCK_SIZE	1024
#define SCULL_BLOCK_NUM		1

struct scull_block {
	void *buf;
	int size;
};

struct scull_data {
	struct scull_block *block;
	int num;
};

struct scull_device {
	int			id;
	const char 		*name;
	struct scull_data 	*data;
	struct cdev		chrdev;
	struct file_operations 	fops;
	spinlock_t		lock;
};

struct scull_info {
	struct scull_dev *dev;
	int nr;
};

static struct scull_info scuinfo;

static void init_scull_info()
{
	scuinfo.dev = NULL;
	scuinfo.nr = 0;
}

static void set_scull_info(struct scull_device *dev, int nr)
{
	scuinfo.dev = dev;
	scuinfo.nr = 0;
}

static void get_scull_info(struct scull_device **dev, int *nr)
{
	if(dev)
		*dev = scuinfo.dev;
	if(nr)
		*nr = scuinfo.nr;
}

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
	struct scull_data *ptr_blk = pdev->blocks;
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
	struct scull_data *ptr_blk = pdev->blocks;
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
	struct scull_data *ptr_blocks;

	ptr_blocks = kmalloc((scull->num) * sizeof(struct scull_data),GFP_KERNEL);
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

#define to_scull_block(s)	((s)->data->block)
#define to_block_num(s)		((s)->data->num)
#define to_block_buf(b)		((b)->buf)
#define to_buf_size(b)		((b)->size)

static int scull_del_data(struct scull_device *scull)
{
	struct scull_block *blk = to_scull_block(scull);
	int num = to_block_num(s);
	int i;

	for(i = 0;i < num;i++) {
		void *pbuf = &blk[i];
		kfree(pbuf);
		pr_info("%s, block buf %p freed\n", __func__, pbuf);
	}
	kfree(blk);
	pr_info("%s, block %p freed\n", __func__, blk);
	return 0;
}

static int __init scull_dev_init(void)
{
	int ret;
	struct scull_device *scu;

	init_scull_info();

	scu = kzalloc(sizeof(struct scull_device), GFP_KERNEL);
	if(!scu) {
		pr_err("%s, alloc scull failed\n", __func__);
		return -ENOMEM;
	}

	pr_info("%s, scu(%p)\n", __func__, scu);
	set_scull_info(scu, 1);
#if 1
	get_scull_info(&scu, &ret);
	pr_info("%s, get scu(%p), ret=%d\n", __func__, scu, ret);
#endif
	spin_lock_init(&scu->lock);

	ret = scull_setup_data(scu);
	if(ret) {
		pr_err("%s, setup data failed\n", __func__);
		goto fail_free_mem;
	}
	ret = scull_setup_cdev(scu);
	if(ret) {
		pr_err("%s, setup cdev failed\n", __func__);
		goto fail_del_data;
	}
	pr_info("%s, done\n", __func__);
	return 0;

fail_del_data:
	scull_del_data(scu);
fail_free_mem:
	kfree(scu);
	return ret;
}

static void __exit scull_dev_exit(void)
{
	struct scull_device *scu;
	int num;
	int i;

	get_scull_info(&scu, &num);

	pr_info("%s, scu(%p), num=%d\n", __func__, scu, num);

	for(i = 0;i < num;i++) {
		kfree(scu);
		scull_del_data(scu);
	}
}

module_init(scull_dev_init);
module_exit(scull_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tariq");
MODULE_DESCRIPTION("scull device driver");
