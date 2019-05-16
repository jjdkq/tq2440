/*************************************************************************
	> File Name: test.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年12月01日 星期六 21时25分41秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEVICE_NAME		"/dev/adc"

int main(int argc,char *argv[])
{
	int fd=-1;
	int err=0;
	int data=0;
	double vol=0;
	fd=open(DEVICE_NAME,O_RDWR);			//以可读可写方式打开设备文件
	if(fd<0)
	{
		printf(DEVICE_NAME" open error!\n");
		exit(0);
	}
	while(1)
	{
		err=read(fd,&data,sizeof(data));
		if(err<0)
		{
			printf("adc read error!\n");
			exit(0);
		}
		else
		{
			printf("adc=%d\n",data);
			vol=data*3.3/1024;
			printf("AIN2=%fv\n",vol);
		}
		sleep(2);
	}
	return 0;
}
