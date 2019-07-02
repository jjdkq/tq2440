/*************************************************************************
	> File Name: led.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年11月21日 星期三 15时15分56秒
 ************************************************************************/

#include "rtc_drv.h"
#include "rtc.h"

static int rtc_major = 231;
static int rtc_minor = 0;
module_param(rtc_major, int, S_IRUGO);
static dev_t devno;

static struct class *rtc_cls;

static volatile struct rtc rtc;

static int rtc_open(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "%s enter\n", __func__);
	/*
	writeb(1, rtc.rtccon);				//使能rtc控制	
	writeb(0x20, rtc.bcdyear);			//设置年
	writeb(0x12, rtc.bcdmon);			//月
	writeb(0x30, rtc.bcddate);			//日
	writeb(0x00, rtc.bcdday);			//星期
	writeb(0x22, rtc.bcdhour);			//小时	
	writeb(0x13, rtc.bcdmin);			//分
	writeb(0x10, rtc.bcdhour);			//秒
	writeb(readb(rtc.rtccon) &~ 1, rtc.rtccon);//禁止rtc控制
	*/
	return 0;
}

static int rtc_ioctl(struct file *file, unsigned cmd, unsigned long arg)
{
	static time_t_ time;
	void __user *user_arg = (void __user *)arg;
	writeb(1, rtc.rtccon);									//使能rtc控制
	switch (cmd)
	{
		case RDTIME:
			time.year = readb(rtc.bcdyear) + 0x2000;
			time.mon = readb(rtc.bcdmon);
			time.date = readb(rtc.bcddate);
			time.day = readb(rtc.bcdday);
			time.hour = readb(rtc.bcdhour);
			time.min = readb(rtc.bcdmin);
			time.sec = readb(rtc.bcdsec);
			copy_to_user(user_arg, &time, sizeof(time));
			break;
		case WRTIME:
			copy_from_user(&time, user_arg, sizeof(time));
			writeb(const_bin2bcd(time.year), rtc.bcdyear);
			writeb(const_bin2bcd(time.mon), rtc.bcdmon);
			writeb(const_bin2bcd(time.date), rtc.bcddate);
			writeb(const_bin2bcd(time.day), rtc.bcdday);
			writeb(const_bin2bcd(time.hour), rtc.bcdhour);
			writeb(const_bin2bcd(time.min), rtc.bcdmin);
			writeb(const_bin2bcd(time.sec), rtc.bcdsec);
			break;
	}
	writeb(readb(rtc.rtccon) &~ 1, rtc.rtccon);//禁止rtc控制
	return 0;
}

static struct file_operations rtc_fops=
{
	.owner = THIS_MODULE,
	.open = rtc_open,
	.unlocked_ioctl = rtc_ioctl
};

/*
static struct miscdevice rtc_misc = 
{   
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &rtc_fops,
};
*/

static int rtc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	struct clk *rtc_clk;
	printk(KERN_INFO "%s enter\n", __func__);
	if (!node)
	{
		dev_dbg(dev, "error!of_node is null\n");
		return -EINVAL;
	}
	struct resource *res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "rtc_physical");
	if (res)
	{
		rtc.rtcbase = devm_ioremap_resource(dev, res);
		rtc.rtccon = rtc.rtcbase + rtccon_offset;
		rtc.bcdyear = rtc.rtcbase + bcdyear_offset;
		rtc.bcdmon = rtc.rtcbase + bcdmon_offset;
		rtc.bcddate = rtc.rtcbase + bcddate_offset;
		rtc.bcdday = rtc.rtcbase + bcdday_offset;
		rtc.bcdhour = rtc.rtcbase + bcdhour_offset;
		rtc.bcdmin = rtc.rtcbase + bcdmin_offset;
		rtc.bcdsec = rtc.rtcbase + bcdsec_offset;

	}
	else
	{
		printk(KERN_ERR "Can't get the mem resource\n");
		return -EINVAL;
	}
	
	rtc_clk = devm_clk_get(dev, "rtc");
	if (IS_ERR(rtc_clk))
	{
		dev_err(dev, "Can't get rtc clock\n");
		return -ENOENT;
	}
	clk_prepare(rtc_clk);
	clk_enable(rtc_clk);

	int err = 0;
	if (rtc_major <= 0)
	{
		err = alloc_chrdev_region(&devno, rtc_minor, 1, DEVICE_NAME);	//从系统获取主设备号
		if (err < 0)
		{
			printk(KERN_ERR "Can't get the major device number from system!\n");
			return -EINVAL;
		}
        rtc_major = MAJOR(devno);    		
	}
	devno = MKDEV(rtc_major, rtc_minor);
	err = register_chrdev(rtc_major,DEVICE_NAME,&rtc_fops);	
	if (err < 0)
	{
		printk(DEVICE_NAME" can't register!\n");
		return -EINVAL; 
	}
	printk(DEVICE_NAME" inittialized\n");
	rtc_cls = class_create(THIS_MODULE, "myrtc");
	if (rtc_cls)
		device_create(rtc_cls, NULL, devno, NULL, "rtc");			// 创建设备节点
	else
		printk(KERN_ERR "error!Can't create rtc class\n");

//	int err;
//	err = misc_register(&rtc_misc);	
	return err;
}
static int rtc_remove(struct platform_device *pdev)
{
	printk(KERN_INFO "%s enter\n", __func__);

	device_destroy(rtc_cls, devno);
	class_destroy(rtc_cls);
	unregister_chrdev(rtc_major, DEVICE_NAME);	


//	misc_deregister(&rtc_misc);	

	devm_iounmap(&pdev->dev, rtc.rtcbase);
	printk ("my "DEVICE_NAME" is over!\n");
}

static const struct of_device_id rtc_compat[] =
{
	{ .compatible = "rtc", .data = NULL },
	{ }
};

static struct platform_driver rtc_driver =
{
	.probe = rtc_probe,
	.remove = rtc_remove,
	.driver = 
	{
		.name = "rtc_drv",
		.of_match_table = rtc_compat,
	},
};

static int __init rtc_init(void)
{
	return platform_driver_register(&rtc_driver);
}

static void __exit rtc_exit(void)
{
	platform_driver_unregister(&rtc_driver);	
}

module_init(rtc_init);
module_exit(rtc_exit);
MODULE_LICENSE("GPL");
