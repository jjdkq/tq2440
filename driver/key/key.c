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
#define KEY_DEVICE_NAME	"key"
#define LED_DEVICE_NAME	"led"
#define LED_MAJOR		231
static unsigned long key_table[]=
{
	S3C2410_GPF0,
	S3C2410_GPF1,
	S3C2410_GPF2,
	S3C2410_GPF4
};
static unsigned long key_cfg_table[]=
{
	S3C2410_GPF0_INP,
	S3C2410_GPF1_INP,
	S3C2410_GPF2_INP,
	S3C2410_GPF4_INP,
};
static int key_open(struct inode *inode,struct file *file)
{
	int i=0;
	for(i=0;i<4;i++)
	{
		s3c2410_gpio_cfgpin(key_table[i],key_cfg_table[i]);//key的io设为输入
		s3c2410_gpio_pullup(key_table[i],1);			//禁止key的io的上拉功能
	}
	return 0;
}
void delay(int i)
{
	unsigned long x=0,y=0;
	for(x=5000;x>0;x--)
		for(y=i;y>0;y--);
}
static int key_ioctl(
	struct inode *inode,
	struct file *file,
	unsigned int cmd,
	unsigned long arg)
{
	int i=0;
	if(s3c2410_gpio_getpin(key_table[0])==0||s3c2410_gpio_getpin(key_table[1])==0||s3c2410_gpio_getpin(key_table[2])==0||s3c2410_gpio_getpin(key_table[3])==0)
	{
		delay(20);				//消抖
		if(s3c2410_gpio_getpin(key_table[0])==0)
			return 1;
		if(s3c2410_gpio_getpin(key_table[1])==0)
			return 2;
		if(s3c2410_gpio_getpin(key_table[2])==0)
			return 3;
		if(s3c2410_gpio_getpin(key_table[3])==0)
			return 4;
	}
	return 0;
}
static struct file_operations key_fops=
{
	.owner=THIS_MODULE,
	.open=key_open,
	.ioctl=key_ioctl
};
static struct miscdevice misc_key = 
{   
	.minor = MISC_DYNAMIC_MINOR,
	.name = KEY_DEVICE_NAME,
	.fops = &key_fops,
};

extern struct file_operations led_fops;
extern struct miscdevice misc_led;
static int __init key_init(void)
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
static void __exit key_exit(void)
{
//	unregister_chrdev(LED_MAJOR,LED_DEVICE_NAME);				//卸载驱动
	misc_deregister(&misc_key);						//卸载驱动
	misc_deregister(&misc_led);						//卸载驱动
	printk (KEY_DEVICE_NAME" and "LED_DEVICE_NAME" is over!\n");
}
module_init(key_init);
module_exit(key_exit);
MODULE_LICENSE("GPL");
