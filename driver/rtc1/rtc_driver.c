#include <linux/module.h>
#include <linux/init.h>
 
MODULE_LICENSE("GPL");	//使用GPL版权协议
/*********驱动的装载卸载函数*********/
static int __init load_driver(void);
static void __exit unload_driver(void);
module_init(load_driver);
module_exit(unload_driver);
/*********驱动的装载卸载函数*********/
 
/*********rtc的寄存器地址*********/
#define pRTCBASE	0x57000040	//rtc寄存器物理基地址
#if 1
#define oRTCCON		0x00	//rtc控制寄存器
#define oTICNT		0x04	//rtc滴答中断控制寄存器
#define oRTCALM		0x10	//rtc报警使能寄存器
/*rtc报警时间寄存器*/
#define oALMSEC		0x14
#define oALMMIN		0x18
#define oALMHOUR	0x1C
#define oALMDATE	0x20
#define oALMMON		0x24
#define oALMYEAR	0x28
/*rtc时间设定寄存器*/
#define oBCDSEC		0x30
#define oBCDMIN		0x34
#define oBCDHOUR	0x38
#define oBCDDATE	0x3C
#define oBCDMON		0x44
#define oBCDYEAR	0x48
#endif
#if 0
typedef enum
{
	oRTCCON=0,
	oTICNT=2,
	oRTCALM,
	oALMSEC,
	oALMMIN,
	oALMHOUR,
	oALMDATE,
	oALMMON,
	oALMYEAR,
	oBCDSEC,
	oBCDMIN,
	oBCDHOUR,
	oBCDDATE,
	oBCDMON,
	oBCDYEAR
	
}rtc_t;
#endif
static void __iomem * vRTCBASE;	//rtc寄存器虚拟基地址
/*********rtc的寄存器地址*********/
 
/*********蜂鸣器寄存器地址*********/
#define pGPBBASE	0x56000010	//物理基地址
#define oGPBCON		0x00
#define oGPBDAT		0x04
#define oGPBUP		0x08
static void __iomem * vGPBBASE;	//虚拟基地址
/*********蜂鸣器寄存器地址*********/
 
/*********作为平台设备的rtc*********/
#include <linux/platform_device.h>
#include <asm/irq.h>
static void release_inplat_device(struct device * inplat_device){}	
	//析构平台设备内嵌的device结构体
static struct resource rtc_resource[] =	//rtc占用资源的数组
{
	[0] =	//rtc寄存器
	{
		.start = pRTCBASE+oRTCCON,
		.end = pRTCBASE+oBCDYEAR,
		.flags = IORESOURCE_MEM,
	},
	[1] =	//rtc滴答中断
	{
		.start = IRQ_TICK,
		.end = IRQ_TICK,
		.flags = IORESOURCE_IRQ,
	},
	[2] =	//rtc报警中断
	{
		.start = IRQ_RTC,
		.end = IRQ_RTC,
		.flags = IORESOURCE_IRQ,
	},
	[3] =	//蜂鸣器寄存器
	{
		.start = pGPBBASE+oGPBCON,
		.end = pGPBBASE+oGPBUP,
		.flags = IORESOURCE_MEM,
	},
};
static struct platform_device platform_rtc =
{
	.name = "platform_rtc",	//平台设备和平台驱动的name要一致
	.id = -1,	//一般都设为-1
	.dev =	//内嵌的device结构体，必须写其release函数
	{
		.release = release_inplat_device,
	},
	.resource = rtc_resource,	//rtc占用资源的数组
	.num_resources = ARRAY_SIZE(rtc_resource),	//资源数组的长度
};
/*********作为平台设备的rtc*********/
 
/*********平台rtc的驱动*********/
static int rtc_probe(struct platform_device *);
static int rtc_remove(struct platform_device *);
static struct platform_driver platform_rtc_driver =
{
	.probe = rtc_probe,
//	.remove = rtc_remove,
	.driver = 
	{
		.name = "platform_rtc",	//平台设备和平台驱动的name要一致
		.owner = THIS_MODULE,
	},
};
/*********平台rtc的驱动*********/
 
