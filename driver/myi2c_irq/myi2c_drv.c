
#include "myi2c.h"

#define SLAVE_ADDR 0xee
#define WRDATA 0
#define RDDATA 1

static myi2c_t myi2c =
{
    .irq = IRQ_IIC,
    .irqName = "myi2c",
};

EXPORT_SYMBOL(myi2c);

static struct clk *i2c_clk;

static void myi2c_init(void)
{
    u32 temp;
    temp = readl(&myi2c.gpeRegs->gpecon);
    temp &= ~((3 << (myi2c.scl * 2)) | (3 << (myi2c.sda * 2)));
    temp |= (2 << (myi2c.scl * 2)) | (2 << (myi2c.sda * 2));
    writel(temp, &myi2c.gpeRegs->gpecon);

    temp = 1 << 7 | 0 << 6 | 1 << 5 | 0xf;
    writel(temp, &myi2c.i2cRegs->i2ccon);
    writel(0x10, &myi2c.i2cRegs->i2cadd);
    writel(0x10, &myi2c.i2cRegs->i2cstat);
}

int pt, dataCount;
u8 mode;
static void myi2c_WriteBytes(u8 slaveAddr, u8 *buf, u8 len)
{
    mode = WRDATA;
    pt = 0;
    myi2c.pData = buf;
    dataCount = len;
    writel(slaveAddr, &myi2c.i2cRegs->i2cds);
    writel(0xf0, &myi2c.i2cRegs->i2cstat);
    printk(KERN_INFO "\n");      //这里要有printk()函数，并且printk()要输出回车，原因未知
    while (dataCount != -1);
}

static void myi2c_ReadBytes(u8 slaveAddr, u8 *buf, u8 len)
{
    mode = RDDATA;
    pt = 0;
    myi2c.pData = buf;
    dataCount = len;
    writel(slaveAddr, &myi2c.i2cRegs->i2cds);
    writel(0xb0, &myi2c.i2cRegs->i2cstat);
    printk(KERN_INFO "\n");     //这里要有printk()函数，并且printk()要输出回车，原因未知
    while (dataCount != -1);
}


static irqreturn_t myi2c_irq(int irq, void *dev_id)
{
    u8 i;
    switch (mode)
    {
        case WRDATA:
            if (!(dataCount--))
            {
                writel(0xd0, &myi2c.i2cRegs->i2cstat);
                writel(0xaf, &myi2c.i2cRegs->i2ccon);
                while (readl(&myi2c.i2cRegs->i2cstat) & (1 << 5));
                break;
            }
            writel(myi2c.pData[pt++], &myi2c.i2cRegs->i2cds);
            for (i = 0; i < 10; i++);
            writel(0xaf, &myi2c.i2cRegs->i2ccon);
            break;
        case RDDATA:
            if ((dataCount--) == 0)
            {
                myi2c.pData[pt++] = readl(&myi2c.i2cRegs->i2cds);
                writel(0x90, &myi2c.i2cRegs->i2cstat);
                writel(0xaf, &myi2c.i2cRegs->i2ccon);
                while (readl(&myi2c.i2cRegs->i2cstat) & (1 << 5));
                break;
            }
            myi2c.pData[pt++] = readl(&myi2c.i2cRegs->i2cds);
            if (dataCount == 0)
                writel(0x2f, &myi2c.i2cRegs->i2ccon);
            else
                writel(0xaf, &myi2c.i2cRegs->i2ccon);
            break;
        default:
            break;
    }
    printk(KERN_INFO "\n");         //这里要有printk()函数，并且printk()要输出回车，原因未知
    return IRQ_HANDLED;
}

EXPORT_SYMBOL(myi2c_init);
EXPORT_SYMBOL(myi2c_WriteBytes);
EXPORT_SYMBOL(myi2c_ReadBytes);
EXPORT_SYMBOL(myi2c_irq);


static int myi2c_probe(struct platform_device *pdev)
{    
    struct resource *res;

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);    //获得第一个IO内存资源
    myi2c.gpeRegs = ioremap(res->start, res->end - res->start + 1);

    res = platform_get_resource(pdev, IORESOURCE_MEM, 1);    //获得第三个IO内存资源
    myi2c.i2cRegs = ioremap(res->start, res->end - res->start + 1);

    res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);    //获得第一个IO号
    myi2c.scl = res->start;

    res = platform_get_resource(pdev, IORESOURCE_IRQ, 1);    //获得第二个IO号
    myi2c.sda = res->start;

    i2c_clk = clk_get(NULL, "i2c");
    clk_enable(i2c_clk);

    return 0;   
}
static int myi2c_remove(struct i2c_client *client)
{  
    iounmap(myi2c.i2cRegs);
    iounmap(myi2c.gpeRegs);
    return 0;
}


static struct platform_driver myi2c_drv =
{  
    .driver = 
	{  
        .name  = "myi2c",
        .owner = THIS_MODULE,  
    },  
    .probe     = myi2c_probe,
    .remove    = myi2c_remove,
};

static int __init myi2c_drv_init(void)
{
    return platform_driver_register(&myi2c_drv);
}

static void __exit myi2c_drv_exit(void)
{
    platform_driver_unregister(&myi2c_drv);
}

module_init(myi2c_drv_init);
module_exit(myi2c_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhoubo");
MODULE_DESCRIPTION("myi2c platform driver");
MODULE_VERSION("V1.0");

