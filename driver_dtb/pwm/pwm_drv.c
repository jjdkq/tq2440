/*************************************************************************
	> File Name: led.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年11月21日 星期三 15时15分56秒
 ************************************************************************/

#include "pwm.h"

static int pwm_major = 231;
static int pwm_minor = 0;
module_param(pwm_major, int, S_IRUGO);
static dev_t devno;

static struct class *pwm_cls;

static volatile pwm_t pwm;

struct clk *pwm_clk;
static int pwm_open(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "%s enter\n", __func__);
	unsigned long temp = readl(&pwm.gpbRegs->GPBCON);
	temp &= ~(3 << (pwm.pin * 2));
	temp |= 2 << (pwm.pin * 2);
	writel(temp, &pwm.gpbRegs->GPBCON);					//gpb1 tout1

	temp = readl(&pwm.pwmRegs->TCFG0);
	temp &= ~0xff;
	temp |= 15;
	writel(temp, &pwm.pwmRegs->TCFG0);					//pres = 15

	temp = readl(&pwm.pwmRegs->TCFG1);
	temp &= ~0xf;
	temp |= 2;
	writel(temp, &pwm.pwmRegs->TCFG1);					//mux = 1 / 8

	unsigned long pclk = clk_get_rate(pwm_clk);
	unsigned long cnt_cmp = pclk / (15 / 8) / 800;
//	unsigned long cnt_cmp = (50000000 >> 7) / 800;
	writel(cnt_cmp, &pwm.pwmRegs->TCNTB0);
	writel(cnt_cmp >> 1, &pwm.pwmRegs->TCMPB0);

	temp = readl(&pwm.pwmRegs->TCON);
	temp &= ~0x1f;
	temp |= 0xb;
	writel(temp, &pwm.pwmRegs->TCON);
	
	temp = readl(&pwm.pwmRegs->TCON);
	temp &= ~0x2;
	writel(temp, &pwm.pwmRegs->TCON);

	return 0;
}

static int pwm_close(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "%s enter\n", __func__);
	//关闭蜂鸣器
	u32 temp = readl(&pwm.gpbRegs->GPBCON);
	temp &= ~(3 << (pwm.pin * 2));
	temp |= 1 << (pwm.pin * 2);
	writel(temp, &pwm.gpbRegs->GPBCON);

	temp = readl(&pwm.gpbRegs->GPBDAT);
	temp &= ~(1 << pwm.pin);
	writel(temp, &pwm.gpbRegs->GPBDAT);
	return 0;
}

static struct file_operations pwm_fops=
{
	.owner = THIS_MODULE,
	.open = pwm_open,
	.release = pwm_close,
};

/*
static struct miscdevice pwm_misc = 
{   
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &pwm_fops,
};
*/

static int pwm_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	printk(KERN_INFO "%s enter\n", __func__);
	if (!node)
	{
		dev_dbg(dev, "error!of_node is null\n");
		return -EINVAL;
	}
	struct resource *res;
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pwm_physical");
	if (res)
	{
		pwm.pwmRegs = ioremap(res->start, res->end - res->end + 1);
	}
	else
	{
		printk(KERN_ERR "Can't get the pwm_physical resource\n");
		return -EINVAL;
	}


	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "gpb_physical");
	if (res)
	{
		pwm.gpbRegs = devm_ioremap_resource(dev, res);
	}
	else
	{
		printk(KERN_ERR "Can't get the gpb_physical resource\n");
		return -EINVAL;
	}
	
	of_property_read_u16(node, "pin", &pwm.pin);
	printk(KERN_INFO "pin = %d\n", pwm.pin);

	pwm_clk = devm_clk_get(dev, "pwm");
	if (IS_ERR(pwm_clk))
	{
		dev_err(dev, "Can't get pwm clock\n");
		return -ENOENT;
	}
	clk_prepare(pwm_clk);
	clk_enable(pwm_clk);

	int err = 0;
	if (pwm_major <= 0)
	{
		err = alloc_chrdev_region(&devno, pwm_minor, 1, DEVICE_NAME);	//从系统获取主设备号
		if (err < 0)
		{
			printk(KERN_ERR "Can't get the major device number from system!\n");
			return -EINVAL;
		}
        pwm_major = MAJOR(devno);    		
	}
	devno = MKDEV(pwm_major, pwm_minor);
	err = register_chrdev(pwm_major,DEVICE_NAME,&pwm_fops);	
	if (err < 0)
	{
		printk(DEVICE_NAME" can't register!\n");
		return -EINVAL; 
	}
	printk(DEVICE_NAME" inittialized\n");
	pwm_cls = class_create(THIS_MODULE, "mypwm");
	if (pwm_cls)
		device_create(pwm_cls, NULL, devno, NULL, "pwm");			// 创建设备节点
	else
		printk(KERN_ERR "error!Can't create pwm class\n");

//	int err;
//	err = misc_register(&pwm_misc);	
	return err;
}
static int pwm_remove(struct platform_device *pdev)
{
	printk(KERN_INFO "%s enter\n", __func__);

	device_destroy(pwm_cls, devno);
	class_destroy(pwm_cls);
	unregister_chrdev(pwm_major, DEVICE_NAME);	


//	misc_deregister(&pwm_misc);	

	iounmap(pwm.pwmRegs);
	devm_iounmap(&pdev->dev, pwm.gpbRegs);
	printk ("my "DEVICE_NAME" is over!\n");
}

static const struct of_device_id pwm_compat[] =
{
	{ .compatible = "pwm", .data = NULL },
	{ }
};

static struct platform_driver pwm_driver =
{
	.probe = pwm_probe,
	.remove = pwm_remove,
	.driver = 
	{
		.name = "pwm_drv",
		.of_match_table = pwm_compat,
	},
};

static int __init pwm_init(void)
{
	return platform_driver_register(&pwm_driver);
}

static void __exit pwm_exit(void)
{
	platform_driver_unregister(&pwm_driver);	
}

module_init(pwm_init);
module_exit(pwm_exit);
MODULE_LICENSE("GPL");
