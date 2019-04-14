/*************************************************************************
	> File Name: main.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年11月29日 星期四 16时02分51秒
 ************************************************************************/

#include "lcd.h"
#include "pic.h"

void delay_ms(unsigned long t)
{
	usleep(t*1000);
}
void LCD_BmpTest( void ) 
{
	//图8
	LCD_FillColor( (0x00<<11) | (0x00<<5) | (0x00)  )  ;		//clear screen
	delay_ms(100);
	LCD_PaintBmp(0, 0, 480, 272, gImage_8);	
	delay_ms(5000);
	
	//图10
	LCD_FillColor( (0x1f<<11) | (0x3f<<5) | (0x1f)  )  ;		//clear screen
	delay_ms(100);
	LCD_PaintBmp(0, 0, 480, 272, gImage_10);	
	delay_ms(5000);

	//图9
	LCD_FillColor( (0x00<<11) | (0x00<<5) | (0x1f)  )  ;		//clear screen
	delay_ms(100);
	LCD_PaintBmp(0, 0, 480, 272, gImage_9);		
	delay_ms(5000);
	
	//图1
	LCD_FillColor( (0x00<<11) | (0x00<<5) | (0x1f)  )  ;		//clear screen
	delay_ms(100);
	LCD_PaintBmp(0, 0, 480, 272, gImage_1);		
	delay_ms(5000);
	
	//图2
	LCD_FillColor( (0x00<<11) | (0x00<<5) | (0x1f)  )  ;		//clear screen
	delay_ms(100);
	LCD_PaintBmp(0, 0, 480, 272, gImage_2);		
	delay_ms(5000);
	
	//图5
	LCD_FillColor( (0x00<<11) | (0x00<<5) | (0x1f)  )  ;		//clear screen
	delay_ms(100);
	LCD_PaintBmp(0, 0, 480, 272, gImage_5);		
	delay_ms(5000);
	
	//图4
	LCD_FillColor( (0x00<<11) | (0x00<<5) | (0x1f)  )  ;		//clear screen
	delay_ms(100);
	LCD_FillColor( (0x00<<11) | (0x3f<<5) | (0x00)  )  ;		//clear screen
	LCD_PaintBmp(0, 0, 480, 272, gImage_4);	
	delay_ms(5000);
	
	LCD_FillColor( (0x00<<11) | (0x00<<5) | (0x1f)  )  ;		//clear screen
	delay_ms(100);	
	LCD_PaintBmp(0, 0, 480, 272, gImage_5);
	delay_ms(5000);		

	//图6
	LCD_FillColor(0xffff);		//fill all screen with some color  
	LCD_PaintBmp(0, 0, 480, 272, gImage_6);
	delay_ms(5000);	
}

void lcd_thread(void)
{
	LCD_Init();
//	LCD_FillColor((0x3f<<5)|(0x1f));
	LCD_FillColor((0x1f<<11) | (0x3f<<5) | (0x1f) );
	delay_ms(100);
	LCD_PutASCII(50,100,'K',0x0000,(0x1f<<11) | (0x3f<<5) | (0),1);
	//LCD_FillColor( (0x00<<11) | (0x00<<5) | (0x00)  )  ;		//clear screen
//	LCD_Printf(50,100, 0x0000,(0x1f<<11) | (0x3f<<5) | (0),1,"笨蛋才思考，聪明的人靠灵感");
//	LCD_Printf(50,100, 0x0000,(0x1f<<11) | (0x3f<<5) | (0),1,"hello");

	while(1)
	{
		printf("Lcd thread is running!\n");
		sleep(1);
		LCD_BmpTest();
	}
}
