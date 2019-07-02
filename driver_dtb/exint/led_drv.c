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


static u32 pin[4];

static int major;
 
static struct class *cls;
static struct device *dev;
 
static volatile unsigned long *gpio_con;
static volatile unsigned long *gpio_dat;
static int gpb8_pin, gpb7_pin;
 
static int led_open(struct inode *inode, struct file *file)
{
	int i = 0;
	u32 temp = readl(gpio_con);
	for (; i < 4; i++)
	{
		temp &= ~(3 << (pin[i] * 2));
		temp |= 1 << (pin[i] * 2);
	}
	writel(temp, gpio_con);

	/*
//	*gpio_con &= ~(0x03 << (gpb8_pin*2));
	*gpio_con &= ~(0x03 << (gpb7_pin*2));
//	*gpio_con |=  (0x01 << (gpb8_pin*2));
	*gpio_con |=  (0x01 << (gpb7_pin*2));
	*/
	return 0;
}
 
static ssize_t led_write(struct file *file, const char __user *buf,
	size_t count, loff_t *ppos)
{
	int val;
	u32 temp;
	copy_from_user(&val, buf, sizeof(val));
	int i;
	temp = readl(gpio_dat);
	for (i = 0; i < 4; i++)
	{
		if (val == 1)
			temp &= ~(1 << pin[i]);
		else
			temp |= 1 << pin[i];
	}
	writel(temp, gpio_dat);

/*
	if(val == 1)
	{
	//	*gpio_dat &= ~(1<<gpb8_pin);
		*gpio_dat &= ~(1<<gpb7_pin);
	}
	else
	{
	//	*gpio_dat |= (1<<gpb8_pin);
		*gpio_dat |= (1<<gpb7_pin);
	}
*/
	return 0;
}

static long led_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	u32 temp = readl(gpio_dat);
	switch (cmd)
	{
		case 0:
			temp &= ~(1 << pin[arg]);
			break;
		case 1:
			temp |= 1 << pin[arg];
			break;
	}
	writel(temp, gpio_dat);
	return 0;
}
 
static struct file_operations led_fops = 
{
	.owner = THIS_MODULE,
	.open  = led_open,
	.write = led_write,
	.unlocked_ioctl = led_ioctl,
};
 
static int led_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct device_node *node = pdev->dev.of_node;
	printk("led_probe, found led\n");
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res)
	{
		gpio_con = ioremap(res->start, res->end - res->start + 1);
		gpio_dat = gpio_con + 1;
	}
	else
		printk(KERN_ERR "Can't get the reg resource!\n");
	//内核解析dtb文件获得pin属性
	/*
	of_property_read_s32(pdev->dev.of_node, "gpb5", &gpb7_pin);
	pin[0] = gpb7_pin;
	of_property_read_s32(pdev->dev.of_node, "gpb6", &gpb7_pin);
	pin[1] = gpb7_pin;
	of_property_read_s32(pdev->dev.of_node, "gpb7", &gpb7_pin);
	pin[2] = gpb7_pin;
	of_property_read_s32(pdev->dev.of_node, "gpb8", &gpb7_pin);
	pin[3] = gpb7_pin;
	*/

//	printk(KERN_INFO "node_name = %s", node->name);
	int i = 0;
	of_property_read_u32_array(node, "pin", pin, 4);
	for (; i < 4; i++)
		printk(KERN_INFO "pin[%d] = %d\n", i, pin[i]);
 
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
	// 取消内存映射
	iounmap(gpio_con);
 
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

