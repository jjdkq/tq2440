/*************************************************************************
    > File Name: bmp180.c
    > Author: zhoubo
    > Mail: 1508540876@qq.com
    > Created Time: 2018年12月18日 星期二 19时57分02秒
 ************************************************************************/

#include "bmp180.h"

#define DEVICE_NAME	"bmp180"
#define SLAVE_ADDR 0xee

static int major = 231;					//主设备号
static int minor = 0;                   //次设备号
module_param(major,int,S_IRUGO);
module_param(minor,int,S_IRUGO);

static dev_t devno;
static struct cdev *cdev;
static struct class *bmp180_class;

//存储BMP180数据的结构
_bmp180 bmp180;

//写一个数据到BMP180
void BMP_WriteOneByte(u8 WriteAddr,u8 DataToWrite)
{
    u8 buf[2] = { WriteAddr, DataToWrite };
    myi2c_WriteBytes(SLAVE_ADDR, buf, 2);
}

//从BMP180读一个16位的数据
short BMP_ReadTwoByte(u8 ReadAddr)
{
    short data;
    u8 msb, lsb;
    u8 buf1[2], buf2[2];

    myi2c_WriteBytes(SLAVE_ADDR, &ReadAddr, 1);
    myi2c_ReadBytes(SLAVE_ADDR, buf1, 1);
    msb = buf1[1];                          //不是buf[0]的原因是第一个中断不接收数据(地址中断)
    u8 temp = ReadAddr + 1;
    myi2c_WriteBytes(SLAVE_ADDR, &temp, 1);
    myi2c_ReadBytes(SLAVE_ADDR, buf2, 1);
    lsb = buf2[1];
    data = msb * 256 + lsb;

    return data;
}

//从BMP180获取计算参数
void BMP_ReadCalibrationData(void)
{
    bmp180.AC1 = BMP_ReadTwoByte(0xAA);
    bmp180.AC2 = BMP_ReadTwoByte(0xAC);
    bmp180.AC3 = BMP_ReadTwoByte(0xAE);
    bmp180.AC4 = BMP_ReadTwoByte(0xB0);
    bmp180.AC5 = BMP_ReadTwoByte(0xB2);
    bmp180.AC6 = BMP_ReadTwoByte(0xB4);
    bmp180.B1  = BMP_ReadTwoByte(0xB6);
    bmp180.B2  = BMP_ReadTwoByte(0xB8);
    bmp180.MB  = BMP_ReadTwoByte(0xBA);
    bmp180.MC  = BMP_ReadTwoByte(0xBC);
    bmp180.MD  = BMP_ReadTwoByte(0xBE);
/*
    printk(KERN_INFO "AC1 = %d AC2 = %d AC3 = %d AC4 = %d\n"
                     "AC5 = %d AC6 = %d B1 = %d B2 = %d\n"
                     "MB = %d MC = %d MD = %d\n", bmp180.AC1,
           bmp180.AC2, bmp180.AC3, bmp180.AC4, bmp180.AC5,
           bmp180.AC6, bmp180.B1, bmp180.B2, bmp180.MB,
           bmp180.MC, bmp180.MD);
*/
}
//从BMP180读取未修正的温度
long BMP_Read_UT(void)
{
    long temp = 0;
    BMP_WriteOneByte(0xF4,0x2E);

    mdelay(5);
    temp = (long)BMP_ReadTwoByte(0xF6);

    return temp;
}
//从BMP180读取未修正的大气压
long BMP_Read_UP(void)
{
    long pressure = 0;

    BMP_WriteOneByte(0xF4,0x34);
    mdelay(5);

    pressure = (long)BMP_ReadTwoByte(0xF6);
    //pressure = pressure + BMP_ReadOneByte(0xf8);
    pressure &= 0x0000FFFF;

    return pressure;
}
//用获取的参数对温度和大气压进行修正，并计算海拔
void BMP_UncompemstatedToTrue(void)
{
//	bmp180.UT = BMP_Read_UT();//第一次读取错误
    bmp180.UT = BMP_Read_UT();//进行第二次读取修正参数
    bmp180.UP = BMP_Read_UP();

    bmp180.X1 = ((bmp180.UT - bmp180.AC6) * bmp180.AC5) >> 15;

    bmp180.X2 = (((long)bmp180.MC) << 11) / (bmp180.X1 + bmp180.MD);
    bmp180.B5 = bmp180.X1 + bmp180.X2;
    bmp180.Temp  = ((bmp180.B5 + 8) >> 4);

    bmp180.B6 = bmp180.B5 - 4000;
    bmp180.X1 = ((long)bmp180.B2 * (bmp180.B6 * bmp180.B6 >> 12)) >> 11;
    bmp180.X2 = ((long)bmp180.AC2) * bmp180.B6 >> 11;
    bmp180.X3 = bmp180.X1 + bmp180.X2;

    bmp180.B3 = ((((long)bmp180.AC1) * 4 + bmp180.X3) + 2) /4;
    bmp180.X1 = ((long)bmp180.AC3) * bmp180.B6 >> 13;
    bmp180.X2 = (((long)bmp180.B1) *(bmp180.B6*bmp180.B6 >> 12)) >>16;
    bmp180.X3 = ((bmp180.X1 + bmp180.X2) + 2) >> 2;
    bmp180.B4 = ((long)bmp180.AC4) * (unsigned long)(bmp180.X3 + 32768) >> 15;
    bmp180.B7 = ((unsigned long)bmp180.UP - bmp180.B3) * 50000;

    if(bmp180.B7 < 0x80000000)
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

    //这步在用户空间进行(在内核操作浮点数会出错)
//	bmp180.altitude = 44330 * (1-pow(bmp180.p / 101325.0,1.0/5.255));
}

