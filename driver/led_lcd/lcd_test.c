#include "lcd.h"
#include "pic.h"
typedef unsigned int U32;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long int screensize ;
char *fbp;
long int location ;
void LCD_Init(void)
{
	int fbfd = -1;
    fbfd = open("/dev/fb0", O_RDWR);                   				 // 打开Frame Buffer设备
    if (fbfd < 0) 
    {
    	printf("Error: cannot open framebuffer device.%x\n",fbfd);	
        exit(1);
    }
    
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))					//获取设备固件信息
    {           
        printf("Error reading fixed information.\n");
        exit(2);
    }
    
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))					 // 获取设备可变信息
    {           
        printf("Error reading variable information.\n");
        exit(3);
    }
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;//计算屏幕像素大小(显存大小)
    //通过mmap系统调用将framebuffer内存映射到用户空间,并返回映射后的起始地址
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
         				fbfd, 0);
    if ((int)fbp == -1) 
    {
    	printf("Error: failed to map framebuffer device to memory.\n");
   	    exit(4);
    }
	vinfo.xoffset=0;
	vinfo.yoffset=0;
}
//LCD输出单个像素
void LCD_PutPixel(unsigned int x,unsigned int y,unsigned short int c)
{
	if(x<480&&y<272)
	{
		location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +   // 定位到具体哪一行的第几个像素  
        		   (y+vinfo.yoffset) * finfo.line_length; 			//定位到哪一行(即该行的第一个像素的地址) 这两句即是实现求某一个像素的地址的功能
		*((unsigned int *)(fbp+location))=c;
	}	
}
//LCD填充颜色
void LCD_FillColor(unsigned long c)
{
	int x=0,y=0;
	for ( y = 0; y < 272; y++ )               	// 行扫描
	{
    	for ( x = 0; x < 480; x++ )				// 列扫描 
        {           							
        	location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +   // 定位到具体哪一行的第几个像素  
            		   (y+vinfo.yoffset) * finfo.line_length; 			//定位到哪一行(即该行的第一个像素的地址) 这两句即是实现求某一个像素的地址的功能
             *((unsigned short int*)(fbp+location)) = c;
        }
	}
}
extern unsigned char __VGA[];//为字库添加的声明，英文ASCII编码
extern unsigned char __CHS[];//为字库添加的声明，汉子字库
//LCD在指定坐标输出ASCII码
void LCD_PutASCII(unsigned int x,unsigned int y,unsigned char ch,unsigned int c,unsigned int bk_c,unsigned int st)
{
       unsigned short int i,j;
       unsigned char *pZK,mask,buf;
	
       pZK = &__VGA[ch*16];
       for( i = 0 ; i < 16 ; i++ )
       {
              mask = 0x80;
              buf = pZK[i];
              for( j = 0 ; j < 8 ; j++ )
              {
                     if( buf & mask )
                     {
                            LCD_PutPixel(x+j,y+i,c);
                     }
                     else
                     {
                            if( !st )
                            {
                                   LCD_PutPixel(x+j,y+i,bk_c);
                            }
                     }
            
                     mask = mask >> 1;
              }
       }
}

void LCD_PaintBmp(int x0,int y0,int h,int l,unsigned char bmp[])
{
	int x,y;
	U32 c;
	int p = 0;
	
    for( y = y0 ; y < l ; y++ )
    {
    	for( x = x0 ; x < h ; x++ )
    	{
    		c = bmp[p+1] | (bmp[p]<<8) ;
			if ( ( (x0+x) < 480) && ( (y0+y) < 272) )
			{
       			location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +   // 定位到具体哪一行的第几个像素  
           		   (y+vinfo.yoffset) * finfo.line_length; 	//定位到哪一行(即该行的第一个像素的地址) 这两句即是实现求某一个像素的地址的功能
            	*((unsigned short int*)(fbp+location)) = c;
//				LCD_PutPixel(x,y,c);
			}
    		p = p + 2 ;
    	}
    }
}

void LCD_PutHz(unsigned int x,unsigned int y,unsigned short int QW,unsigned int c,unsigned int bk_c,unsigned int st)
{
	unsigned short int i,j;
    unsigned char *pZK,mask,buf;
    pZK = &__CHS[ (  ( (QW >> 8) - 1 )*94 + (QW & 0x00FF)- 1 )*32 ];
    for( i = 0 ; i < 16 ; i++ )
    {
     	//左
        mask = 0x80;
        buf = pZK[i*2];
        for( j = 0 ; j < 8 ; j++ )
        {
        	if( buf & mask )
            {
    	        LCD_PutPixel(x+j,y+i,c);
            }
            else
            {
            	if( !st )
                {
                	LCD_PutPixel(x+j,y+i,bk_c);
                }
            }
          	mask = mask >> 1;
         } 
        
         //右
         mask = 0x80;
         buf = pZK[i*2+1 ];
       	 for( j = 0 ; j < 8 ; j++ )
         {
         	if( buf & mask )
            {
            	LCD_PutPixel(x+j+8 ,y+i,c);
            }
            else
            {
            	if( !st )
                {
                	LCD_PutPixel(x+j+8 ,y+i,bk_c);
                }
            }
            mask = mask >>1;
         }                 
	}
}

void LCD_Printf(unsigned int x,unsigned int y,unsigned int c,unsigned int bk_c,unsigned int st,char *fmt,...)
{
	char __LCD_Printf_Buf[256];
    va_list ap;
    unsigned char *pStr = (unsigned char *)__LCD_Printf_Buf;
    unsigned int i = 0;

    va_start(ap,fmt);
    vsprintf(__LCD_Printf_Buf,fmt,ap);
    va_end(ap);
 
    while(*pStr != 0 )
    {
    	switch(*pStr)
        {
        	case '\n' :
             	break;
            default:
            	if( *pStr > 0xA0 & *(pStr+1) > 0xA0 )  //中文输出
                {
                	LCD_PutHz( x , y , (*pStr - 0xA0)*0x0100 + *(pStr+1) - 0xA0 , c , bk_c , st);
					pStr++;
                    i++;
  					 x += 16;
                }
                else               					//英文输出
              	{
                	LCD_PutASCII( x , y , *pStr , c , bk_c , st );
                    x += 8;

                }
				break;
		}
        pStr++;
        i++;		
        if( i > 256 ) 
			break;
	} 
}




