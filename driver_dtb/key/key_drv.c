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
#include <linux/slab.h>

#define DEVICE "key"

static int major;
static struct class *key_class;
static struct device *dev;

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
		temp &= ~(3 << (mykey.pin[i] * 2));		//gpf0,1,2,4输入
	}
	writel(temp, mykey.gpfcon);
	return 0;
}
	

static int key_scan(void)
{
	int i;
	u32 temp = readl(mykey.gpfdat);
	mdelay(20);
	for (i = 0; i < ARRAY_SIZE(mykey.pin); i++)
	{
		if (!(temp & (1 << (mykey.pin[i]))))
			return i == 4? i : i + 1;
	}
	return 0;
}

static ssize_t key_read(struct file *file, const char __user *buf,
						size_t count, loff_t *ppos)
{
	int key = key_scan();
	if (key)
		copy_to_user(buf, &key, sizeof(key));
	return 0;
}


static struct file_operations key_fops = 
{
	.open = key_open,
	.read = key_read,
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

