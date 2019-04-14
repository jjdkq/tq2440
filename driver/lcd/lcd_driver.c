#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/mach/map.h>
#include <mach/regs-lcd.h>
#include <mach/regs-gpio.h>
#include <mach/fb.h>

/*
static int s3c_lcdfb_setcolreg(unsigned int regno, unsigned int red,
        unsigned int green, unsigned int blue,
        unsigned int transp, struct fb_info *info);

*/

struct lcd_regs 
{
 	unsigned long lcdcon1;
 	unsigned long lcdcon2;
 	unsigned long lcdcon3;
 	unsigned long lcdcon4;
 	unsigned long lcdcon5;
    unsigned long lcdsaddr1;
    unsigned long lcdsaddr2;
    unsigned long lcdsaddr3;
    unsigned long redlut;
    unsigned long greenlut;
    unsigned long bluelut;
    unsigned long reserved[9];
    unsigned long dithmode;
    unsigned long tpal;
    unsigned long lcdintpnd;
    unsigned long lcdsrcpnd;
    unsigned long lcdintmsk;
    unsigned long lpcsel;
};

static struct fb_ops s3c_lcdfb_ops= 
{
	.owner  = THIS_MODULE,
 	//.fb_setcolreg = s3c_lcdfb_setcolreg,//设置调色板，见注释3
 	.fb_fillrect = cfb_fillrect,
 	.fb_copyarea = cfb_copyarea,
 	.fb_imageblit = cfb_imageblit,
};

static struct fb_info *s3c_lcd;
static volatile unsigned long *gpbcon;
static volatile unsigned long *gpbdat;
static volatile unsigned long *gpccon;
static volatile unsigned long *gpdcon;
static volatile unsigned long *gpgcon;
static volatile struct lcd_regs *lcd_regs;
static u32 pseudo_palette[16];

#if 0
/* from pxafb.c */
static inline unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
 	return chan << bf->offset;
}

static int s3c_lcdfb_setcolreg(unsigned int regno, unsigned int red,
        unsigned int green, unsigned int blue,
        unsigned int transp, struct fb_info *info)
{
 	unsigned int val;
 
 	if (regno > 16)
  	return 1;

 	/* 用red,green,blue三原色构造出val，val最终是16位象素*/
 	val  = chan_to_field(red, &info->var.red);
 	val |= chan_to_field(green, &info->var.green);
 	val |= chan_to_field(blue, &info->var.blue);
 
 	pseudo_palette[regno] = val;
	return 0;
}
#endif

