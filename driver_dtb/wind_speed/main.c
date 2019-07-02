/*************************************************************************
	> File Name: main.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2019年05月22日 星期三 15时34分52秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int fd;
int device_open(void)
{
	fd = open("/dev/wind_speed", O_RDWR);
	if (fd < 0)
	{
		printf("Can't open the wind_speed device\n");
		return -1;
	}
}

void mydelay(unsigned long x)
{
	unsigned long y = 50000;
	for (; y > 0; y--)
		for (; x > 0; x--);
}

int main(int argc, char **argv)
{
	int i;
	device_open();
	double windSpeed = 0.0;
	int f;
	while (1)
	{
		i = read(fd, &f, sizeof(f));
		if (i < 0)
			perror("Can't read the wind_speed device\n");
	//	printf("The main thread is running...\n");
		windSpeed = (f * 7) / 80.0 + 0.1;
		if(windSpeed == 0.1) //风速等于0
			windSpeed = 0;
		printf("wind_speed = %.3f m/s\n", windSpeed);
	}
	close(fd);
	return 0;
}