/*********作为混杂设备的rtc*********/
#include <linux/fs.h>
#include <linux/miscdevice.h>
int rtc_open(struct inode *, struct file *);
int rtc_release(struct inode *, struct file *);
static int rtc_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
static struct file_operations rtc_operation =
{
	.owner = THIS_MODULE,
	.open = rtc_open,
	.release = rtc_release,
	.ioctl = rtc_ioctl,
};
static struct miscdevice misc_rtc =
{
	.minor = 255,	//混杂设备主设备号就是10，255表示动态分配次设备号
	.name = "misc_rtc",
	.fops = &rtc_operation,
};
/*********作为混杂设备的rtc*********/
 
/*——————————————————以下是函数的具体实现——————————————————*/
 
/*********驱动的装载函数*********/
static int __init load_driver(void)
{
	int ret;
	/*注册平台rtc*/
	ret = platform_device_register(&platform_rtc);
	if(ret)
	{
		printk(KERN_ALERT "平台rtc注册失败");
		return ret; 
	}
	/*注册平台rtc驱动*/
	ret = platform_driver_register(&platform_rtc_driver);
	if(ret)
	{
		printk(KERN_ALERT "平台rtc驱动注册失败");
		return ret;
	}
	/*注册混杂rtc*/	
	ret = misc_register(&misc_rtc);
	if(ret)
	{
		printk(KERN_ALERT "混杂rtc注册失败");
		return ret;
	}
	printk(KERN_ALERT "rtc注册成功");
	return 0;	
}
/*********驱动的装载函数*********/
 
/*********驱动的卸载函数*********/
static void __exit unload_driver(void)
{
	platform_driver_unregister(&platform_rtc_driver);
	platform_device_unregister(&platform_rtc);
	misc_deregister(&misc_rtc);
	printk(KERN_ALERT "rtc卸载完成");
}
/*********驱动的卸载函数*********/
 
/*********rtc的probe函数*********/
#include <linux/io.h>
#include <linux/interrupt.h>
static irqreturn_t rtc_tick_handle(int, void *);	//rtc滴答中断的处理函数
static int rtc_probe(struct platform_device * rtc)
{
	int ret;
	struct resource *rtc_mem, *rtc_tick_irq, *beep_mem;
	/*rtc寄存器地址映射*/
	rtc_mem = platform_get_resource(rtc, IORESOURCE_MEM, 0);
	if(rtc_mem == NULL)
	{
		printk(KERN_ALERT"platform_get_resource失败");
		return -ENOENT;
	}

	if(request_mem_region(rtc_mem->start, rtc_mem->end-rtc_mem->start+1, rtc->name) == NULL)
	{
		printk(KERN_ALERT"request_mem_region失败");
		return -ENOENT;
	}

	vRTCBASE = ioremap(rtc_mem->start, rtc_mem->end-rtc_mem->start+1);
	if(vRTCBASE == NULL)
	{
		printk(KERN_ALERT"寄存器地址映射失败");
		return -EINVAL;
	}
	/*申请滴答中断线*/
	rtc_tick_irq = platform_get_resource(rtc, IORESOURCE_IRQ, 0);
	if(rtc_tick_irq == NULL)
	{
		printk(KERN_ALERT"platform_get_resource失败");
		return -ENOENT;
	}
	ret = request_irq(rtc_tick_irq->start, rtc_tick_handle, 0, rtc->name, rtc);
	if(ret != 0)
	{
		printk(KERN_ALERT"申请滴答中断线失败");
		return ret; 
	}
	/*初始化rtc，一定要先使能RTCCON的第0位才能读写寄存器*/
	writeb(0x01, vRTCBASE+oRTCCON);	//合并BCD码，XTAL分配时钟，使能读写
	writeb(0x00, vRTCBASE+oTICNT);	//禁止滴答中断
	writeb(0x00, vRTCBASE+oRTCALM);	//禁止报警
	writeb(0x00, vRTCBASE+oALMSEC);	//报警时间寄存器清零
	writeb(0x00, vRTCBASE+oALMMIN);
	writeb(0x00, vRTCBASE+oALMHOUR);
	writeb(0x00, vRTCBASE+oALMDATE);
	writeb(0x00, vRTCBASE+oALMMON);
	writeb(0x00, vRTCBASE+oALMYEAR);
	writeb(0x00, vRTCBASE+oBCDSEC);	//当前时间寄存器清零
	writeb(0x00, vRTCBASE+oBCDMIN);
	writeb(0x00, vRTCBASE+oBCDHOUR);
	writeb(0x00, vRTCBASE+oBCDDATE);
	writeb(0x00, vRTCBASE+oBCDMON);
	writeb(0x00, vRTCBASE+oBCDYEAR);
	printk(KERN_ALERT "rtc初始化完成");
	/*蜂鸣器寄存器地址映射*/
	beep_mem = platform_get_resource(rtc, IORESOURCE_MEM, 1);
	if(beep_mem == NULL)
	{
		printk(KERN_ALERT"platform_get_resource失败");
		return -ENOENT;
	}

	if(request_mem_region(beep_mem->start, beep_mem->end-beep_mem->start+1, rtc->name) == NULL)
	{
		printk(KERN_ALERT"request_mem_region失败");
		return -ENOENT;
	}

	vGPBBASE = ioremap(beep_mem->start, beep_mem->end-beep_mem->start+1);
	if(vGPBBASE == NULL)
	{
		printk(KERN_ALERT"寄存器地址映射失败");
		return -EINVAL;
	}
	/*初始化蜂鸣器*/
	writel(readl(vGPBBASE+oGPBCON) & ~2, vGPBBASE+oGPBCON);
	writel(readl(vGPBBASE+oGPBCON) | 1, vGPBBASE+oGPBCON);	//GPB0输出
	writel(readl(vGPBBASE+oGPBDAT) & ~1, vGPBBASE+oGPBDAT);	//关闭蜂鸣器
	writel(readl(vGPBBASE+oGPBUP) & ~1, vGPBBASE+oGPBUP);	//使能上拉电阻
	return 0;
}
/*********rtc的probe函数*********/
 
