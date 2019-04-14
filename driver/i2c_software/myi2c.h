#ifndef __I2C_H
#define __I2C_H
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
   	   		   
//IIC所有操作函数
void i2c_init(void);                //初始化IIC的IO口				 
void myi2c_start(void);				//发送IIC开始信号
void myi2c_stop(void);	  			//发送IIC停止信号
void i2c_send_one_byte(unsigned char);			//IIC发送一个字节
unsigned char i2c_read_one_byte(unsigned char);//IIC读取一个字节
unsigned char i2c_wait_ack(void);			//IIC等待ACK信号
void i2c_ack(void);					//IIC发送ACK信号
void i2c_nack(void);				//IIC不发送ACK信号 
void sda_out(void);
void sda_in(void);
void scl_out(void);
void sda_setpin(unsigned);
void scl_setpin(unsigned);
unsigned sda_getpin(void);
#endif

















