/*************************************************************************
	> File Name: cd4051.c
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
#include <asm/uaccess.h>
#include <linux/compiler.h>

#include <linux/gpio.h>			//for gpio_direction_output()

#define DEVICE_NAME "cd4051"		/*注册驱动时建立的设备名称*/ 
#define cd4051_MAJOR	231			//cd4051主设备号
static unsigned long cd4051_table[]=
{
	S3C2410_GPB5,
	S3C2410_GPB6,
	S3C2410_GPB7,
};
static unsigned long cd4051_cfg_table[]=
{
	S3C2410_GPB5_OUTP,
	S3C2410_GPB6_OUTP,
	S3C2410_GPB7_OUTP,
};
static int cd4051_open(struct inode *inode,struct file *file)
{
	int i=0;
	for(i=0;i<3;i++)
	{
		s3c2410_gpio_cfgpin(cd4051_table[i],cd4051_cfg_table[i]);//cd4051的io设为输出
	}
	return 0;
}
static int cd4051_ioctl(
	struct inode *inode,
	struct file *file,
	unsigned int cmd,
	unsigned long arg)
{
	int i;
	switch (cmd)
	{
		case 0:											//通道0
			for(i = 0; i < 3; i++)
				s3c2410_gpio_setpin(cd4051_table[i],0);	
			return 0;
		case 1:											//通道1
			s3c2410_gpio_setpin(cd4051_table[0],1);
			s3c2410_gpio_setpin(cd4051_table[1],0);
			s3c2410_gpio_setpin(cd4051_table[2],0);
			return 0;
		case 2:
			s3c2410_gpio_setpin(cd4051_table[0],0);
			s3c2410_gpio_setpin(cd4051_table[1],1);
			s3c2410_gpio_setpin(cd4051_table[2],0);
			return 0;
		case 3:
			s3c2410_gpio_setpin(cd4051_table[0],1);
			s3c2410_gpio_setpin(cd4051_table[1],1);
			s3c2410_gpio_setpin(cd4051_table[2],0);
			return 0;
		case 4:
			s3c2410_gpio_setpin(cd4051_table[0],0);
			s3c2410_gpio_setpin(cd4051_table[1],0);
			s3c2410_gpio_setpin(cd4051_table[2],1);
			return 0;
		case 5:
			s3c2410_gpio_setpin(cd4051_table[0],1);
			s3c2410_gpio_setpin(cd4051_table[1],0);
			s3c2410_gpio_setpin(cd4051_table[2],1);
			return 0;
		case 6:
			s3c2410_gpio_setpin(cd4051_table[0],1);
			s3c2410_gpio_setpin(cd4051_table[1],1);
			s3c2410_gpio_setpin(cd4051_table[2],0);
			return 0;
		case 7:											
			for(i = 0; i < 3; i++)
				s3c2410_gpio_setpin(cd4051_table[i],1);	
			return 0;
		default:
			return -EINVAL;
	}
}
static struct file_operations cd4051_fops=
{
	.owner=THIS_MODULE,
	.open=cd4051_open,
	.ioctl=cd4051_ioctl
};
static struct miscdevice misc = 
{   
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &cd4051_fops,
};

static int __init cd4051_init(void)
{
	int i=0;
//	i=register_chrdev(cd4051_MAJOR,DEVICE_NAME,&cd4051_fops);	//注册字符设备(需要手动mknod cd4051设备)
	i=misc_register(&misc);							//(自动)注册杂项设备(主设备号:10,自动分配次设备号)
	if(i<0)
		printk("cd4051 can't register!\n");
	else
		printk("mycd4051 inittialized\n");
	return i;
}
static void __exit cd4051_exit(void)
{
//	unregister_chrdev(cd4051_MAJOR,DEVICE_NAME);		//卸载驱动
	misc_deregister(&misc);							//卸载驱动
	printk ("my "DEVICE_NAME" is over!\n");
}
module_init(cd4051_init);
module_exit(cd4051_exit);
MODULE_LICENSE("GPL");
