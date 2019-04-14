/*************************************************************************
    > File Name: spi.h
    > Author: zhoubo
    > Mail: 1508540876@qq.com
    > Created Time: 2019年03月17日 星期日 16时45分54秒
 ************************************************************************/

#ifndef SPI_H_
#define SPI_H_

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/cdev.h>

/* spi操作函数 */
void Spi_Init(void);		//spi初始化
void Spi_WriteOneByte(u8);	//spi写一字节数据
u8 Spi_ReadOneByte(void);	//spi读一字节数据

void Spi_Cs_OutMode(void);	//片选信号输出模式
void Spi_Cs_SetPin(u8);
void Spi_Ck_OutMode(void);	//时钟信号输出模式
void Spi_Ck_SetPin(u8);
void Spi_Si_OutMode(void);	//从机输入(主机输出)模式
void Spi_Si_SetPin(u8);
void Spi_So_InMode(void);	//从机输出(主机输入)模式
u8 Spi_So_GetPin(void);

/* spi相关io寄存器定义 */
typedef struct
{
    unsigned long gpbcon;
    unsigned long gpbdat;
}spi_regs_t;

#endif
