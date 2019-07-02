#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/sched.h> 
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <linux/input.h>
#include <linux/platform_device.h>
// 设备资源
static struct resource led_resource[] = 
{	
	[0] =
	{
		.start = 0x56000010,		//the address of GPBDAT
		.end   = 0x56000010 + 4,
		.flags = IORESOURCE_MEM,	//此时.start和.end表示内存地址
	},
	[1] = 
	{
		.start = 8,					//GPB8
		.end   = 8,
		.flags = IORESOURCE_IRQ,	//此时.start和.end表示中断号
	},
};
 
static void led_release(struct device *dev)
{
 
}

// 创建一个设备
static struct platform_device led_dev =
{
	.name = "myled",	//设备名字与驱动相匹配
	.id	  = -1,
	.num_resources = ARRAY_SIZE(led_resource),
	.resource = led_resource,
	
	.dev =
	{
		.release = led_release,
		//.devt = MKDEV(252, 1),
	},
};
 
static int led_dev_init(void)
{
	//向bus注册led_dev match drv链表进行配对
	platform_device_register(&led_dev);
	return 0;
}
 
static void led_dev_exit(void)
{
	platform_device_unregister(&led_dev);
}
 
module_init(led_dev_init);
module_exit(led_dev_exit);
MODULE_LICENSE("GPL");
	
