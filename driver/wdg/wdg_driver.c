/*************************************************************************
	> File Name: wdg_driver.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2019年01月01日 星期二 15时51分54秒
 ************************************************************************/


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
#include <linux/io.h>
#include <linux/miscdevice.h>


#define pWTBASE		0x53000000				//看门狗寄存器物理地址基地址
#define oWTCON		0x00					//看门狗控制寄存器地址偏移
#define oWTDAT		0x04					//看门狗数据寄存器地址偏移
#define oWTCNT		0x08					//看门狗计数寄存器地址偏移

//看门狗相关寄存器的虚拟地址
static unsigned short *wtbase;
static unsigned short *wtcon;
static unsigned short *wtdat;
static unsigned short *wtcnt;

static struct resource wdg_resource[] =
{
	[0] =
	{
		.start = pWTBASE,
		.end   = pWTBASE + oWTCNT,
		.flags = IORESOURCE_MEM,
	},
	[1] = 
	{
		.start = IRQ_WDT,					//看门狗中断号
		.end   = IRQ_WDT,
		.flags = IORESOURCE_IRQ,
	},
};

static void wdg_release(struct device *dev)
{

}

//定义并初始化等待队列
//DECLARE_WAIT_QUEUE_HEAD(wdg_waitq);
unsigned irq_flag;							//中断标志位

//看门狗中断
static irqreturn_t wdg_interrupt(int irq, void *dev_id)
{
//	irq_flag = 1;							//发生了中断
	printk(KERN_INFO "Ready to feed the dog!\n");
	writew(20000, wtcnt);					//喂狗
//	wake_up_interruptible(&wdg_waitq);		//唤醒休眠的进程
	return IRQ_RETVAL(IRQ_HANDLED);
}

static struct platform_device wdg_pdev = 
{
	.name = "mywdg",
	.id   = -1,
	.num_resources = ARRAY_SIZE(wdg_resource),
	.resource = wdg_resource,
	.dev  =
	{
		.release = wdg_release,
	},
};


static int wdg_open(struct inode *inode, struct file *file)
{
	int err = 0;
	writew(20000, wtdat);
	writew(20000, wtcnt);
	writew(77 << 8 | 1 << 5 | 3 << 3 | 1 << 2 | 1 << 0, wtcon);
	//注册看门狗中断
	err = request_irq(IRQ_WDT, wdg_interrupt, 0, "mywdg", (void *)&irq_flag);
	if (err)
	{
		printk(KERN_ERR "request wdg_irq error!\n");
		free_irq(IRQ_WDT, &irq_flag);
		return -EBUSY;
	}
	return 0;
}

static struct file_operations wdg_fops = 
{
	.owner = THIS_MODULE,
	.open  = wdg_open,

};

static struct miscdevice wdg_misc =
{
	.minor = MISC_DYNAMIC_MINOR,
	.name  = "mywdg",
	.fops  = &wdg_fops,
};

static int wdg_probe(struct platform_device *pdev)
{
	int err = 0;
	struct resource *res;
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);		//获得看门狗寄存器地址
	wtbase = ioremap(res->start, res->end - res->start + 1);
	wtcon  = wtbase + oWTCON;
	wtdat  = wtbase + oWTDAT;
	wtcnt  = wtbase + oWTCNT;
	printk(KERN_INFO "wdg_probe, found wdg\n");
	misc_register(&wdg_misc);
/*	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);		//获得看门狗中断号
	//注册看门狗中断
	err = request_irq(IRQ_WDT, wdg_interrupt, 0, pdev->name, pdev);
	if (err)
	{
		printk(KERN_ERR "request wdg_irq error!\n");
		free_irq(res->start, pdev);
		return -EBUSY;
	}
	*/
	return 0;
}

static int wdg_remove(struct platform_device *pdev)
{
	misc_deregister(&wdg_misc);
	iounmap(wtbase);
	free_irq(IRQ_WDT, &irq_flag);
	return 0;
}

static struct platform_driver wdg_pdrv =
{
	.probe = wdg_probe,
	.remove = wdg_remove,
	.driver =
	{
		.name = "mywdg",
	},
};

static int __init wdg_init(void)
{
	platform_device_register(&wdg_pdev);
	platform_driver_register(&wdg_pdrv);
	return 0;
}

static void __exit wdg_exit(void)
{
	platform_device_unregister(&wdg_pdev);
	platform_driver_unregister(&wdg_pdrv);
}

module_init(wdg_init);
module_exit(wdg_exit);
MODULE_LICENSE("GPL");

