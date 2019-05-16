#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/sched.h> 
#include <linux/irq.h>
#include <asm/uaccess.h>
 
#include <linux/platform_device.h>
#include <asm/io.h>

static int major;
 
static struct class *cls;
static struct device *dev;
 
//static volatile unsigned long *gpio_con;
static volatile unsigned short *gpio_dat;
static int pin;
 
static void __iomem *gpio_base;

static int led_open(struct inode *inode, struct file *file)
{
/*	
	*gpio_base &= ~(0x03 << (pin*2));
	*gpio_base |=  (0x01 << (pin*2));
*/	
	
	writel(readl(gpio_base)&~(0x03<<(pin*2)),gpio_base);
	writel(readl(gpio_base)|(0x01<<(pin*2)),gpio_base);
	
	return 0;
}
 
static ssize_t led_write(struct file *file, const char __user *buf,
	size_t count, loff_t *ppos)
{
	int val;
	copy_from_user(&val, buf, sizeof(val));
 
	if(val == 1)
	{
//		*gpio_dat &= ~(1<<pin);
	writew(readw(gpio_dat)&~(1<<pin),gpio_dat);
	}
	else
	{
//		*gpio_dat |=  (1<<pin);
	writew(readw(gpio_dat)|(1<<pin),gpio_dat);
	}
	return 0;
}
 
static struct file_operations led_fops = 
{
	.owner = THIS_MODULE,
	.open  = led_open,
	.write = led_write,
};
 
static int led_probe(struct platform_device *pdev){
 
	struct resource *res;
	// 最后一个参数 0 表示第1个该类型的资源
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);//获得第1个IO内存资源
	gpio_base= ioremap(res->start, res->end - res->start + 1);
	gpio_dat = gpio_base+ 1;
 
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);//获得第1个IO中断号资源
	pin = res->start;
 
	printk("led_probe, found led\n");
 
	// 注册设备驱动 创建设备节点
	major = register_chrdev(0, "myled", &led_fops);
	// 创建类
	cls = class_create(THIS_MODULE, "myled");
	// 创建设备节点
	dev = device_create(cls, NULL, MKDEV(major, 0), NULL, "led");
 
	return 0;
}
 
static int led_remove(struct platform_device *pdev){
 
	printk("led_remove, remove led\n");	
	// 删除设备节点
	device_unregister(dev);
	// 销毁类
	class_destroy(cls);
	// 取消注册设备驱动
	unregister_chrdev(major, "myled");
	// 取消内存映射
	iounmap(gpio_base);
 
	return 0;
}
 
struct platform_driver led_drv = 
{
	.probe 	= led_probe,	//匹配到dev之后调用probe
	.remove = led_remove,
	.driver = 
	{
		.name = "myled",	//要和platform_device中的name相同
	},
};
 
static int led_drv_init(void){
 
	platform_driver_register(&led_drv);
	return 0;
}
 
static void led_drv_exit(void){
	
	platform_driver_unregister(&led_drv);
}
 
module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");

