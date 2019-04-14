#ifndef __LCD_H
#define __LCD_H
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
void LCD_Init(void);
void LCD_PutPixel(unsigned int x,unsigned int y,unsigned short int c);
void LCD_FillColor(unsigned long c);
void LCD_PutASCII(unsigned int x,unsigned int y,unsigned char ch,unsigned int c,unsigned int bk_c,unsigned int st);
void LCD_PaintBmp(int x0,int y0,int h,int l,unsigned char bmp[]);
void LCD_Printf(unsigned int x,unsigned int y,unsigned int c,unsigned int bk_c,unsigned int st,char *fmt,...);
void LCD_PutHz(unsigned int x,unsigned int y,unsigned short int QW,unsigned int c,unsigned int bk_c,unsigned int st);
void LCD_BmpTest( void ) ;

#endif
