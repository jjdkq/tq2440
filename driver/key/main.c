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
void delay(int i)
{
	unsigned long x=0,y=0;
	for(x=5000;x>0;x--)
		for(y=i;y>0;y--);
}
int main(int argc,char **argv)
{
	int fd=-1,fd1=-1;
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
	//printf("请输入时间间隔(ms):\n");
	//scanf("%d",&t);
	int key=0;
	for(i=0;i<4;i++)
		ioctl(fd,1,i);
	while(1)
	{
#if 1
		key=ioctl(fd1);
		switch (key)
		{
			case 1:
				ioctl(fd,0,0);
			break;
			case 2:
				ioctl(fd,0,1);
			break;
			case 3:
				ioctl(fd,0,2);
			break;
			case 4:
//				ioctl(fd,0,3);
				t=1;
			break;   
		} 
#endif
		if (t==1)
			break;
		/*
		for(i=0;i<3;i++)
		{
			ioctl(fd,0,i);
			delay(t);
			ioctl(fd,1,i);
			delay(t);
		}
		*/
	}
	for(i=0;i<4;i++)
		ioctl(fd,1,i);
	return 0;
}
