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
static int led2_open(struct inode *inode,struct file *file)
{
	int i=0;
	for(i=2;i<4;i++)
	{
		s3c2410_gpio_cfgpin(led_table[i],led_cfg_table[i]);//led的io设为输出
	}
	return 0;
}
extern int led_ioctl(
	struct inode *inode,
	struct file *file,
	unsigned int cmd,
	unsigned long arg);
	
struct file_operations led2_fops=
{
	.owner=THIS_MODULE,
	.open=led2_open,
	.ioctl=led_ioctl
};

