#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/sched.h> 
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
 
#include <linux/platform_device.h>
#include <linux/io.h>

#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>

#include <mach/gpio-samsung.h>
#include <linux/of_gpio.h>

static int major;
 
static struct class *cls;
static struct device *dev;
 
static int gpio[4];
static int led_open(struct inode *inode, struct file *file)
{
	int i = 0;
	for (i = 0; i < ARRAY_SIZE(gpio); i++)
	{
		gpio_direction_output(gpio[i], 1);
	}

	return 0;
}
 
static ssize_t led_write(struct file *file, const char __user *buf,
	size_t count, loff_t *ppos)
{
	int val;
	u32 temp;
	copy_from_user(&val, buf, sizeof(val));

	int i;
	for (i = 0; i < 4; i++)
	{
		gpio_set_value(gpio[i], val);
	}

	return 0;
}
 
static struct file_operations led_fops = 
{
	.owner = THIS_MODULE,
	.open  = led_open,
	.write = led_write,
};
 
static int led_probe(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	printk("led_probe, found led\n");

	int i = 0;
	char *name[ARRAY_SIZE(gpio)] = { "led1", "led2", "led3", "led4" };
	for (i = 0; i < ARRAY_SIZE(name); i++)
	{
		gpio[i] = of_get_named_gpio(node, name[i], 0);
		printk(KERN_INFO "gpio[%d] = %d\n", i, gpio[i]);
	}
	

	// 注册设备驱动 创建设备节点
	major = register_chrdev(0, "myled", &led_fops);
	// 创建类
	cls = class_create(THIS_MODULE, "myled");
	// 创建设备节点
	dev = device_create(cls, NULL, MKDEV(major, 0), NULL, "led");
 
	return 0;
}
 
static int led_remove(struct platform_device *pdev)
{
	printk("led_remove, remove led\n");	
	// 删除设备节点
	device_unregister(dev);
	// 销毁类
	class_destroy(cls);
	// 取消注册设备驱动
	unregister_chrdev(major, "myled");
	return 0;
}

//for device tree
static const struct of_device_id led_table[] = 
{
	{
		.compatible = "myled",		//要与设备树中的.compatible一致
		.data = NULL,
	},
	{ }
};

struct platform_driver led_drv = 
{
	.probe 	= led_probe,	//匹配到dev之后调用probe
	.remove = led_remove,
	.driver = 
	{
		.name = "led_drv",
		.of_match_table = led_table,
	},
};
 
static int led_drv_init(void)
{
	platform_driver_register(&led_drv);
	return 0;
}
 
static void led_drv_exit(void)
{	
	platform_driver_unregister(&led_drv);
}
 
module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");

