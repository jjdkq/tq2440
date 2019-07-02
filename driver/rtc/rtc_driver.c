#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/sched.h> 
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <linux/bcd.h>
 
#include <linux/platform_device.h>
#include <linux/io.h>
#include "rtc.h"
 
static int major;
 
static struct class *cls;
static struct device *dev;

#define rtccon_o	0x00	//rtc控制寄存器
#define ticnt_o		0x04	//rtc滴答中断控制寄存器
#define rtcalm_o	0x10	//rtc报警使能寄存器
/*rtc报警时间寄存器*/
#define almsec_o	0x14
#define almmin_o	0x18
#define almhour_o	0x1C
#define almdate_o	0x20
#define almmon_o	0x24
#define almyear_o	0x28
/*rtc时间设定寄存器*/
#define bcdsec_o	0x30
#define bcdmin_o	0x34
#define bcdhour_o	0x38
#define bcddate_o	0x3C
#define bcdday_o	0x40
#define bcdmon_o	0x44
#define bcdyear_o	0x48

//需要用到的相关rtc寄存器的虚拟地址
static unsigned char *rtcbase;
static unsigned char *rtccon;
static unsigned char *bcdyear;
static unsigned char *bcdmon;
static unsigned char *bcddate;
static unsigned char *bcdday;
static unsigned char *bcdhour;
static unsigned char *bcdmin;
static unsigned char *bcdsec;

static int rtc_open(struct inode *inode, struct file *file)
{
	writeb(1,rtccon);				//使能rtc控制	
	writeb(0x20,bcdyear);			//设置年
	writeb(0x12,bcdmon);			//月
	writeb(0x30,bcddate);			//日
	writeb(0x00,bcdday);			//星期
	writeb(0x22,bcdhour);			//小时	
	writeb(0x13,bcdmin);			//分
	writeb(0x10,bcdhour);			//秒
	writeb(readb(rtccon)&~1,rtccon);//禁止rtc控制
	return 0;
}
/*
typedef struct
{
	unsigned short year;
	unsigned mon;
	unsigned day;
	unsigned date;
	unsigned week;
	unsigned hour;
	unsigned min;
	unsigned sec;
}time_t_;
*/

#define	RD_TIME	1	

static int rtc_ioctl(struct inode *inode, struct file *file,
	unsigned int cmd, unsigned long arg)
{
	static time_t_ time;
	int err=1;
	void __user *user_arg=(void __user *)arg;
	writeb(1,rtccon);				//使能rtc控制
	switch (cmd)
	{
		case RD_TIME:
			time.year=readb(bcdyear)+0x2000;
			time.mon=readb(bcdmon);
			time.date=readb(bcddate);
			time.day=readb(bcdday);
			time.hour=readb(bcdhour);
			time.min=readb(bcdmin);
			time.sec=readb(bcdsec);
			err=copy_to_user(user_arg, &time, sizeof(time));
		break;
	}
	writeb(readb(rtccon)&~1,rtccon);//禁止rtc控制
	return err? -1:0;
}
 
static struct file_operations rtc_fops = 
{
	.owner = THIS_MODULE,
	.open  = rtc_open,
	.ioctl = rtc_ioctl,
};
 
static int rtc_probe(struct platform_device *pdev)
{
	struct resource *res;
	// 最后一个参数 0 表示第1个该类型的资源
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);//获得第1个IO内存资源
	rtcbase= ioremap(res->start, res->end - res->start + 1);
	rtccon=rtcbase+rtccon_o;
	bcdyear=rtcbase+bcdyear_o;
	bcdmon=rtcbase+bcdmon_o;
	bcddate=rtcbase+bcddate_o;
	bcdday=rtcbase+bcdday_o;
	bcdhour=rtcbase+bcdhour_o;
	bcdmin=rtcbase+bcdmin_o;
	bcdsec=rtcbase+bcdsec_o;
 
	printk("rtc_probe, found rtc\n");
 
	// 注册设备驱动 创建设备节点
	major = register_chrdev(0, "myrtc", &rtc_fops);
	// 创建类
	cls = class_create(THIS_MODULE, "myrtc");
	// 创建设备节点
	dev = device_create(cls, NULL, MKDEV(major, 0), NULL, "rtc");
 
	return 0;
}
 
static int rtc_remove(struct platform_device *pdev)
{ 
	printk("rtc_remove, remove rtc\n");	
	// 删除设备节点
	device_unregister(dev);
	// 销毁类
	class_destroy(cls);
	// 取消注册设备驱动
	unregister_chrdev(major, "myrtc");
	// 取消内存映射
	iounmap(rtcbase);
 
	return 0;
}
 
struct platform_driver rtc_drv = 
{
	.probe 	= rtc_probe,	//匹配到dev之后调用probe
	.remove = rtc_remove,
	.driver = 
	{
		.name = "myrtc",	//要和platform_device中的name相同
	},
};
 
static int rtc_drv_init(void){
 
	platform_driver_register(&rtc_drv);
	return 0;
}
 
static void rtc_drv_exit(void){
	
	platform_driver_unregister(&rtc_drv);
}
 
module_init(rtc_drv_init);
module_exit(rtc_drv_exit);
MODULE_LICENSE("GPL");

