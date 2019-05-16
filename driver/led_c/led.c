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
#include <linux/cdev.h>

#define DEVICE_NAME "led"		/*注册驱动时建立的设备名称*/ 

static int major=231;			//主设备号
static int minor=0;				//次设备号
module_param(major,int,S_IRUGO);
module_param(minor,int,S_IRUGO);

struct cdev *led_dev;
static dev_t devno;				//设备编号
static struct class *led_class;

static unsigned long led_table[]=
{
	S3C2410_GPB5,
	S3C2410_GPB6,
	S3C2410_GPB7,
	S3C2410_GPB8
};
static unsigned long led_cfg_table[]=
{
	S3C2410_GPB5_OUTP,
	S3C2410_GPB6_OUTP,
	S3C2410_GPB7_OUTP,
	S3C2410_GPB8_OUTP
};
static int led_open(struct inode *inode,struct file *file)
{
	int i=0;
	for(i=0;i<4;i++)
	{
		s3c2410_gpio_cfgpin(led_table[i],led_cfg_table[i]);//led的io设为输出
	}
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
			s3c2410_gpio_setpin(led_table[arg],0);		//点亮led
			return 0;
		case 1:
			s3c2410_gpio_setpin(led_table[arg],1);		//熄灭led
			return 0;
		default:
			return -EINVAL;
	}
}
static struct file_operations led_fops=
{
	.owner=THIS_MODULE,
	.open=led_open,
	.ioctl=led_ioctl
};

static int __init led_init(void)
{
	int ret=0;
	
	if(major>0)										//静态分配设备号
	{
		devno=MKDEV(major,minor);					//构建设备号
		ret=register_chrdev_region(devno,1,DEVICE_NAME);//注册主设备号(可以不要这句)
	}
	else											//动态分配设备号
	{
		ret=alloc_chrdev_region(&devno,minor,1,DEVICE_NAME);//从系统获取主设备号
		major=MAJOR(devno);							//保存获取的主设备号(可以不要这句)
	}
	if(ret<0)										
	{
		printk(KERN_ERR"can't get major %d\n",major);
		return -1;
	}
	led_dev=cdev_alloc();							//为led_dev结构体分配空间
	if(led_dev)										//成功分配空间
	{
		cdev_init(led_dev,&led_fops);				//初始化led_dev
		led_dev->owner=THIS_MODULE;
		if(cdev_add(led_dev,devno,1))				//添加字符设备失败
		{
			printk(KERN_ERR"add led dev error\n");
			goto error;
		}
	}
	else
	{
		printk(KERN_ERR"alloc led_dev error!\n");
		return -1;
	}
	led_class=class_create(THIS_MODULE,"led_class");//创建led类
	if(IS_ERR(led_class))
	{
		printk(KERN_ERR"create class error!\n");
		return -1;
	}
	device_create(led_class,NULL,devno,NULL,DEVICE_NAME);//创建设备文件/dev/led(没有这句的话需要手动mknod)
	printk(KERN_INFO"led device created completely!\n");
	return 0;
error:
	unregister_chrdev_region(devno,1);					//释放设备编号
	return -1;
}
static void __exit led_exit(void)
{
	device_destroy(led_class,devno);
	class_destroy(led_class);
	cdev_del(led_dev);									//移除字符设备
	unregister_chrdev_region(devno,1);
	printk(KERN_INFO"led device unregistered completely!\n");
}
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
