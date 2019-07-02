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
#define DEVICE_NAME "led"		/*注册驱动时建立的设备名称*/ 
#define LED_MAJOR	231			//LED主设备号
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
	printk ("my "DEVICE_NAME" is over!\n");
}
module_init(led_init);
module_exit(led_exit);
//MODULE_LICENSE("GPL");
