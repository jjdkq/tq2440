#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>

#include <asm/mach/map.h>
#include <linux/i2c.h>
#include <linux/delay.h>

#include <asm/uaccess.h>
#include "bmp180.h"

#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>


/* 1. 确定主设备号 */
static int major;
static struct cdev bmp180_cdev;
static struct class *cls;

//i2c_client结构体描述了从机器件
static struct i2c_client *bmp180_client;

/* 从bmp180读取多个字节的数据 */
static int bmp180_read_len(struct i2c_client * client, unsigned char reg_add, unsigned char len, unsigned char *buf)
{
    int ret;

    /* 要读取的那个寄存器的地址 */
    char txbuf = reg_add;
  #if 1

	/* i2c_mag结构体描述了主机和从机之间进行传输的数据,并通过i2c_transfer()      * 函数进行传输
	 */
    struct i2c_msg msg[] = 
	{
        {client->addr, 0, 1, &txbuf},       //0表示写,将txbuf指向的数据写到主机中,client->addr表示从机地址
        {client->addr, I2C_M_RD, len, buf}, //读(主机读取从机)len个字节的数据存储到buf中
    };

    /* 通过i2c_transfer函数操作msg */
    ret = i2c_transfer(client->adapter, msg, 2);    //执行2条msg,client->adapter表示i2c适配器
    if (ret < 0)
    {
        printk("i2c_transfer read err\n");
        return -1;
    }
#endif
/*
	int i;
	for (i = 0; i < len; i++)
	{
		buf[i] = i2c_smbus_read_byte_data(client, reg_add);
	}
*/
    return 0;
}

/* bmp180向主机写一个字节的数据 */
static int bmp180_write_byte(struct i2c_client * client, unsigned char reg_addr, unsigned char data)
{
    int ret;

#if 1
    /* 要写的那个寄存器的地址和要写的数据 */
    char txbuf[] = { reg_addr, data };

    struct i2c_msg msg[] = 
	{
        { client->addr, 0, 2, txbuf }	//0表示写,将2个字节的数据(bmp180寄存器的地址和要写的数据)由从机(bmp180)写入到主机中(s3c2440)
    };

	/* 通过i2c_transer()操作msg */
    ret = i2c_transfer(client->adapter, msg, 1);	//执行1条msg
  #endif
  	
  	//ret = i2c_smbus_write_byte_data(client, reg_addr, data);
  	
  	if (ret < 0)
    {
        printk("i2c_transfer write err\n");
        return -1;
    }

    return 0;
}

//存储BMP180数据的结构
_bmp180 bmp180;

//从BMP180获取计算参数
void BMP_ReadCalibrationData(void)
{
	unsigned char rxbuf[2];  
	bmp180_read_len(bmp180_client, 0xaa, 2, rxbuf);
	bmp180.AC1 = ((u16)(rxbuf[0]) << 8 | rxbuf[1]);	//先读到高字节数据
	bmp180_read_len(bmp180_client, 0xac, 2, rxbuf);
	bmp180.AC2 = ((u16)(rxbuf[0]) << 8 | rxbuf[1]);
	bmp180_read_len(bmp180_client, 0xae, 2, rxbuf);
	bmp180.AC3 = ((u16)(rxbuf[0]) << 8 | rxbuf[1]);
	bmp180_read_len(bmp180_client, 0xb0, 2, rxbuf);
	bmp180.AC4 = ((u16)(rxbuf[0]) << 8 | rxbuf[1]);
	bmp180_read_len(bmp180_client, 0xb2, 2, rxbuf);
	bmp180.AC5 = ((u16)(rxbuf[0]) << 8 | rxbuf[1]);
	bmp180_read_len(bmp180_client, 0xb4, 2, rxbuf);
	bmp180.AC6 = ((u16)(rxbuf[0]) << 8 | rxbuf[1]);
	bmp180_read_len(bmp180_client, 0xb6, 2, rxbuf);
	bmp180.B1 = ((u16)(rxbuf[0]) << 8 | rxbuf[1]);
	bmp180_read_len(bmp180_client, 0xb8, 2, rxbuf);
	bmp180.B2 = ((u16)(rxbuf[0]) << 8 | rxbuf[1]);
	bmp180_read_len(bmp180_client, 0xba, 2, rxbuf);
	bmp180.MB = ((u16)(rxbuf[0]) << 8 | rxbuf[1]);
	bmp180_read_len(bmp180_client, 0xbc, 2, rxbuf);
	bmp180.MC = ((u16)(rxbuf[0]) << 8 | rxbuf[1]);
	bmp180_read_len(bmp180_client, 0xbe, 2, rxbuf);
	bmp180.MD = ((u16)(rxbuf[0]) << 8 | rxbuf[1]);
}

