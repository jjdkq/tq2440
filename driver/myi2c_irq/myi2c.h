#ifndef MYI2C_H
#define MYI2C_H

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
#include <linux/clk.h>
#include <linux/workqueue.h>

typedef struct
{
    u32 gpecon;
}gpeRegs_t;

typedef struct
{
    u32 i2ccon;
    u32 i2cstat;
    u32 i2cadd;
    u32 i2cds;
    u32 i2clc;
}i2cRegs_t;

typedef struct
{
    volatile gpeRegs_t *gpeRegs;
    volatile i2cRegs_t *i2cRegs;
    u8 sda;
    u8 scl;
    u32 irq;                //中断号
    char *irqName;          //中断名称
    u8 *pData;

}myi2c_t;

#endif // MYI2C_H

