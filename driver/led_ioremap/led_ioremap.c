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
#include <linux/io.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "led"		/*注册驱动时建立的设备名称*/ 
#define LED_MAJOR	231			//LED主设备号

typedef struct
{
	unsigned long gpbcon,gpbdat;
}led_regs_t;					//定义led引脚寄存器
led_regs_t *led_regs;
static int led_open(struct inode *inode,struct file *file)
{
	//将led引脚寄存器的物理地址映射到虚拟地址
	led_regs=ioremap(0x56000010,sizeof(led_regs_t));
	led_regs->gpbcon&=~(0xff<<10);
	led_regs->gpbcon|=0x55<<10;
	return 0;
}
static int led_ioctl(
	struct inode *inode,
	struct file *file,
	unsigned int cmd,
	unsigned long arg)
{
	if(arg>4)
		return -EINVAL;
	switch(cmd)
	{
		case 0:
			led_regs->gpbdat&=~(1<<(arg+5));
			printk("GPB5 on\n");
			return 0;
		case 1:
			led_regs->gpbdat|=1<<(arg+5);
			printk("GPB5 off\n");
			return 0;
		default:
			return -EINVAL;
	}
}

typedef enum
{
	LED1_ON=0,
	LED1_OFF,
	LED2_ON,
	LED2_OFF,
	LED3_ON,
	LED3_OFF,
	LED4_ON,
	LED4_OFF
}led_sta;

static ssize_t led_write(struct file *file,const char __user *buff,										size_t count,loff_t *offp)
{
	led_sta temp;
	unsigned err;
	err=copy_from_user(&temp,buff,sizeof(temp));
	if(err)
		printk("copy from user err\n");
	switch(temp)
	{
		case LED1_ON:
			led_regs->gpbdat&=~(1<<5);
		break;
		case LED1_OFF:
			led_regs->gpbdat|=1<<5;
		break;
		case LED2_ON:
			led_regs->gpbdat&=~(1<<6);
		break;
		case LED2_OFF:
			led_regs->gpbdat|=1<<6;
		break;
		case LED3_ON:
			led_regs->gpbdat&~(1<<7);
		break;
		case LED3_OFF:
			led_regs->gpbdat|=1<<7;
		break;
		case LED4_ON:
			led_regs->gpbdat&=~(1<<8);
		break;
		case LED4_OFF:
			led_regs->gpbdat|=1<<8;
		break;
	}
	return err? -EFAULT:0;
}

static struct file_operations led_fops=
{
	.owner=THIS_MODULE,
	.open=led_open,
	.ioctl=led_ioctl,
	.write=led_write
};
static struct miscdevice misc = 
{   
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &led_fops,
};

static int __init led_init(void)
{
	int i=0;
//	i=register_chrdev(LED_MAJOR,DEVICE_NAME,&led_fops);	//注册字符设备(需要手动mknod led设备)
	i=misc_register(&misc);							//(自动)注册杂项设备(主设备号:10,自动分配次设备号)
	if(i<0)
		printk("led can't register!\n");
	else
		printk("myled inittialized\n");
	return i;
}
static void __exit led_exit(void)
{
//	unregister_chrdev(LED_MAJOR,DEVICE_NAME);		//卸载驱动
	misc_deregister(&misc);							//卸载驱动
	iounmap(led_regs);
	printk ("my "DEVICE_NAME" is over!\n");
}
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