static int bmp180_open(struct inode *ionde,struct file *file)
{
    u8 err = request_irq(myi2c.irq, myi2c_irq, IRQF_DISABLED, myi2c.irqName, 0);
    if (err)
    {
        printk(KERN_ERR "request irq err!\n");
        free_irq(myi2c.irq, 0);
    }
    myi2c_init();
    BMP_ReadCalibrationData();
    return 0;
}
static int bmp180_read(struct file *file,const char __user *buf,
                        ssize_t count,loff_t *ppos)
{
    unsigned err=0;
    BMP_UncompemstatedToTrue();
    int data[3]={bmp180.Temp/10,bmp180.Temp%10,bmp180.p};
    err=copy_to_user(buf,data,sizeof(data));
    if(err)
        printk("copy to user error!\n");
    return err? -EFAULT:0;

}

//卸载中断处理函数(不能省略!!!)
static int myi2c_close(struct inode *inode, struct file *file)
{
    free_irq(myi2c.irq, 0);
    return 0;
}

static struct file_operations bmp180_fops=
{
    .owner=THIS_MODULE,
    .open=bmp180_open,
    .read=bmp180_read,
    .release = myi2c_close,
};

static int __init bmp180_init(void)
{
    int ret;
    if(major)					//静态添加设备号
    {
        devno=MKDEV(major,minor);
        ret=register_chrdev_region(devno,minor,DEVICE_NAME);
    }
    else						//动态添加设备号
    {
        ret=alloc_chrdev_region(&devno,minor,1,DEVICE_NAME);
        major=MAJOR(devno);
    }
    if(ret<0)
    {
        printk(KERN_ERR"Can't add the devno!\n");
        return -1;
    }
    cdev=cdev_alloc();
    if(cdev)
    {
        cdev_init(cdev,&bmp180_fops);
        if(cdev_add(cdev,devno,1))
        {
            printk(KERN_ERR"Can't add the bmp180 device!\n");
            goto err;
        }
    }
    bmp180_class=class_create(THIS_MODULE,"bmp180_class");
    if(IS_ERR(bmp180_class))
    {
        printk(KERN_ERR"Can't add the bmp180 class!\n");
        return -1;
    }
    device_create(bmp180_class,NULL,devno,NULL,DEVICE_NAME);
    printk(KERN_INFO"The bmp180 device created conpletely!\n");
    return 0;
err:
    unregister_chrdev_region(devno,1);
    return -1;
}

static void __exit bmp180_exit(void)
{
    device_destroy(bmp180_class,devno);
    class_destroy(bmp180_class);
    cdev_del(cdev);
    unregister_chrdev_region(devno,1);
    printk(KERN_INFO"bmp180 device unregisteres completely!\n");
}

module_init(bmp180_init);
module_exit(bmp180_exit);
MODULE_LICENSE("GPL");

