/*************************************************************************
	> File Name: key_drv.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2019年05月22日 星期三 13时48分13秒
 ************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/interrupt.h>

#define DEVICE "key"

static int major;
static struct class *key_class;
static struct device *dev;

static DECLARE_WAIT_QUEUE_HEAD(key_waitq);      //定义并初始化等待队列
EXPORT_SYMBOL(key_waitq);
static int ev_press;                                   //中断事件标志
EXPORT_SYMBOL(ev_press);

struct key
{
	volatile u32 *gpfcon;
	volatile u32 *gpfdat;
	u32 pin[4];
};

static struct key mykey;

static int key_open(struct inode *inode, struct file *file)
{
	int i;
	u32 temp = readl(mykey.gpfcon);
	for (i = 0; i < ARRAY_SIZE(mykey.pin); i++)
	{
		temp |= (3 << (mykey.pin[i] * 2));		//gpf0,1,2,4中断
	}
	writel(temp, mykey.gpfcon);
	return 0;
}
	
extern int irq_id;
static ssize_t key_read(struct file *file, const char __user *buf,
						size_t count, loff_t *ppos)
{
	wait_event_interruptible(key_waitq,ev_press);//如果ev_press=0,将会休眠线程      
	ev_press=0;                                 //没有发生中断
	int err = copy_to_user(buf, &irq_id, sizeof(irq_id));
	return err? -EFAULT : 0;
}

extern struct device *exint_dev;
extern int irq[5];
static char *dev_id[5] = { "key-1", "key-2", "key-3", "key-4", "key-8" };
static int key_release(struct inode *inode, struct file *file)
{
	int i;
	for (i = 0; i < 5; i++)
		devm_free_irq(exint_dev, irq[i], dev_id[i]);
	return 0;
}

static struct file_operations key_fops = 
{
	.open = key_open,
	.read = key_read,
	.release = key_release,
};

static int key_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct decice_node *node = pdev->dev.of_node;

	printk(KERN_INFO "%s key found\n", __FUNCTION__);
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res)
	{
		mykey.gpfcon = ioremap(res->start, res->end - res->start + 1);
		mykey.gpfdat = mykey.gpfcon + 1;
	}
	else
		printk(KERN_ERR "Can't get the %s resource!\n", DEVICE);

	int i = 0;
	of_property_read_u32_array(node, "pin", mykey.pin, ARRAY_SIZE(mykey.pin));
	for (; i < ARRAY_SIZE(mykey.pin); ++i)
	{
		printk(KERN_INFO "mykey.pin[%d] = %d\n", i, mykey.pin[i]);
	}
	
	major = register_chrdev(0, DEVICE, &key_fops);
	key_class = class_create(THIS_MODULE, DEVICE);
	dev = device_create(key_class, NULL, MKDEV(major, 0), NULL, DEVICE);
	return 0;
}

static int key_remove(struct platform_drvice *pdev)
{
	printk(KERN_INFO "%s remove %s\n", __FUNCTION__, DEVICE);
	device_unregister(dev);
	class_destroy(key_class);
	unregister_chrdev(major, DEVICE);
	iounmap(mykey.gpfcon);
	return 0;
}

static const struct of_device_id key_table[] =
{
	{ .compatible = "key", .data = NULL, },
	{ }
};

static struct platform_driver key_drv = 
{
	.probe = key_probe,
	.remove = key_remove,
	.driver = 
	{
		.name = "key_drv",
		.of_match_table = key_table
	},
};

static int __init key_drv_init(void)
{
	return platform_driver_register(&key_drv);
}

static void __exit key_drv_exit(void)
{
	platform_driver_unregister(&key_drv);
}

module_init(key_drv_init);
module_exit(key_drv_exit);
MODULE_LICENSE("GPL");

