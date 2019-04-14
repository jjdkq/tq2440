/*************************************************************************
	> File Name: led.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年11月21日 星期三 15时15分56秒
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
#include <asm/uaccess.h>
#include <linux/poll.h>

#define KEY_DEVICE_NAME	"key"
#define LED_DEVICE_NAME	"led"
#define LED_MAJOR		231

//按键IO
static unsigned long key_table[]=
{
	S3C2410_GPF1,
	S3C2410_GPF4,
	S3C2410_GPF2,
	S3C2410_GPF0
};

//按键输入
static unsigned long key_cfg_table[]=
{
	S3C2410_GPF1_EINT1,
	S3C2410_GPF4_EINT4,
	S3C2410_GPF2_EINT2,
	S3C2410_GPF0_EINT0,
};

//声明按键中断结构体
typedef struct 
{
	int irq;				//中断号
	unsigned long flags;	//中断标志，用来定义中断触发方式
	char *name;				//中断名称
}key_irq_t;

//定义并初始化按键中断结构体数组
key_irq_t key_irq[]=
{
	{IRQ_EINT1,IRQF_TRIGGER_FALLING,"KEY1"},	//按键1，下降沿触发中断
	{IRQ_EINT4,IRQF_TRIGGER_FALLING,"KEY2"},	//按键2，下降沿触发中断
	{IRQ_EINT2,IRQF_TRIGGER_FALLING,"KEY3"},	//按键3，下降沿触发中断
	{IRQ_EINT0,IRQF_TRIGGER_FALLING,"KEY4"},	//按键4，下降沿触发中断
};

volatile int press_cnt[4];
int ev_press;									//中断事件标志
static DECLARE_WAIT_QUEUE_HEAD(key_waitq);		//定义并初始化等待队列

//或者
//wait_queue_head_t key_waitq;					//定义初始化队列(后面还需要初始化等待队列)

//中断处理函数
static irqreturn_t key_interrupt(int irq,void *dev_id)
{
	//volatile int *cnt=(volatile int *)dev_id;
	//*cnt=*cnt+1;									//按键计数
	
	*(int *)dev_id=*(int *)dev_id+1;				//按键计数(这里的dev_id相当于press_cnt)

	ev_press=1;										//发生了中断
	wake_up_interruptible(&key_waitq);				//唤醒休眠的进程
	return IRQ_RETVAL(IRQ_HANDLED);
}

static int key_open(struct inode *inode,struct file *file)
{
	int i=0;
	int err=0;
	for(i=0;i<4;i++)
	{
		s3c2410_gpio_cfgpin(key_table[i],key_cfg_table[i]);//key的io设为中断
		s3c2410_gpio_pullup(key_table[i],1);			//禁止key的io的上拉功能
	}
	//初始化等待队列
	//init_waitqueue_head(&key_waitq);
	
	//注册中断处理函数
	for(i=0;i<sizeof(key_irq)/sizeof(key_irq[0]);i++)
	{
		err=request_irq(
				key_irq[i].irq,						//中断号
				key_interrupt,						//中断处理函数
				key_irq[i].flags,					//中断触发方式
				key_irq[i].name,					//中断名称
				(void *)&press_cnt[i]				//按键所按(中断发生)次数
				);
		if(err)										//注册出错
			break;
	}
	if(err)		//如果出错，释放已注册函数
	{
		i--;						
		for(;i>=0;i--)
			free_irq(key_irq[i].irq,(void *)&press_cnt[i]);
		return -EBUSY;
	}

	return 0;
}

//卸载中断处理函数(不能省略!!!)
static int key_close(struct inode *inode,struct file *file)
{
	int i=0;
	for(i=0;i<sizeof(key_irq)/sizeof(key_irq[0]);i++)
	{
		free_irq(key_irq[i].irq,(void *)&press_cnt[i]);
	}
	return 0;
}

static int key_read(struct file *filp,char __user *buff,
		size_t count,loff_t *offp)
{
	unsigned long err;
	wait_event_interruptible(key_waitq,ev_press);//如果ev_press=0,将会休眠进程		
	ev_press=0;									//没有发生中断
	err=copy_to_user(buff,(const void *)press_cnt,min(sizeof(press_cnt),count));				//将按键状态从内核空间复制到用户空间
	//err=copy_to_user(buff,(const void *)press_cnt,sizeof(press_cnt));				//将按键状态从内核空间复制到用户空间
	memset((void *)press_cnt,0,sizeof(press_cnt));//清零当前空间(按键状态)
	return err? -EFAULT:0;
}

static unsigned int key_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;
	/* 将进程挂在key_waiq队列上，不休眠进程 */
	poll_wait(file, &key_waitq, wait);
	if (ev_press)							//按下了按键（进入了中断）
	{
		mask |= POLLIN | POLLRDNORM;		//数据可读
	}
	return mask;

}

static struct file_operations key_fops=
{
	.owner=THIS_MODULE,
	.open=key_open,
	.release=key_close,
	.read=key_read,
	.poll = key_poll,
};
static struct miscdevice misc_key = 
{   
	.minor = MISC_DYNAMIC_MINOR,
	.name = KEY_DEVICE_NAME,
	.fops = &key_fops,
};

extern struct file_operations led_fops;
extern struct miscdevice misc_led;
static int __init mykey_init(void)
{
	int i=0,a=0;
//	i=register_chrdev(LED_MAJOR,LED_DEVICE_NAME,&led_fops);	//注册字符设备(需要手动mknod key设备)
	i=misc_register(&misc_led);							//(自动)注册杂项设备(主设备号:10,自动分配次设备号)
	a=misc_register(&misc_key);							//(自动)注册杂项设备(主设备号:10,自动分配次设备号)
	if(i<0||a<0)
	{
		if(i<0)
			printk("led can't register!\n");
		else
			printk("key can't register!\n");
	}
	else
		printk("Key and led inittialized\n");
	return 0;
}
static void __exit mykey_exit(void)
{
//	unregister_chrdev(LED_MAJOR,LED_DEVICE_NAME);				//卸载驱动
	misc_deregister(&misc_key);						//卸载驱动
	misc_deregister(&misc_led);						//卸载驱动
	printk (KEY_DEVICE_NAME" and "LED_DEVICE_NAME" is over!\n");
}
module_init(mykey_init);
module_exit(mykey_exit);
MODULE_LICENSE("GPL");
