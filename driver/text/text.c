/*************************************************************************
	> File Name: text.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年12月17日 星期一 10时06分40秒
 ************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define DEVICE_NAME		"text"

static unsigned device_major=231;
static unsigned device_minor=0;
module_param(device_major,int,S_IRUGO);
module_param(device_minor,int,S_IRUGO);

static ssize_t text_open(struct inode *inode,struct file *file)
{
	printk("text open ok\n");
	return 0;
}
static int text_write(struct file *file,const char __user *buf,
					  ssize_t count,loff_t *ppos)
{
	int error;
	char a;
	error=copy_from_user(&a,buf,sizeof(a));
	printk("from user:%d\n",a);
	return error? -1:0;
}
static struct file_operations text_fops=
{
	.owner=THIS_MODULE,
	.open=text_open,
	.write=text_write,
};
static dev_t devno;
static struct cdev *dev;
static struct class *text_class;
static int __init text_init(void)
{
	int ret;
	if(device_major)
	{
		devno=MKDEV(device_major,device_minor);
		ret=register_chrdev_region(devno,1,DEVICE_NAME);
	}
	else
	{
		ret=alloc_chrdev_region(&devno,device_minor,1,DEVICE_NAME);
		device_major=MAJOR(devno);
	}
	if(ret<0)
	{
		printk("add dev_id error!\n");
		return -1;
	}
	dev=cdev_alloc();
	if(dev)
	{
		cdev_init(dev,&text_fops);
		dev->owner=THIS_MODULE;
		if(cdev_add(dev,devno,1))
		{
			printk("add text device error!\n");
			goto error;
		}
	}
	else
	{
		printk("alloc text_dev error!\n");
		return -1;
	}
	text_class=class_create(THIS_MODULE,"text_class");
	if(IS_ERR(text_class))
	{
		printk(KERN_ERR"create text class error!\n");
		return -1;
	}
	device_create(text_class,NULL,devno,NULL,DEVICE_NAME);
	printk(KERN_INFO"text device created completely!\n");
	return 0;
error:
	unregister_chrdev_region(devno,1);
	return -1;
}
static void __exit text_exit(void)
{
	device_destroy(text_class,devno);
	class_destroy(text_class);
	cdev_del(dev);
	unregister_chrdev_region(devno,1);
	printk(KERN_INFO"text device unregisteres completely!\n");
}
module_init(text_init);
module_exit(text_exit);
MODULE_LICENSE("GPL");
