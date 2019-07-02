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

#define DEVICE_NAME "pwm"

int main(int argc, char **argv)
{
	int fd = -1;
	fd = open("/dev/"DEVICE_NAME, O_RDWR);				//打开驱动文件
	if (fd < 0)
	{
		perror("Can't open /dev/"DEVICE_NAME"\n");
		exit(-1);
	}
	sleep(2);
	close(fd);
	return 0;
}
