/*************************************************************************
	> File Name: test.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2019年01月08日 星期二 16时12分07秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>		//for struct input_event

#define KEY_A	30				//外部中断0对应的键值(在驱动中设置)
#define KEY_B	48				//外部中断1对应的键值
#define KEY_C	46				//外部中断2对应的键值
#define KEY_D	32				//外部中断4对应的键值

/* 注意:需要修改文件系统的/etc/mdev.conf文件在/dev目录下建立input子目录 
 * /dev/input/event[0-3]分别对应4个按键的中断
*/

int main(int argc, char *argv[])
{
	int fd = -1, count = -1;
	struct input_event inev;
	if (argc != 2)
	{
		printf("usage: exint /dev/input/event[0-3]\n");
		exit(-1);
	}
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		perror("Can't open the input device!\n");
		exit(-1);
	}
	count = read(fd, &inev, sizeof(struct input_event));
	if (count < 0)
	{
		perror("read input device failed!\n");
		exit(-1);
	}
	if (inev.type == EV_KEY)		//中断事件类型是按键中断			
	{
		printf("This is a key event!\n");
		printf("code = %d  value = %d\n", inev.code, inev.value);
	}
	switch (inev.code)
	{
		case KEY_A:
			printf("This is EXINT0\n");
		break;
		case KEY_B:
			printf("This is EXINT1\n");
		break;
		case KEY_C:
			printf("This is EXINT2\n");
		break;
		case KEY_D:
			printf("This is EXINT4\n");
		break;
	}
	return 0;
}


