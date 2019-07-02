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

#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/io.h>

#define DEVICE_NAME		"dht11"		/*注册驱动时建立的设备名称*/ 
#define DHT11_MAJOR		231			//DHT11主设备号

#define DHT11_NOEXIT	1			//DHT11不存在
#define DHT11_EXIT		0			//DHT11存在

struct dht11
{
	volatile unsigned long *gpbcon;
	volatile unsigned long *gpbdat;
	unsigned char pin;
};

static volatile struct dht11 dht11;

//DHT11设为输出
static void dht11_out(void)
{
	u32 temp = readl(dht11.gpbcon);
	temp &= ~(3 << (dht11.pin * 2));
	temp |= 1 << (dht11.pin * 2);
	writel(temp, dht11.gpbcon);
}
//DHT11设为输入
static void dht11_in(void)
{
	u32 temp = readl(dht11.gpbcon);
	temp &= ~(3 << (dht11.pin * 2));
	writel(temp, dht11.gpbcon);
}
//DHT输出电平
static void dht11_setpin(int x)
{
	u32 temp = readl(dht11.gpbdat);
	temp &= ~(1 << dht11.pin);
	temp |= x << dht11.pin;
	writel(temp, dht11.gpbdat);
}
//DHT11输入电平
static int dht11_getpin(void)
{
	if (readl(dht11.gpbdat) & ( 1 << dht11.pin))
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
	if (dht11_check())			//没有检测到dht11
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

static int dht11_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	printk(KERN_INFO "%s enter\n", __func__);
	if (!node)
	{
		dev_dbg(dev, "error!of_node is null\n");
		return -EINVAL;
	}
	struct resource *res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "gpbcon");
	if (res)
	{
//		dht11.gpbcon = ioremap(res->start, res->end - res->start + 1);
//		dht11.gpbdat = dht11.gpbcon + 1;
//或者
		dht11.gpbcon = devm_ioremap_resource(dev, res);
		dht11.gpbdat = dht11.gpbcon + 1;
	}
	else
	{
		printk(KERN_ERR "Can't get the mem resource\n");
		return -EINVAL;
	}
	unsigned temp;
	of_property_read_u32(node, "pin", &temp);
	dht11.pin = (unsigned char)temp;

	int i=0;
//	i=register_chrdev(DHT11_MAJOR,DEVICE_NAME,&led_fops);	//注册字符设备(需要手动mknod设备)
	i=misc_register(&misc);							//(自动)注册杂项设备(主设备号:10,自动分配次设备号)
	if(i<0)
		printk(DEVICE_NAME" can't register!\n");
	else
		printk(DEVICE_NAME" inittialized\n");
	return i;
}
static int dht11_remove(struct platform_device *pdev)
{
//	unregister_chrdev(DHT11_MAJOR,DEVICE_NAME);		//卸载驱动
	printk(KERN_INFO "%s enter\n", __func__);
	misc_deregister(&misc);							//卸载驱动
//	iounmap(dht11.gpbcon);
//或者
	devm_iounmap(&pdev->dev, dht11.gpbcon);

	printk ("my "DEVICE_NAME" is over!\n");
}

static const struct of_device_id dht11_compat[] =
{
	{ .compatible = "dht11", .data = NULL },
	{ }
};

static struct platform_driver dht11_driver =
{
	.probe = dht11_probe,
	.remove = dht11_remove,
	.driver = 
	{
		.name = "dht11_drv",
		.of_match_table = dht11_compat,
	},
};

static int __init dht11_init(void)
{
	return platform_driver_register(&dht11_driver);
}

static void __exit dht11_exit(void)
{
	platform_driver_unregister(&dht11_driver);	
}

module_init(dht11_init);
module_exit(dht11_exit);
MODULE_LICENSE("GPL");
