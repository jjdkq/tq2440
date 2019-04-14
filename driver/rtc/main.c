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
#include "rtc.h"
void mydelay(int i)
{
	unsigned long x=0,y=0;
	for(x=5000;x>0;x--)
		for(y=i;y>0;y--);
}

int main(int argc,char **argv)
{
	int fd=-1,err=-1;
	time_t_ time;
	/*
 	if(argc != 3)
 	{
  		printf("Usage:%s \n",argv[0]);
  		exit(0);
 	}
	*/
	fd=open("/dev/rtc",O_RDWR);				//打开驱动文件
	
	if(fd<0)
	{
		printf("open error\n");
		exit(-1);
	}
	/*
	year=strtoul(argv[1],0,0)-1;
	if(!strcmp(argv[2],"on"))
		ioctl(fd,0,year);
	else if(!strcmp(argv[2],"off"))
		ioctl(fd,1,year);
	else
		exit(0);
	*/
	int i=0,t=1000;
//	printf("请输入时间间隔(ms):\n");
//	scanf("%d",&t);

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
	}
	*/
		if(ioctl(fd,1,&time))
		{
			perror("read time error!\n");
			close(fd);
			return -1;
		}
		printf("%04x-%02x-%02x  %02x:%02x:%02x\n",time.year,time.mon,time.date,time.hour,time.min,time.sec);
		sleep(1);
	}
	return 0;
}