#if 0
/*********rtc的remove函数*********/
static int rtc_remove(struct platform_device * rtc)
{
	struct resource *beep_mem, *rtc_tick_irq, *rtc_mem;
	/*释放蜂鸣器io内存*/
	iounmap(vGPBBASE);
	beep_mem = platform_get_resource(rtc, IORESOURCE_MEM, 1);
	if(beep_mem == NULL)
	{
		printk(KERN_ALERT"platform_get_resource失败");
		return -ENOENT;
	}
	release_mem_region(beep_mem->start, beep_mem->end-beep_mem->start+1);
    writeb(0x00, vRTCBASE+oTICNT);  //禁止滴答中断
    writeb(0x00, vRTCBASE+oRTCCON);	//禁止读写rtc寄存器
	/*释放滴答中断线*/
	rtc_tick_irq = platform_get_resource(rtc, IORESOURCE_IRQ, 0);
	if(rtc_tick_irq == NULL)
	{
		printk(KERN_ALERT"platform_get_resource失败");
		return -ENOENT;
	}
	free_irq(rtc_tick_irq->start, rtc);
	/*释放rtc的io内存*/
	iounmap(vRTCBASE);
	rtc_mem = platform_get_resource(rtc, IORESOURCE_MEM, 0);
	if(rtc_mem == NULL)
	{
		printk(KERN_ALERT"platform_get_resource失败");
		return -ENOENT;
	}
	release_mem_region(rtc_mem->start, rtc_mem->end-rtc_mem->start+1);
	printk(KERN_ALERT"资源释放完成");
	return 0;
}
#endif
/*********rtc的remove函数*********/
 
/*********rtc的open函数*********/
int rtc_open(struct inode * inode, struct file * file)
{
	printk(KERN_ALERT "打开rtc成功");
	return 0;
}
/*********rtc的open函数*********/
 
/*********rtc的release函数*********/
int rtc_release(struct inode * inode, struct file * file)
{
	printk(KERN_ALERT "关闭rtc完成");
	return 0;
}
/*********rtc的release函数*********/
 
/*********rtc的ioctl函数*********/
#include <linux/rtc.h>
unsigned bcd2bin(unsigned char val)	//一字节合并bcd码转化为无符号数
{
	return (val&0x0f) + (val>>4)*10;
}
unsigned char bin2bcd(unsigned val)	//一字节无符号数转化为合并bcd码
{
	return ((val/10) << 4) + val%10;
}