//从BMP180读取未修正的温度
long BMP_Read_UT(void)
{
	long temp = 0;
	unsigned char rxbuf[2];	

	bmp180_write_byte(bmp180_client, 0xf4, 0x2e);
	mdelay(5);	
	bmp180_read_len(bmp180_client, 0xf6, 2, rxbuf);
	temp = (long)((u16)rxbuf[0] << 8 | rxbuf[1]);

	return temp;
}

//从BMP180读取未修正的大气压
long BMP_Read_UP(void)
{
	long pressure = 0;
	unsigned char rxbuf[2];	

	bmp180_write_byte(bmp180_client, 0xf4, 0x34);
	mdelay(5);
	
	bmp180_read_len(bmp180_client, 0xf6, 2, rxbuf);
	pressure = (long)((u16)rxbuf[0] << 8 | rxbuf[1]);
	//pressure = pressure + BMP_ReadOneByte(0xf8);
	pressure &= 0x0000FFFF;
	
	return pressure;
}

//用获取的参数对温度和大气压进行修正，并计算海拔
void BMP_UncompemstatedToTrue(void)
{
//	bmp180.UT = BMP_Read_UT();		//第一次读取错误
	bmp180.UT = BMP_Read_UT();		//进行第二次读取修正参数
	bmp180.UP = BMP_Read_UP();
	
	bmp180.X1 = ((bmp180.UT - bmp180.AC6) * bmp180.AC5) >> 15;
	bmp180.X2 = (((long)bmp180.MC) << 11) / (bmp180.X1 + bmp180.MD);
	bmp180.B5 = bmp180.X1 + bmp180.X2;
	bmp180.Temp = ((bmp180.B5 + 8) >> 4);
	
	bmp180.B6 = bmp180.B5 - 4000;
	bmp180.X1 = ((long)bmp180.B2 * (bmp180.B6 * bmp180.B6 >> 12)) >> 11;
	bmp180.X2 = ((long)bmp180.AC2) * bmp180.B6 >> 11;
	bmp180.X3 = bmp180.X1 + bmp180.X2;
	
	bmp180.B3 = ((((long)bmp180.AC1) * 4 + bmp180.X3) + 2) / 4;
	bmp180.X1 = ((long)bmp180.AC3) * bmp180.B6 >> 13;
	bmp180.X2 = (((long)bmp180.B1) * (bmp180.B6*bmp180.B6 >> 12)) >> 16;
	bmp180.X3 = ((bmp180.X1 + bmp180.X2) + 2) >> 2;
	bmp180.B4 = ((long)bmp180.AC4) * (unsigned long)(bmp180.X3 + 32768) >> 15;
	bmp180.B7 = ((unsigned long)bmp180.UP - bmp180.B3) * 50000;
	
	if (bmp180.B7 < 0x80000000)
	{
		bmp180.p = (bmp180.B7 * 2) / bmp180.B4;		
	}
	else
	{
		bmp180.p = (bmp180.B7 / bmp180.B4) * 2;
	}
	
	bmp180.X1 = (bmp180.p >> 8) * (bmp180.p >>8);
	bmp180.X1 = (((long)bmp180.X1) * 3038) >> 16;
	bmp180.X2 = (-7357 * bmp180.p) >> 16;
	
	bmp180.p = bmp180.p + ((bmp180.X1 + bmp180.X2 + 3791) >> 4);
	
//	bmp180.altitude = 44330 * (1-pow(bmp180.p / 101325.0,1.0/5.255)); //在用户空间执行这步(在内核操作浮点数会出错)  
}

static int bmp180_open(struct inode *inode, struct file *file)   
{
	BMP_ReadCalibrationData(); 
    return 0;  
}  

