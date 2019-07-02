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

#define ctoi(x)		(x - 48)

int main(int argc, char **argv)
{
	int fd = -1;
	time_t_ time;
	fd = open("/dev/"DEVICE_NAME, O_RDWR);				//打开驱动文件
	
	if (argc < 2)
	{
		printf("usage: rtc -r or rtc -w yyyy-mm-dd w hh:mm:ss\n");
		exit(-1);
	}

	if (fd < 0)
	{
		perror("open "DEVICE_NAME" error\n");
		exit(-1);
	}
	
	if (argc > 2)
	{
		printf("Now set the time: %s  %s  %s\n", argv[2], argv[3], argv[4]);
		time.year = ctoi(argv[2][2]) * 10 + ctoi(argv[2][3]);
		time.mon = ctoi(argv[2][5]) * 10 + ctoi(argv[2][6]);
		time.date = ctoi(argv[2][8]) * 10 + ctoi(argv[2][9]);
		time.day = ctoi(argv[3][0]);
		time.hour = ctoi(argv[4][0]) * 10 + ctoi(argv[4][1]);
		time.min = ctoi(argv[4][3]) * 10 + ctoi(argv[4][4]);
		time.sec = ctoi(argv[4][6]) * 10 + ctoi(argv[4][7]);
		if (ioctl(fd, WRTIME, &time))
		{
			perror("read time error!\n");
			close(fd);
			return -1;
		}
	}
	else
	{
		printf("   %s    %s   %s\n", "Date", "Week", "Time");
		while(1)
		{
			if (ioctl(fd, RDTIME, &time))
			{
				perror("read time error!\n");
				close(fd);
				return -1;
			}
			printf("%04x-%02x-%02x  %1x  %02x:%02x:%02x\n", time.year, time.mon, time.date, time.day, time.hour, time.min, time.sec);
			sleep(1);
		}
	}
	return 0;
}
