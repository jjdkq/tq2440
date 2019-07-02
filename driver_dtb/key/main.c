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

#define KEY0_PRESS 1
#define KEY1_PRESS 2
#define KEY2_PRESS 3
#define KEY4_PRESS 4

int led_fd, key_fd;
int device_open(void)
{
	led_fd = open("/dev/led", O_RDWR);
	if (led_fd < 0)
	{
		printf("Can't open the led device\n");
		return -1;
	}

	key_fd = open("/dev/key", O_RDWR);
	if (key_fd < 0)
	{
		printf("Can't open the key device\n");
		return -1;
	}
}

int key_scan(void)
{
	int key = 0;
	read(key_fd, &key, sizeof(key));
	return key;
}

void led_sta(int sta, int no)
{
	ioctl(led_fd, sta, no);
}

void mydelay(unsigned long x)
{
	unsigned long y = 50000;
	for (; y > 0; y--)
		for (; x > 0; x--);
}
void led_ctrl(void)
{
	int key = key_scan();
	mydelay(20000);
	switch (key)
	{
		case KEY0_PRESS:
			printf("KEY0_PRESS\n");
			led_sta(0, 0);
			led_sta(0, 2);
			led_sta(1, 1);
			led_sta(1, 3);
			break;
		case KEY1_PRESS:
			printf("KEY1_PRESS\n");
			led_sta(1, 0);
			led_sta(1, 2);
			led_sta(0, 1);
			led_sta(0, 3);
			break;
		case KEY2_PRESS:
			printf("KEY2_PRESS\n");
			led_sta(1, 0);
			led_sta(1, 1);
			led_sta(0, 2);
			led_sta(0, 3);
			break;
		case KEY4_PRESS:
			printf("KEY4_PRESS\n");
			led_sta(1, 0);
			led_sta(1, 3);
			led_sta(1, 1);
			led_sta(0, 2);
			break;
	}
	key = 0;
}

int main(int argc, char **argv)
{
	device_open();
	int i;
	for ( i = 0; i < 4; i++)
		ioctl(led_fd, 1, i);
	while (1)
		led_ctrl();
}
