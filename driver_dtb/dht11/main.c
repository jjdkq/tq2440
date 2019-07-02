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
	int fd=-1;
	int i=0;
	fd=open("/dev/dht11",0);				//打开驱动文件
	
	if(fd<0)
	{
		printf("dht11 open error\n");
		exit(0);
	}
	
	int data[5];
	while(1)
	{
		i=read(fd,data,sizeof(data));
		if(i<0)
		{
			printf("read error\n");
			continue;
		}
		printf("temp = %dC\n",data[2]);		//打印温度
		printf("humi = %d%%\n",data[0]);	//打印湿度
		sleep(5);							//延时5s
	}
}
