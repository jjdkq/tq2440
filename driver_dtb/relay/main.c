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

#define RELAY_ON    0
#define RELAY_OFF   1

int main(int argc, char **argv)
{
    int fd = -1;

    if(argc != 2)
 	{
        printf("Usage:%s on/off\n", argv[0]);
        exit(-1);
 	}

    fd = open("/dev/relay", O_RDWR);				//打开驱动文件
	
    if (fd < 0)
	{
        perror("open error\n");
        exit(-1);
	}

    if (!strcmp(argv[1], "on"))
        ioctl(fd, RELAY_ON, 0);
    else if(!strcmp(argv[1], "off"))
        ioctl(fd, RELAY_OFF, 0);
	else
        exit(-1);
}