static int bmp180_read(struct file *file,const char __user *buf,
						ssize_t count,loff_t *ppos)
{
	unsigned err = 0;
	BMP_UncompemstatedToTrue();
	int data[3] = {bmp180.Temp / 10, bmp180.Temp % 10, bmp180.p};
	err = copy_to_user(buf, data, sizeof(data));
	if (err)
		printk("copy to user error!\n");
	return err? -EFAULT:0;
		
}

/* 2. 构造file_operations */
static struct file_operations bmp180_fops = 
{
    .owner = THIS_MODULE,
    .open  = bmp180_open,
    .read  = bmp180_read,    
};

//当i2c驱动中设备列表的名称(i2c_driver结构体中的.driver.id_table.name)与i2c设备的名称(bmp180_dev.c中的I2C_BOARD_INFO()的第一个参数)相同时，该函数会被调用;
//当该探测函数被调用时，会传入被探测到的i2c设备的指针(bmp180_dev.c中的bmp180_client)
//static int bmp180_probe(struct i2c_client *client, const struct i2c_device_id *id)  
static int bmp180_probe_new(struct i2c_client *client) 
{
    int res;
    struct device *bmp180_res;
    dev_t devid;

    bmp180_client = client;
    
    printk(KERN_INFO "%s enter\n", __func__);

    /* 3. 告诉内核 */
#if 0
    major = register_chrdev(0, "hello", &hello_fops); /* (major,  0), (major, 1), ..., (major, 255)都对应hello_fops */
#else /*仅仅是注册设备号*/
    if (major) 
	{
        devid = MKDEV(major, 0);
        register_chrdev_region(devid, 1, "bmp180");  /* (major,0) 对应 pwm_fops, (major, 1~255)都不对应pwm_fops */
    } 
	else 
	{
        alloc_chrdev_region(&devid, 0, 1, "bmp180"); /* (major,0) 对应 pwm_fops, (major, 1~255)都不对应pwm_fops */
        major = MAJOR(devid);                     
    }

    cdev_init(&bmp180_cdev, &bmp180_fops);
    res = cdev_add(&bmp180_cdev, devid, 1);
    if (res)
    {
        printk("cdev_add failed\n");
        unregister_chrdev_region(MKDEV(major, 0), 1);
        return 0;
    }
#endif
    cls = class_create(THIS_MODULE, "bmp180");
    bmp180_res = device_create(cls, NULL, MKDEV(major, 0), NULL, "bmp180"); /* /dev/xxx */
    if (IS_ERR(bmp180_res)) 
    {
        printk("device_create failed\n");
        return 0;
    }

    return 0;   
}
static int bmp180_remove(struct i2c_client *client)  
{  
    device_destroy(cls, MKDEV(major, 0));//class_device_destroy(cls,MKDEV(major, 0));

    class_destroy(cls);

    cdev_del(&bmp180_cdev);
    unregister_chrdev_region(MKDEV(major, 0), 1);   

    return 0;  
}

static const struct i2c_device_id bmp180_id[] =
{  
    {"bmp180", 0},  
};

static const struct of_device_id bmp180_ids[] =
{
    { .compatible = "bmp180", },
    { },
};

struct i2c_driver bmp180_driver = 
{  
    .driver = 
	{  
        .name  = "bmp180",  
        .owner = THIS_MODULE,  
        .of_match_table = of_match_ptr(bmp180_ids),
    },  
    //.probe     = bmp180_probe, 
 	.probe_new     = bmp180_probe_new, 
    .remove    = bmp180_remove,  
	//id_table成员中包含了i2c驱动可以匹配的设备名称列表，若i2c设备的名称与
	//该列表中的任一名称匹配，表示i2c驱动与i2c设备匹配
    //.id_table  = bmp180_id,  
};

static int I2C_bmp180_init(void)
{
    return i2c_add_driver(&bmp180_driver);
}

static void I2C_bmp180_exit(void)
{
    i2c_del_driver(&bmp180_driver);
}

module_init(I2C_bmp180_init);
module_exit(I2C_bmp180_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhoubo");
MODULE_DESCRIPTION("A i2c-bmp180 Module for testing module ");
MODULE_VERSION("V1.0");