/* 加载模块之后,使用命令hwclock -f /dev/misc_rtc -r之后会显示rtc时间(硬件时钟时间),使用date -s "年-月-日 时:分:秒"修改系统时钟时间之后,再使用hwclock -f /dev/misc_rtc -w即可将rtc时间修改为和系统时钟时间一样
 * hwclock -f /dev/misc_rtc命令会寻找misc_rtc设备,然后调用其ioctl函数
 */

static int rtc_ioctl(struct inode * inode, struct file * file, unsigned int command, unsigned long arg)
{
	void __user * user_arg = (void __user *)arg;
	struct rtc_time time;
	switch(command)
	{
		case RTC_RD_TIME:	//读时间时考虑到1s误差，应该最后读秒，且判断秒是否为0
		time.tm_min = bcd2bin(readb(vRTCBASE+oBCDMIN));
		time.tm_hour = bcd2bin(readb(vRTCBASE+oBCDHOUR));
		time.tm_mday = bcd2bin(readb(vRTCBASE+oBCDDATE));
		time.tm_mon = bcd2bin(readb(vRTCBASE+oBCDMON));
		time.tm_year = bcd2bin(readb(vRTCBASE+oBCDYEAR));
		time.tm_sec = bcd2bin(readb(vRTCBASE+oBCDSEC));
		if(time.tm_sec == 0)	//如果秒为0，应该重读一次
		{
			time.tm_min = bcd2bin(readb(vRTCBASE+oBCDMIN));
			time.tm_hour = bcd2bin(readb(vRTCBASE+oBCDHOUR));
			time.tm_mday = bcd2bin(readb(vRTCBASE+oBCDDATE));
			time.tm_mon = bcd2bin(readb(vRTCBASE+oBCDMON));
			time.tm_year = bcd2bin(readb(vRTCBASE+oBCDYEAR));
			time.tm_sec = bcd2bin(readb(vRTCBASE+oBCDSEC));			
		}
		if (copy_to_user(user_arg, &time, sizeof(time)))
		{
			printk(KERN_ALERT "copy_to_user失败");
			return -EFAULT;
		}
		printk(KERN_ALERT "读取时间成功");
		return 0;
	case RTC_SET_TIME:
		if(copy_from_user(&time, user_arg, sizeof(time)))
		{
			printk(KERN_ALERT "copy_from_user失败");
			return -EFAULT;
		}
		writeb(bin2bcd(time.tm_sec), vRTCBASE+oBCDSEC);
		writeb(bin2bcd(time.tm_min), vRTCBASE+oBCDMIN);
		writeb(bin2bcd(time.tm_hour), vRTCBASE+oBCDHOUR);
		writeb(bin2bcd(time.tm_mday), vRTCBASE+oBCDDATE);
		writeb(bin2bcd(time.tm_mon), vRTCBASE+oBCDMON);
		writeb(bin2bcd(time.tm_year), vRTCBASE+oBCDYEAR);
		printk(KERN_ALERT "设置时间成功");
		return 0;
	case 999999:	//使能滴答中断
		writeb(0xFF, vRTCBASE+oTICNT);
		printk(KERN_ALERT "滴答中断开启");
		return 0;
	case 1000000:	//禁止滴答中断
		writeb(0x00, vRTCBASE+oTICNT);
		printk(KERN_ALERT "滴答中断关闭");
		return 0;
	default:
		printk(KERN_ALERT "未定义控制命令");
		return -ENOTTY;	//未定义命令
	}
}
/*********rtc的ioctl函数*********/
 
/*********rtc滴答中断的处理函数*********/
static irqreturn_t rtc_tick_handle(int irq, void * dev_id)
{
	volatile int i, j, k;
	//蜂鸣器响一下
	writel(readl(vGPBBASE+oGPBDAT) | 1, vGPBBASE+oGPBDAT);
	for(i = 0; i < 256; i++)
		for(j = 0; j < 256; j++)
			for(k = 0; k < 16; k++);
	writel(readl(vGPBBASE+oGPBDAT) & ~1, vGPBBASE+oGPBDAT);
	printk(KERN_ALERT "滴答中断");
	return 0;
}
/*********rtc滴答中断的处理函数*********/

