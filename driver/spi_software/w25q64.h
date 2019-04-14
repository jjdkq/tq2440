/*************************************************************************
    > File Name: w25q64.h
    > Author: zhoubo
    > Mail: 1508540876@qq.com
    > Created Time: 2019年03月17日 星期日 22时14分05秒
 ************************************************************************/

#ifndef W25Q64_H_
#define W25Q64_H_
#include "spi.h"

void w25q264_Init(void);
u8 w25q64_ReadSR(void);
void w25q64_WriteSR(u8);
void w25q64_Write_Enable(void);
void w25q64_Write_Disble(void);
void w25q64_Wait_Busy(void);

u16 w25q64_ReadID();
void w25q64_Write_OnePage(u8 *, u32, u16);
void w25q64_Write_Data(u8 *, u32, u16);
void w25q64_Read_Data(u8 *, u32, u16);
void w25q64_Write_NoCheck(u8 *, u32, u16);
void w25q64_Erase_Sector(u32);
void w25q64_Erase_Chip(void);

#define W25Q64_SIZE             (8*1024*1024)

//w25q64指令表
#define w25q64_WriteEnable		0x06
#define w25q64_WriteDisable		0x04
#define w25q64_ReadStatusReg    0x05
#define w25q64_WriteStatusReg   0x01
#define w25q64_ReadData			0x03
#define w25q64_FastReadData		0x0B
#define w25q64_FastReadDual		0x3B
#define w25q64_PageProgram		0x02
#define w25q64_BlockErase       0xD8
#define w25q64_SectorErase		0x20
#define w25q64_ChipErase        0xC7
#define w25q64_PowerDown        0xB9
#define w25q64_ReleasePowerDown	0xAB
#define w25q64_DeviceID			0xAB
#define w25q64_ManufactDeviceID	0x90
#define w25q64_JedecDeviceID    0x9F

#endif
