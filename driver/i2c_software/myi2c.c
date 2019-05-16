#include "myi2c.h"

EXPORT_SYMBOL(sda_out);
EXPORT_SYMBOL(sda_in);
EXPORT_SYMBOL(scl_out);
EXPORT_SYMBOL(sda_setpin);
EXPORT_SYMBOL(scl_setpin);
EXPORT_SYMBOL(sda_getpin);
EXPORT_SYMBOL(i2c_init);
EXPORT_SYMBOL(i2c_read_one_byte);
EXPORT_SYMBOL(i2c_send_one_byte);
EXPORT_SYMBOL(i2c_wait_ack);
EXPORT_SYMBOL(i2c_ack);
EXPORT_SYMBOL(i2c_nack);
EXPORT_SYMBOL(myi2c_start);
EXPORT_SYMBOL(myi2c_stop);

void sda_out(void)
{
	s3c2410_gpio_cfgpin(S3C2410_GPB8,S3C2410_GPB8_OUTP);	//SDA out
}
void scl_out(void)
{
	s3c2410_gpio_cfgpin(S3C2410_GPB8,S3C2410_GPB6_OUTP);	//SCL out
}
void sda_in(void)
{
	s3c2410_gpio_cfgpin(S3C2410_GPB8,S3C2410_GPB8_INP);		//SDA in
}

void sda_setpin(unsigned x)
{
	s3c2410_gpio_setpin(S3C2410_GPB8,x);	
}
void scl_setpin(unsigned x)
{
	s3c2410_gpio_setpin(S3C2410_GPB6,x);	
}

unsigned sda_getpin(void)
{
	if(s3c2410_gpio_getpin(S3C2410_GPB8))
		return 1;
	return 0;
}

void i2c_init(void)
{
	scl_out();	
	sda_out();
	scl_setpin(1);
	sda_setpin(1);
}

void myi2c_start(void)
{
	sda_out();
	scl_setpin(1);
	sda_setpin(1);
	udelay(4);
	sda_setpin(0);
	udelay(4);
	scl_setpin(0);
}
void myi2c_stop(void)
{
	sda_out();
	scl_setpin(0);
	sda_setpin(0);
	udelay(4);
	sda_setpin(1);
	scl_setpin(1);
	udelay(4);
		
}
unsigned char i2c_wait_ack(void)
{
	unsigned t=0;
	sda_in();      //SDA设置为输入  
	sda_setpin(1);
	udelay(1);	   
	scl_setpin(1);
	udelay(1);	 
	while(sda_getpin())
	{
		t++;
		if(t>250)
		{
			myi2c_stop();
			return 1;
		}
	}
	scl_setpin(0);//时钟输出0 	   
	return 0;  
} 

void i2c_ack(void)
{
	scl_setpin(0);
	sda_out();
	sda_setpin(0);
	udelay(2);
	scl_setpin(1);
	udelay(2);
	scl_setpin(0);
}
void i2c_nack(void)
{
	scl_setpin(0);
	sda_out();
	sda_setpin(1);
	udelay(2);
	scl_setpin(1);
	udelay(2);
	scl_setpin(0);
}
void i2c_send_one_byte(unsigned char data)
{
	unsigned t;
	sda_out();
	scl_setpin(0);
	for(t=0;t<8;t++)
	{
		sda_setpin((data&0x80)>>7);
		data<<=1;
		udelay(2);
		scl_setpin(1);
		udelay(2);
		scl_setpin(0);
		udelay(2);
	}	
}
unsigned char i2c_read_one_byte(unsigned char ack)
{
	unsigned char i,data=0;
	sda_in();
	for(i=0;i<8;i++)
	{
		scl_setpin(0);
		udelay(2);
		scl_setpin(1);
		data<<=1;
		if(sda_getpin())
			data++;
		udelay(1);
	}
	if(!ack)
	{
		i2c_nack();
	}
	else
		i2c_ack();
	return data;
}

