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
#include <poll.h>

void delay(int i)
{
	unsigned long x=0,y=0;
	for(x=5000;x>0;x--)
		for(y=i;y>0;y--);
}
int main(int argc,char **argv)
{
	int fd=-1,fd1=-1;
	int i=0;
	int ret;	
	
	/*
 	if(argc != 3)
 	{
  		printf("Usage:%s \n",argv[0]);
  		exit(0);
 	}
	*/
	fd=open("/dev/led",0);				//打开驱动文件
	
	if(fd<0)
	{
		printf("led open error\n");
		exit(0);
	}
	fd1=open("/dev/key",0);				//打开驱动文件
	
	if(fd1<0)
	{
		printf("key open error\n");
		exit(0);
	}
#define LED_ALL_OFF {for(i=0;i<4;i++) ioctl(fd,1,i);}
	LED_ALL_OFF;
	int press_cnt[4];
	struct pollfd fds[] = 
	{
		{
			.fd = fd1,
			.events = POLLIN,
		},
	};

	while(1)
	{
		/* poll()返回0时表示5s时间到了，而这段时间里，没有发生“数据可读”
		 * 的事件
		 */ 
		ret = poll(fds, sizeof(fds) / sizeof(fds[0]), 5000);
		if (!ret)
			printf("time out!\n");
		else
		{
			i=read(fd1,press_cnt,sizeof(press_cnt));
			if(i<0)
			{
				printf("read error\n");
				continue;
			}
			printf("The process is running...\n");

			if(press_cnt[0])
			{
				for(i=0;i<4;i=i+2)
				ioctl(fd,0,i);
				delay(10000);
			}
			if(press_cnt[1])
			{
				for(i=1;i<3;i++)
				ioctl(fd,0,i);
				delay(10000);
			}
			if(press_cnt[2])
			{
				for(i=0;i<4;i++)
				ioctl(fd,0,i);
				delay(10000);
			}
			if(press_cnt[3])
			{
				for(i=3;i<4;i++)
				ioctl(fd,0,i);
				delay(10000);
			}

			LED_ALL_OFF;
		}
	/*	
		for(i=0;i<4;i++)
		{
			if(press_cnt[i])
			printf("K%d has been pressed %d times\n",i+1,press_cnt[i]);
		}
	*/
	}
	return 0;
}
