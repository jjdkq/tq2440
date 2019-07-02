/*************************************************************************
	> File Name: test.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年12月19日 星期三 10时42分42秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#define DEVICE_NAME	"/dev/bmp180"

int main(int argc,char *argv[])
{
	int fd=-1,err=-1;
	int data[3];
	double altitude=0.0;
	fd=open(DEVICE_NAME,O_RDWR);
	if(fd<0)
	{
		printf("Can't open the bmp180!\n");
		return -1;
	}
	while(1)
	{
		err=read(fd,data,sizeof(data));
		if(err<0)
		{
			printf("bmp180 read error!\n");
			return -1;
		}
		printf("temp = %ld.%ld^C  p = %ld  ", data[0], data[1], data[2]);
		altitude=44330*(1-pow(data[2]/101325.0,1.0/5.255));
		printf("altitude = %.3f\n", altitude);
		sleep(2);
	}
}
