#ifndef _BMP180_H_
#define _BMP180_H_
#include "myi2c.h"

extern void myi2c_init(void);
extern void myi2c_ReadBytes(u8, u8 *, u8);
extern void myi2c_WriteBytes(u8, u8 *, u8);
extern myi2c_t myi2c;
extern irqreturn_t myi2c_irq(int, void *);

typedef struct __BMP180
{
	short AC1;
	short AC2;
	short AC3;
	unsigned short AC4;
	unsigned short AC5;
	unsigned short AC6;
	short B1;
	short B2;
	short MB;
	short MC;
	short MD;
	long UT;
	long UP;
	long X1;
	long X2;
	long X3;
	long B3;
	unsigned long B4;
	long B5;
	long B6;
	long B7;
	long p;
	long Temp;
	float altitude;
}_bmp180;

extern _bmp180 bmp180;

//typedef unsigned char uint8_t;
void BMP_ReadCalibrationData(void);
long BMP_Read_UT(void);
long BMP_Read_UP(void);
void BMP_UncompemstatedToTrue(void);

#endif




