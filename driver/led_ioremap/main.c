/*************************************************************************
	> File Name: main.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年11月21日 星期三 16时14分28秒
 ************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
void mydelay(int i)
{
	unsigned long x=0,y=0;
	for(x=5000;x>0;x--)
		for(y=i;y>0;y--);
}

typedef enum
{
	LED1_ON=0,
	LED1_OFF,
	LED2_ON,
	LED2_OFF,
	LED3_ON,
	LED3_OFF,
	LED4_ON,
	LED4_OFF
}led_sta;

int main(int argc,char **argv)
{
	int fd=-1;
	int led_no=0;
	/*
 	if(argc != 3)
 	{
  		printf("Usage:%s \n",argv[0]);
  		exit(0);
 	}
	*/
	fd=open("/dev/led",O_RDWR);				//打开驱动文件
	
	if(fd<0)
	{
		printf("open error\n");
		exit(0);
	}
	/*
	led_no=strtoul(argv[1],0,0)-1;
	if(!strcmp(argv[2],"on"))
		ioctl(fd,0,led_no);
	else if(!strcmp(argv[2],"off"))
		ioctl(fd,1,led_no);
	else
		exit(0);
	*/
	int i=0,t=1000;
//	printf("请输入时间间隔(ms):\n");
//	scanf("%d",&t);
	led_sta val;
	int err=0;
	//LED全熄灭
	val=LED1_OFF;	
	err=write(fd,&val,sizeof(val));
	if(err<0)
	{
		printf("Led device can't write!\n");
		exit(0);
	}
	val=LED2_OFF;	
	write(fd,&val,sizeof(val));
	val=LED3_OFF;	
	write(fd,&val,sizeof(val));
	val=LED4_OFF;	
	write(fd,&val,sizeof(val));
	while(1)
	{
		/*
		for(i=0;i<4;i++)
		{
			ioctl(fd,0,i);
//			mydelay(t);
			sleep(2);			//延时2s

			ioctl(fd,1,i);
//			mydelay(t);
			sleep(2);
		}
		*/
		ioctl(fd, 0, 1);
		val=LED1_ON;
		write(fd,&val,sizeof(val));
		val=LED3_ON;
		write(fd,&val,sizeof(val));
		sleep(2);
		val=LED2_OFF;
		write(fd,&val,sizeof(val));
		val=LED4_OFF;
		write(fd,&val,sizeof(val));
		sleep(2);
		val=LED1_OFF;
		write(fd,&val,sizeof(val));
		val=LED3_OFF;
		write(fd,&val,sizeof(val));
		sleep(2);
		val=LED2_ON;
		write(fd,&val,sizeof(val));
		val=LED4_ON;
		write(fd,&val,sizeof(val));
		sleep(2);
	}
	return 0;
}
