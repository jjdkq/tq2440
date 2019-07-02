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
static struct resource rtc_resource[] = 
{	
	[0] =
	{
		.start = 0x57000040,		//the address of RTCCON
		.end   = 0x57000040 +0x48-1,
		.flags = IORESOURCE_MEM,	//此时.start和.end表示内存地址
	},
	/*
	[1] = 
	{
		.start = 8,					//GPB8
		.end   = 8,
		.flags = IORESOURCE_IRQ,	//此时.start和.end表示中断号
	},
	*/
};
 
static void rtc_release(struct device *dev)
{
 
}

// 创建一个设备
static struct platform_device rtc_dev =
{
	.name = "myrtc",	//设备名字与驱动相匹配
	.id	  = -1,
	.num_resources = ARRAY_SIZE(rtc_resource),
	.resource = rtc_resource,
	
	.dev =
	{
		.release = rtc_release,
		//.devt = MKDEV(252, 1),
	},
};
 
static int rtc_dev_init(void)
{
	//向bus注册rtc_dev match drv链表进行配对
	platform_device_register(&rtc_dev);
	return 0;
}
 
static void rtc_dev_exit(void)
{
	platform_device_unregister(&rtc_dev);
}
 
module_init(rtc_dev_init);
module_exit(rtc_dev_exit);
MODULE_LICENSE("GPL");
	

