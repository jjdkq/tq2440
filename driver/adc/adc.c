/*************************************************************************
	> File Name: adc.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年12月01日 星期六 20时02分21秒
 ************************************************************************/

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <linux/wait.h>
#include <linux/clk.h>

#define DEVICE_NAME		"adc"
#define PCLK			50000000
#define ADCCLK			2500000
typedef struct
{
	unsigned long adccon,adctsc,adcdly,adcdat0;		//ADC的相关寄存器
}adc_regs_t;

typedef struct 
{
	wait_queue_head_t adc_waitq;					//定义等待队列(后面还要初始化等待队列)
	unsigned ch;									//输入通道
	unsigned pres;									//时钟分频系数
	unsigned adc_data;								//获取的(数字)值
	unsigned flag;									//中断事件标志
}adc_dev_t;

//或者
//static DECLARE_WAIT_QUEUE_HEAD(adc_waitq);		//定义并初始化等待队列(后面不需要初始化等待队列)

adc_regs_t *adc_regs;
adc_dev_t adc_dev=
{
	.ch=0x02,										//AIN2
	.pres=PCLK/ADCCLK-1,
};

//ADC中断函数
static irqreturn_t adc_interrupt(int irq,void *dev_id)
{
	adc_dev.adc_data=adc_regs->adcdat0&0x3ff;		//读取adc值
	adc_dev.flag=1;									//发生了中断
	wake_up_interruptible(&(adc_dev.adc_waitq));	//唤醒休眠的进程
	return IRQ_RETVAL(IRQ_HANDLED);
}

static int adc_open(struct inode *inode,struct file *file)
{
	unsigned err;

	//将ADC寄存器的物理地址映射为虚拟地址
	adc_regs=ioremap(0x58000000,sizeof(adc_regs_t));
	adc_regs->adccon&=~(1<<14|0xff<<6|7<<3|1<<2|1<<0);		 //清除之前的设置
	adc_regs->adccon|=1<<14|adc_dev.pres<<6|adc_dev.ch<<3|0<<2;	//使用预分频，分频系数=(PCLK/ADCCLK-1),通道2,正常工作模式
	adc_regs->adctsc=~(1<<2);								 //普通转换模式

	init_waitqueue_head(&(adc_dev.adc_waitq));				//初始化等待队列
	//注册adc中断
	err=request_irq(
			IRQ_ADC,										//中断号
			adc_interrupt,									//中断处理函数
			IRQF_SHARED,									//中断触发方式
			DEVICE_NAME,									//中断名称
			&adc_dev
			);
	if(err)
	{
		printk(KERN_ERR"request adc_irq err!\n");
		free_irq(IRQ_ADC,&adc_dev);
		return -EBUSY;
	}
	return 0;
}

static int adc_read(struct file *file,char __user *buff,
					size_t count,loff_t *offp)
{
	unsigned err;
	
	adc_regs->adccon|=1<<0;									 //启动AD转换(这句不能省略，因为转换开始后，位0会被清0,必须手动置1启动转换)
	wait_event_interruptible(adc_dev.adc_waitq,adc_dev.flag);//中断标志为0的话将会休眠进程
	adc_dev.flag=0;											 //没有发生中断
	err=copy_to_user(buff,&adc_dev.adc_data,sizeof(adc_dev.adc_data));		//将转换值从内核空间复制到用户空间
	if(err)
		printk("copy to user error!\n");
	return err? -EFAULT:0;
}

//卸载中断(不能省略这个函数!!!)
static int adc_close(struct inode *inode,struct file *file)
{
	free_irq(IRQ_ADC,&adc_dev);
	return 0;
}

static struct file_operations adc_fops=
{
	.owner=THIS_MODULE,
	.open=adc_open,
	.read=adc_read,
	.release=adc_close
};

static struct miscdevice misc=
{
	.minor=MISC_DYNAMIC_MINOR,
	.name=DEVICE_NAME,
	.fops=&adc_fops
};

static struct clk *adc_clk;
static int __init adc_init(void)
{
	int err=0;
	adc_clk=clk_get(NULL,"adc");		//获取ADC时钟
	clk_enable(adc_clk);				//使能ADC时钟
	
	err=misc_register(&misc);

	if(err<0)
		printk(DEVICE_NAME" register error!\n");
	else
		printk(DEVICE_NAME" register success!\n");
	return 0;
}

static void __exit adc_exit(void)
{
	misc_deregister(&misc);
	iounmap(adc_regs);
	printk(DEVICE_NAME" is over!\n");
}

module_init(adc_init);
module_exit(adc_exit);
MODULE_LICENSE("GPL");
