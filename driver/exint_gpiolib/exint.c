/*************************************************************************
	> File Name: exint.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2019年01月08日 星期二 13时56分23秒
 ************************************************************************/

#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/input.h>
#include <asm/uaccess.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <mach/regs-gpio.h>
#include <asm/bitops.h>
#include <linux/delay.h>
#include <linux/irq.h>

//说明:通过input子系统和gpiolib的api来写外部中断驱动

struct key_irq
{
	unsigned int code;					//键值
	unsigned long gpio;					//外部中断对应的gpio
	struct work_struct key_queue;		//工作队列
};

static struct key_irq key_irq[] = 
{
	[0] = 
	{
		.code = KEY_A,
		.gpio = S3C2410_GPF0,
	},
	[1] = 
	{
		.code = KEY_B,
		.gpio = S3C2410_GPF1,
	},
	[2] = 
	{
		.code = KEY_C,
		.gpio = S3C2410_GPF2,
	},
	[3] = 
	{
		.code = KEY_D,
		.gpio = S3C2410_GPF4,
	},
};

static struct input_dev *indev;

//外部中断函数
static irqreturn_t key_interrupt(int irq, void *dev_id)
{
	int i = (int) dev_id;
	int gpio = key_irq[i].gpio;				//获取按键的gpio
	int code = key_irq[i].code;				//获取按键的键值
	udelay(20);
	if(gpio_get_value(gpio))				//获取按键gpio的输入值
		return IRQ_HANDLED;
	input_report_key(indev, code, 1);		//报告按键按下事件
	input_sync(indev);
	schedule_work(&key_irq[i].key_queue);	//提交工作队列,调用key_scan()函数
	return IRQ_HANDLED;
}

static void key_scan(struct work_struct *queue)
{
	struct key_irq *key_irq_temp = container_of(queue, struct key_irq, key_queue);
	int gpio = key_irq_temp->gpio;
	int code = key_irq_temp->code;
	//每隔10ms检查按键是否松开
	while(!gpio_get_value(gpio))
		mdelay(10);
	input_report_key(indev, code, 0);		//报告按键松开事件
	input_sync(indev);
}

static int __init input_init(void)
{
	int ret = 0, i = 0;
	int code, gpio;
	int irq_no = 0;
	indev = input_allocate_device();		//为输入设备驱动对象申请内存空间
	if(!indev)
	{
		printk(KERN_ERR "Allocate the input device error!\n");
		return -ENOMEM;
	}
	indev->name = "My input";
	set_bit(EV_KEY, indev->evbit);			//设置输入设备支持按键事件
	for(i = 0; i < (sizeof(key_irq) / sizeof(key_irq[0])); i++)
	{
		code = key_irq[i].code;
		gpio = key_irq[i].gpio;
		INIT_WORK(&key_irq[i].key_queue, key_scan);	//初始化工作队列
		set_bit(code, indev->keybit);				//设置输入设备的键值
		gpio_free(gpio);
		ret = gpio_request(gpio, "My gpio");		//申请gpio
		if(ret)
		{
			printk(KERN_ERR "gpio failed!\n");
			return -EBUSY;
		}
		gpio_direction_input(gpio);					//gpio设为输入
		irq_no = gpio_to_irq(gpio);
		//申请中断,中断触发方式为下降沿触发
		ret = request_irq(irq_no, key_interrupt, IRQF_TRIGGER_FALLING, 
						"My irq", (void *)i);
		if(ret)
		{
			printk(KERN_ERR"Request irq error!\n");
			return -EBUSY;
		}
		input_register_device(indev);				//注册输入设备驱动
		printk(KERN_INFO "input device is OK!\n");
	}
	return 0;
}
static void __exit input_exit(void)
{
	int i, irq_no = 0;
	for(i = 0; i < (sizeof(key_irq) / sizeof(key_irq[0])); i++)
	{
		free_gpio(key_irq[i].gpio);
		irq_no = gpio_to_irq(key_irq[i].gpio);
		free_irq(irq_no, (void *)i);
	}
	input_unregister_device(indev);					//注销输入设备驱动
	printk(KERN_INFO "input device is over!\n");
}
module_init(input_init);
module_exit(input_exit);
MODULE_LICENSE("GPL");
