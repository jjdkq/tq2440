#ifndef __PWM_H
#define __PWM_H

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

#define DEVICE_NAME	"pwm"       /*注册驱动时建立的设备名称*/ 
	
typedef struct
{
	volatile unsigned long TCFG0;
	volatile unsigned long TCFG1;
	volatile unsigned long TCON;
	volatile unsigned long TCNTB0;
	volatile unsigned long TCMPB0;
	volatile unsigned long TCNTO0;
}pwmRegs_t;

typedef struct
{
	volatile unsigned long GPBCON;
	volatile unsigned long GPBDAT;
}gpbRegs_t;

typedef struct
{
	volatile pwmRegs_t *pwmRegs;
	volatile gpbRegs_t *gpbRegs;
	u16 pin;
}pwm_t;

#endif
