/*************************************************************************
    > File Name: relay.c
    > Author: zhoubo
    > Mail: 1508540876@qq.com
    > Created Time: 2018年11月21日 星期三 15时15分56秒
 ************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>

#define DEVICE_NAME "relay"		/*注册驱动时建立的设备名称*/
#define relay_MAJOR	231

#define RELAY_ON    0
#define RELAY_OFF   1

typedef struct
{
    unsigned long gpbcon,gpbdat;
}relay_regs_t;					//定义relay引脚寄存器
relay_regs_t *relay_regs;
static int relay_open(struct inode *inode,struct file *file)
{
    //将relay引脚寄存器的物理地址映射到虚拟地址
    relay_regs = ioremap(0x56000010, sizeof(relay_regs_t));
    relay_regs->gpbcon &= ~(3 << 10);
    relay_regs->gpbcon |= 1 << 10;
    return 0;
}

static int relay_ioctl(
    struct inode *inode,
    struct file *file,
    unsigned int cmd,
    unsigned long arg)
{
    if (arg > 4)
        return -EINVAL;
    switch (cmd)
    {
        case RELAY_ON:
            relay_regs->gpbdat &= ~(1 << 5);
            return 0;
        case RELAY_OFF:
            relay_regs->gpbdat |= 1 << 5;
            return 0;
        default:
            return -EINVAL;
    }
}

static struct file_operations relay_fops =
{
    .owner = THIS_MODULE,
    .open  = relay_open,
    .ioctl = relay_ioctl,
};
static struct miscdevice misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &relay_fops,
};

static int __init relay_init(void)
{
    int i = -1;
//	i=register_chrdev(relay_MAJOR,DEVICE_NAME,&relay_fops);	//注册字符设备(需要手动mknod relay设备)
    i = misc_register(&misc);							//(自动)注册杂项设备(主设备号:10,自动分配次设备号)
    if (i < 0)
        printk("relay can't register!\n");
    else
        printk("myrelay inittialized\n");
    return i;
}
static void __exit relay_exit(void)
{
//	unregister_chrdev(relay_MAJOR,DEVICE_NAME);		//卸载驱动
    misc_deregister(&misc);							//卸载驱动
    iounmap(relay_regs);
    printk("my "DEVICE_NAME" is over!\n");
}
module_init(relay_init);
module_exit(relay_exit);
MODULE_LICENSE("GPL");

