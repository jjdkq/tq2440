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
#include <linux/i2c.h>

/* i2c_board_info结构体描述了i2c器件的设备信息 */
static struct i2c_board_info bmp180_info = 
{   
    /* 从机(bmp180)的地址为:0xee >> 1(注意:在内核中i2c从机的读写控制位在从机地址
     * 的最高位,而在裸机或者软件模拟i2c中,读写控制位在最低位);从机名称为:bmp180(这个
     * 名称要与i2c_driver结构体中的名称(.driver.name)一致,否则i2c_driver中的探测
     * 函数(.probe)不能被调用)
	 */
    I2C_BOARD_INFO("bmp180", 0xee >> 1),	//"bmp180"为i2c设备的名称
};

static struct i2c_client *bmp180_client;//定义一个i2c客户端指针

static int I2C_bmp180_init(void)
{
    struct i2c_adapter *i2c_adap;		//定义一个i2c适配器指针

    i2c_adap = i2c_get_adapter(0);		//在编号为0的i2c总线上获得一个i2c适配器
	/* 将i2c适配器和新增的i2c器件(bmp180)关联起来,组成一个客户端 */
    bmp180_client = i2c_new_device(i2c_adap, &bmp180_info);
    i2c_put_adapter(i2c_adap);			//释放适配器

    return 0;
}

static void I2C_bmp180_exit(void)
{
	/* 释放i2c设备 */
    i2c_unregister_device(bmp180_client);
}

module_init(I2C_bmp180_init);
module_exit(I2C_bmp180_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhoubo");
MODULE_DESCRIPTION("A bmp180 Module for testing module ");
MODULE_VERSION("V1.0");

