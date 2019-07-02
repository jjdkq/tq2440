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
#include <linux/delay.h>
#include <asm/uaccess.h>

#define DEVICE_NAME		"dht11"		/*注册驱动时建立的设备名称*/ 
#define DHT11_MAJOR		231			//DHT11主设备号

#define DHT11_NOEXIT	1			//DHT11不存在
#define DHT11_EXIT		0			//DHT11存在

//DHT11设为输出
static void dht11_out(void)
{
	s3c2410_gpio_cfgpin(S3C2410_GPB5,S3C2410_GPB5_OUTP);
}
//DHT11设为输入
static void dht11_in(void)
{
	s3c2410_gpio_cfgpin(S3C2410_GPB5,S3C2410_GPB5_INP);
}
//DHT输出电平
static void dht11_setpin(int x)
{
	s3c2410_gpio_setpin(S3C2410_GPB5,x);
}
//DHT11输入电平
static int dht11_getpin(void)
{
	if(s3c2410_gpio_getpin(S3C2410_GPB5))
		return 1;
	return 0;
}
//复位DHT11
static void dht11_rst(void)
{
	dht11_out();					//dht11设为输出
	dht11_setpin(0);				//dht11输出0
	mdelay(20);						//延时20ms
	dht11_setpin(1);				//dht11输出1
	udelay(30);						//延时30us
}
//检查DHT11是否存在
static int dht11_check(void)
{
	int t=0;
	dht11_in();						//dht11设为输入
	while(dht11_getpin()&&t<100)
	{
		t++;
		udelay(1);					//延时1us
	}
	if(t>=100)
		return DHT11_NOEXIT;		
	t=0;
	while(!dht11_getpin()&&t<100)
	{
		t++;
		udelay(1);
	}
	if(t>=100)
		return DHT11_NOEXIT;
	return DHT11_EXIT;
}
static int dht11_open(struct inode *inode,struct file *file)
{
	dht11_rst();
	while (dht11_check())			//没有检测到dht11
	{
		printk(KERN_ERR "No dht11!Please check!\n");
	}
	return dht11_check();
}
//DHT11读取1个位
static int dht11_read_onebit(void)
{
	int t=0;
	dht11_in();
	while(dht11_getpin()&&t<100)
	{
		t++;
		udelay(1);
	}
	t=0;
	while(!dht11_getpin()&&t<100)
	{
		t++;
		udelay(1);
	}
	udelay(40);
	if(dht11_getpin())
		return 1;
	return 0;
}
//dht11读取1个字节
static int dht11_read_onebyte(void)
{
	int i=0,temp=0;
	for(;i<8;i++)
	{
		temp<<=1;
		temp|=dht11_read_onebit();
	}
	return temp;
}
//dht11获取数据
static void dht11_getdata(int *datap)
{
	int data[5],i=0;
	dht11_rst();
	if(!dht11_check())
	{
		for(;i<5;i++)
			data[i]=dht11_read_onebyte();
		if((data[0]+data[1]+data[2]+data[3])==data[4])		//校验数据
		{
			for(i=0;i<5;i++)
				datap[i]=data[i];
		}
	}
}
static int dht11_read(struct file *file,char __user *buff,
					size_t count,loff_t *offp)
{
	unsigned long err=0;
	static int data[5];
	dht11_getdata(data);
	//将得到的数据从内核空间复制到用户空间
	err=copy_to_user(buff,(const void *)data,sizeof(data));
	memset((void *)data,0,sizeof(data));				//清零空间
	return err? -EFAULT:0;
}
static struct file_operations dht11_fops=
{
	.owner=THIS_MODULE,
	.open=dht11_open,
	.read=dht11_read
};
static struct miscdevice misc = 
{   
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dht11_fops,
};

static int __init dht11_init(void)
{
	int i=0;
//	i=register_chrdev(DHT11_MAJOR,DEVICE_NAME,&led_fops);	//注册字符设备(需要手动mknod设备)
	i=misc_register(&misc);							//(自动)注册杂项设备(主设备号:10,自动分配次设备号)
	if(i<0)
		printk(DEVICE_NAME" can't register!\n");
	else
		printk(DEVICE_NAME" inittialized\n");
	return i;
}
static void __exit dht11_exit(void)
{
//	unregister_chrdev(DHT11_MAJOR,DEVICE_NAME);		//卸载驱动
	misc_deregister(&misc);							//卸载驱动
	printk ("my "DEVICE_NAME" is over!\n");
}
module_init(dht11_init);
module_exit(dht11_exit);
MODULE_LICENSE("GPL");
