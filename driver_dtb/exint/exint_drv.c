
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
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#define DEVICE "key"

static int major;
static struct class *key_class;
static struct device *dev;

static DECLARE_WAIT_QUEUE_HEAD(key_waitq);      //定义并初始化等待队列
static int ev_press;                            //中断事件标志

static struct device *exint_dev;
static int irq[4];
static int irq_id;

struct mykey
{
	volatile u32 *gpfcon;
	volatile u32 *gpfdat;
	u32 pin[4];
};

static struct mykey mykey;


/* 按键驱动部分 */
static int key_open(struct inode *inode, struct file *file)
{
	int i;
	u32 temp = readl(mykey.gpfcon);
	for (i = 0; i < ARRAY_SIZE(mykey.pin); i++)
	{
		temp &= ~(3 << (mykey.pin[i] * 2));		
		temp |= 2 << (mykey.pin[i] * 2);		//gpf0,1,2,4中断
	}
	writel(temp, mykey.gpfcon);
	return 0;
}
	
static ssize_t key_read(struct file *file, const char __user *buf,
						size_t count, loff_t *ppos)
{
	printk(KERN_INFO "%s enter\n", __func__);
	wait_event_interruptible(key_waitq,ev_press);//如果ev_press=0,将会休眠线程      
	ev_press=0;                                 //没有发生中断
	int err = copy_to_user(buf, &irq_id, sizeof(irq_id));
	return err? -EFAULT : 0;
}

static char *dev_id[4] = { "key_1", "key_2", "key_3", "key_4", };
static char **irq_name = dev_id; 

static int key_release(struct inode *inode, struct file *file)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(dev_id); i++)
//		devm_free_irq(exint_dev, irq[i], dev_id[i]);	//与devm_request_any_context_irq()对应
//或者
		free_irq(irq[i], dev_id[i]);					//与request_irq()对应
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


/* 外部中断驱动部分 */
static irqreturn_t tq2440_key_isr(int irq, void *dev_id)
{
    printk("%s enter, irq: %d, %s\n", __func__, irq, (char *)dev_id);
	ev_press = 1;
	irq_id = irq;
	wake_up_interruptible(&key_waitq);              //唤醒休眠的线程 
    return IRQ_HANDLED;
}

static int exint_probe(struct platform_device *pdev) 
{
    exint_dev = &pdev->dev;
    int irq_gpio = -1;

    printk("%s enter.\n", __func__);

    if (!exint_dev->of_node) {
        dev_err(exint_dev, "no platform data.\n");
        return -EINVAL;
    }
	int i;
#if 1
	//方法1:要和设备树中的方法1对应
	for (i = 0; i < ARRAY_SIZE(dev_id); i++)
	{
		irq_gpio = of_get_named_gpio(exint_dev->of_node, irq_name[i], 0);
		irq[i] = gpio_to_irq(irq_gpio);
		printk("%s: gpio: %d ---> irq (%d)\n", __func__, irq_gpio, irq[i]);
	/*
		//要和devm_free_irq()对应
		devm_request_any_context_irq(exint_dev, irq[i],
		    tq2440_key_isr, IRQF_TRIGGER_FALLING, irq_name[i], dev_id[i]);
	*/
		//或者
		//要和free_irq()对应
		request_irq(irq[i], tq2440_key_isr, IRQF_TRIGGER_FALLING, irq_name[i], dev_id[i]);
	}
#endif
#if 0
	//或者
	//方法2:要和设备树中的方法2对应
	for (i = 0; i < ARRAY_SIZE(dev_id); i++)
	{
		irq[i] = platform_get_irq(pdev, i);
		printk("%s: get irq %d\n", __func__, irq[i]);
	/*
		devm_request_any_context_irq(exint_dev, irq[i],
		    tq2440_key_isr, IRQF_TRIGGER_FALLING, irq_name[i], dev_id[i]);
	*/
		request_irq(irq[i], tq2440_key_isr, IRQF_TRIGGER_FALLING, irq_name[i], dev_id[i]);
	}
#endif

	key_probe(pdev);
    return 0;
}

static int exint_remove(struct platform_device *pdev)
{

    printk("%s enter.\n", __func__);

	key_remove(pdev);
    return 0;
}

static const struct of_device_id tq2440_key_dt_ids[] =
{
    { .compatible = "exint", },
    { },
};

MODULE_DEVICE_TABLE(of, tq2440_key_dt_ids);

static struct platform_driver exint_driver = 
{
    .driver = 
	{
        .name = "exint_drv",
        .of_match_table = of_match_ptr(tq2440_key_dt_ids),
    },
    .probe  = exint_probe,
    .remove = exint_remove,
};

static int __init exint_init(void)
{
    int ret;

    ret = platform_driver_register(&exint_driver);
    if (ret)
        printk(KERN_ERR "exint: probe failed: %d\n", ret);

    return ret;
}
module_init(exint_init);

static void __exit exint_exit(void)
{
    platform_driver_unregister(&exint_driver);
}
module_exit(exint_exit);

MODULE_LICENSE("GPL");
