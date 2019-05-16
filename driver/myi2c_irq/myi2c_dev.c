
#include "myi2c.h"

/* myi2c资源定义 */
static struct resource myi2c_resource[] =
{
    [0] =
    {
        .start = 0x56000040,                //gpecon寄存器物理地址
        .end = 0x56000040 + 4 - 1,          //gpecon
        .flags = IORESOURCE_MEM,
    },
    [1] =
    {
        .start = 0x54000000,                //i2ccon寄存器物理地址
        .end = 0x54000000 + 20 - 1,         //i2ccon,i2cstat,i2cadd,i2cds,i2clc
        .flags = IORESOURCE_MEM,
    },
    [2] =
    {
        .start = 14,                         //gpe14(scl)
        .end = 14,
        .flags = IORESOURCE_IRQ,
    },
    [3] =
    {
        .start = 15,                         //gpe15(sda)
        .end = 15,
        .flags = IORESOURCE_IRQ,
    },
};

static void myi2c_release(struct device *dev) { }

/* 定义spi设备 */
static struct platform_device myi2c_dev =
{
    .name = "myi2c",
    .id = -1,
    .resource = myi2c_resource,
    .num_resources = ARRAY_SIZE(myi2c_resource),
    .dev =
    {
        .release = myi2c_release,
    },
};

static int __init myi2c_dev_init(void)
{
    platform_device_register(&myi2c_dev);
    return 0;
}
module_init(myi2c_dev_init);

static void __exit myi2c_dev_exit(void)
{
    platform_device_unregister(&myi2c_dev);
}
module_exit(myi2c_dev_exit);
MODULE_LICENSE("GPL");
