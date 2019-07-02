
/*************************************************************************
	> File Name: wind_speed_drv.c
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

#include <linux/timer.h>

#define DEVICE "wind_speed"

static int major;
static struct class *wind_speed_class;
static struct device *dev;

static DECLARE_WAIT_QUEUE_HEAD(wind_speed_waitq);      	//定义并初始化等待队列
static int ev_press;                            		//中断事件标志

static struct device *wind_speed_dev;
static int irq;
static int irq_id;

struct mywind_speed
{
	volatile u32 *gpfcon;
	u32 pin;
	
	atomic_t f;									//原子变量，用来记录脉冲频率
	struct timer_list s_timer;					//定时器
};

static struct mywind_speed mywind_speed; 

static int f;
static void second_timer_handle(struct timer_list *list)
{
    mod_timer(&mywind_speed.s_timer, jiffies + HZ);	//触发下一次定时,HZ表示1s触发一次该定时器中断
    
    f = atomic_read (&mywind_speed.f);
	atomic_set(&mywind_speed.f, 0);					//将原子变量(脉冲频率)设为0
//    printk (KERN_NOTICE "current jiffies is %lu\n", jiffies);
}

static int wind_speed_open(struct inode *inode, struct file *file)
{
	u32 temp = readl(mywind_speed.gpfcon);

	temp &= ~(3 << (mywind_speed.pin * 2));		
	temp |= 2 << (mywind_speed.pin * 2);		//gpf4中断

	writel(temp, mywind_speed.gpfcon);
	
	init_timer_key(&mywind_speed.s_timer, second_timer_handle, 0,
					NULL, NULL);
    mywind_speed.s_timer.function = second_timer_handle;
    mywind_speed.s_timer.expires = jiffies + HZ;
    add_timer(&mywind_speed.s_timer);
	
    atomic_set(&mywind_speed.f, 0);
    return 0;
}
	
static ssize_t wind_speed_read(struct file *file, const char __user *buf,
						size_t count, loff_t *ppos)
{
	wait_event_interruptible(wind_speed_waitq,ev_press);//如果ev_press=0,将会休眠线程      
	ev_press=0;                                 		//没有发生中断
	int err = copy_to_user(buf, &f, sizeof(f));
	return err? -EFAULT : 0;
}

static char dev_id[] = "wind_speed";
static char *irq_name = dev_id; 

static int wind_speed_release(struct inode *inode, struct file *file)
{
	free_irq(irq, dev_id);					//与request_irq()对应
	del_timer (&mywind_speed.s_timer);
	return 0;
}

static struct file_operations wind_speed_fops = 
{
	.open = wind_speed_open,
	.read = wind_speed_read,
	.release = wind_speed_release,
};

static int wind_speed_mkdev(struct platform_device *pdev)
{
	struct resource *res;
	struct decice_node *node = pdev->dev.of_node;

	printk(KERN_INFO "%s wind_speed found\n", __FUNCTION__);
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res)
	{
		mywind_speed.gpfcon = ioremap(res->start, res->end - res->start + 1);
	}
	else
		printk(KERN_ERR "Can't get the %s resource!\n", DEVICE);

	of_property_read_u32(node, "pin", &mywind_speed.pin);
	printk(KERN_INFO "mywind_speed.pin = %d\n", mywind_speed.pin);

	major = register_chrdev(0, DEVICE, &wind_speed_fops);
	wind_speed_class = class_create(THIS_MODULE, DEVICE);
	dev = device_create(wind_speed_class, NULL, MKDEV(major, 0), NULL, DEVICE);
	return 0;
}

static int wind_speed_remove(struct platform_drvice *pdev)
{
	printk(KERN_INFO "%s remove %s\n", __FUNCTION__, DEVICE);
	device_unregister(dev);
	class_destroy(wind_speed_class);
	unregister_chrdev(major, DEVICE);
	iounmap(mywind_speed.gpfcon);
	return 0;
}


/* 外部中断驱动部分 */
static irqreturn_t tq2440_wind_speed_isr(int irq, void *dev_id)
{
	ev_press = 1;
	//irq_id = irq;
	
	atomic_inc(&mywind_speed.f);
	
	wake_up_interruptible(&wind_speed_waitq);              //唤醒休眠的线程 
    return IRQ_HANDLED;
}

static int wind_speed_probe(struct platform_device *pdev) 
{
    wind_speed_dev = &pdev->dev;

    printk("%s enter.\n", __func__);

    if (!wind_speed_dev->of_node) {
        dev_err(wind_speed_dev, "no platform data.\n");
        return -EINVAL;
    }

	irq = platform_get_irq(pdev, 0);
	printk("%s: get irq %d\n", __func__, irq);
	/*
		devm_request_any_context_irq(wind_speed_dev, irq[i],
		    tq2440_wind_speed_isr, IRQF_TRIGGER_FALLING, irq_name[i], dev_id[i]);
	*/
	request_irq(irq, tq2440_wind_speed_isr, IRQF_TRIGGER_FALLING, irq_name, dev_id);

	wind_speed_mkdev(pdev);
    return 0;
}

static const struct of_device_id wind_speed_dt_ids[] =
{
    { .compatible = "wind_speed", },
    { },
};

MODULE_DEVICE_TABLE(of, wind_speed_dt_ids);

static struct platform_driver wind_speed_driver = 
{
    .driver = 
	{
        .name = "wind_speed_drv",
        .of_match_table = of_match_ptr(wind_speed_dt_ids),
    },
    .probe  = wind_speed_probe,
    .remove = wind_speed_remove,
};

static int __init wind_speed_init(void)
{
    int ret;

    ret = platform_driver_register(&wind_speed_driver);
    if (ret)
        printk(KERN_ERR "wind_speed: probe failed: %d\n", ret);

    return ret;
}
module_init(wind_speed_init);

static void __exit wind_speed_exit(void)
{
    platform_driver_unregister(&wind_speed_driver);
}
module_exit(wind_speed_exit);

MODULE_LICENSE("GPL");