static int lcd_init(void)
{
 	/* 1. 分配一个fb_info */
 	s3c_lcd = framebuffer_alloc(0, NULL);

 	/* 2. 设置 */
 	/* 2.1 设置固定的参数 */
 	strcpy(s3c_lcd->fix.id, "mylcd"); 
 	s3c_lcd->fix.smem_len = 480*272*16/8;			 //显存的长度=分辨率*每象素字节数
 	s3c_lcd->fix.type     = FB_TYPE_PACKED_PIXELS;	/* Packed Pixels */
 	s3c_lcd->fix.visual   = FB_VISUAL_TRUECOLOR; 	/* TFT为真彩色，所以要设置成这个 */
 	s3c_lcd->fix.line_length = 480*2;				//每行的长度，以字节为单位
 
 	/* 2.2 设置可变的参数 */
 	s3c_lcd->var.xres           = 480;//x方向分辨率
 	s3c_lcd->var.yres           = 272;//y方向分辨率
 	s3c_lcd->var.xres_virtual   = 480;//x方向虚拟分辨率，参考注释1
 	s3c_lcd->var.yres_virtual   = 272;//y方向虚拟分辨率
 	s3c_lcd->var.bits_per_pixel = 16;//每个象素使用多少位

 	/* RGB:565 */
	s3c_lcd->var.red.offset     = 11;//红色偏移值为11
	s3c_lcd->var.red.length     = 5;//红色位长为5
	 
	s3c_lcd->var.green.offset   = 5;//绿色偏移值为5
	s3c_lcd->var.green.length   = 6;//绿色位长为6

	s3c_lcd->var.blue.offset    = 0;//蓝色偏移值为0
	s3c_lcd->var.blue.length    = 5;//蓝色位长为5

	s3c_lcd->var.activate       = FB_ACTIVATE_NOW;//使设置的值立即生效
 
 
	/* 2.3 设置操作函数 */
	s3c_lcd->fbops              = &s3c_lcdfb_ops;
	 
	/* 2.4 其他的设置 */
	s3c_lcd->pseudo_palette = pseudo_palette;//存放调色板所调颜色的数组
	//s3c_lcd->screen_base  = ;  /* 显存的虚拟地址，这个在后面设置的 */ 
	s3c_lcd->screen_size   = 480*272*16/8;//显存的大小

	/* 3. 硬件相关的操作 */
	/* 3.1 配置GPIO用于LCD */
	gpbcon = ioremap(0x56000010, 8);	//将寄存器的物理地址映射为虚拟地址		
	gpbdat = gpbcon+1;
	gpccon = ioremap(0x56000020, 4);
	gpdcon = ioremap(0x56000030, 4);
	gpgcon = ioremap(0x56000060, 4);

    *gpccon  = 0xaaaa02a9;   /* GPIO管脚用于VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND */
 	*gpdcon  = 0xaaaaaaaa;   /* GPIO管脚用于VD[23:8] */
//或者
//	writel(0xaaaaaaaa,gpdcon);
 
	//*gpbcon &= ~(3);  /* GPB0设置为输出引脚 */
 	//*gpbcon |= 1;
 	//*gpbdat &= ~1;     /* 输出低电平，关闭背光*/

 	*gpgcon |= (3<<8); /* GPG4用作LCD_PWREN */
 
 	/* 3.2 根据LCD手册设置LCD控制器, 比如VCLK的频率等 */
 	lcd_regs = ioremap(0x4D000000, sizeof(struct lcd_regs));//映射各lcd控制寄存器，这种映射方法要学会

 	/* bit[17:8]: VCLK = HCLK / [(CLKVAL+1) x 2], 
 	 *            10MHz(100ns) = 100MHz / [(CLKVAL+1) x 2]
 	 *            CLKVAL = 4
 	 * bit[6:5]: 0b11, TFT LCD
 	 * bit[4:1]: 0b1100, 16 bpp for TFT，表示每个象素用多少位来表示
 	 * bit[0]  : 0 = Disable the video output and the LCD control signal.
 	 */
 	lcd_regs->lcdcon1  = (4<<8) | (0<<7) | (3<<5) | (12<<1) | 0;
//或者
//	writel((4<<8) | (0<<7) | (3<<5) | (12<<1) | 0,&(lcd_regs->lcdcon1)  );
#if 1
 	/* 垂直方向的时间参数
	 * bit[31:24]: VBPD, VSYNC之后再过多长时间才能发出第1行数据
	 *             LCD手册 T0-T2-T1=4
	 *             VBPD=3
	 * bit[23:14]: 多少行, 320, 所以LINEVAL=320-1=319
	 * bit[13:6] : VFPD, 发出最后一行数据之后，再过多长时间才发出VSYNC
	 *             LCD手册T2-T5=322-320=2, 所以VFPD=2-1=1
	 * bit[5:0]  : VSPW, VSYNC信号的脉冲宽度, LCD手册T1=1, 所以VSPW=1-1=0
  	 */
 	lcd_regs->lcdcon2  = (2<<24) | (271<<14) | (4<<6) | (8<<0);

 	/* 水平方向的时间参数
  	* bit[25:19]: HBPD, VSYNC之后再过多长时间才能发出第1行数据
  	*             LCD手册 T6-T7-T8=17
  	*             HBPD=16
 	* bit[18:8]: 多少列, 240, 所以HOZVAL=240-1=239
 	* bit[7:0] : HFPD, 发出最后一行里最后一个象素数据之后，再过多长时间才发出HSYNC
 	*             LCD手册T8-T11=251-240=11, 所以HFPD=11-1=10
 	*/
 	lcd_regs->lcdcon3 = (10<<19) | (479<<8) | (19<<0);

 	/* 水平方向的同步信号
  	* bit[7:0] : HSPW, HSYNC信号的脉冲宽度, LCD手册T7=5, 所以HSPW=5-1=4
  	*/ 
 	lcd_regs->lcdcon4 = 13<<8 | 30;

#else
lcd_regs->lcdcon2 = S3C2410_LCDCON2_VBPD(5) | 
  S3C2410_LCDCON2_LINEVAL(319) | 
  S3C2410_LCDCON2_VFPD(3) | 
  S3C2410_LCDCON2_VSPW(1);

lcd_regs->lcdcon3 = S3C2410_LCDCON3_HBPD(10) | 
  S3C2410_LCDCON3_HOZVAL(239) | 
  S3C2410_LCDCON3_HFPD(1);

lcd_regs->lcdcon4 = S3C2410_LCDCON4_MVAL(13) | 
  S3C2410_LCDCON4_HSPW(0);

#endif
 	/* 信号的极性 
  	* bit[11]: 1=565 format
  	* bit[10]: 0 = The video data is fetched at VCLK falling edge
  	* bit[9] : 1 = HSYNC信号要反转,即低电平有效 
 	 * bit[8] : 1 = VSYNC信号要反转,即低电平有效 
 	 * bit[6] : 0 = VDEN不用反转
  	* bit[3] : 0 = PWREN输出0
  	* bit[1] : 0 = BSWP
  	* bit[0] : 1 = HWSWP 2440手册P413
  	*/
	lcd_regs->lcdcon5 = (1<<11) | (0<<10) | (1<<9) | (1<<8) | (0<<7) |(0<<6)|(1<<3)|(0<<1)|1;
 
 	/* 3.3 分配显存(framebuffer), 并把地址告诉LCD控制器 */
        /* s3c_lcd->screen_base：显存基址，是虚拟地址
         *s3c_lcd->fix.smem_len：显存长度
         *s3c_lcd->fix.smem_start：显存起始地址，是物理地址
       	 */
 	s3c_lcd->screen_base = dma_alloc_writecombine(NULL, s3c_lcd->fix.smem_len, &s3c_lcd->fix.smem_start, GFP_KERNEL);
 
 	lcd_regs->lcdsaddr1  = (s3c_lcd->fix.smem_start >> 1) & ~(3<<30);//存放起始地址
 	lcd_regs->lcdsaddr2  = ((s3c_lcd->fix.smem_start + s3c_lcd->fix.smem_len) >> 1) & 0x1fffff;//存放结束地址
 	lcd_regs->lcdsaddr3  = (480*16/16);  /* 一行的长度(单位: 2字节) */ 
 
 	/* 启动LCD */
 	lcd_regs->lcdcon1 |= (1<<0); /* 使能LCD控制器 */
 	lcd_regs->lcdcon5 |= (1<<3); /* 使能LCD本身 */
 	//*gpbdat |= 1;     /* 输出高电平, 使能背光 */  

 	/* 4. 注册 */
 	register_framebuffer(s3c_lcd);
 
 	return 0;
}

static void lcd_exit(void)
{
 	unregister_framebuffer(s3c_lcd);
 	lcd_regs->lcdcon1 &= ~(1<<0); /* 关闭LCD本身 */
// *gpbdat &= ~1;     /* 关闭背光 */
 	dma_free_writecombine(NULL, s3c_lcd->fix.smem_len, s3c_lcd->screen_base, s3c_lcd->fix.smem_start);
 	iounmap(lcd_regs);			//取消地址映射(释放之前映射所占用的虚拟地址)
 	iounmap(gpbcon);
 	iounmap(gpccon);
 	iounmap(gpdcon);
 	iounmap(gpgcon);
 	framebuffer_release(s3c_lcd);
}

module_init(lcd_init);
module_exit(lcd_exit); 
MODULE_LICENSE("GPL");
