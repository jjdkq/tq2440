

#ifndef _RTC_DRV_H
#define _RTC_DRV_H

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
#include <linux/cdev.h>
#include <linux/clk.h>
#include <linux/bcd.h>

#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/io.h>

#define rtccon_offset	0x00			//rtc控制寄存器
#define ticnt_offset	0x04			//rtc滴答中断控制寄存器
#define rtcalm_offset	0x10			//rtc报警使能寄存器
/*rtc报警时间寄存器*/
#define almsec_offset	0x14
#define almmin_offset	0x18
#define almhour_offset	0x1C
#define almdate_offset	0x20
#define almmon_offset	0x24
#define almyear_offset	0x28
/*rtc时间设定寄存器*/
#define bcdsec_offset	0x30
#define bcdmin_offset	0x34
#define bcdhour_offset	0x38
#define bcddate_offset	0x3C
#define bcdday_offset	0x40
#define bcdmon_offset	0x44
#define bcdyear_offset	0x48

struct rtc
{
	 volatile void __iomem  *rtcbase;
	 volatile unsigned char *rtccon;
	 volatile unsigned char *bcdyear;
	 volatile unsigned char *bcdmon;
	 volatile unsigned char *bcddate;
	 volatile unsigned char *bcdday;
	 volatile unsigned char *bcdhour;
	 volatile unsigned char *bcdmin;
	 volatile unsigned char *bcdsec;
};

#endif
