/*************************************************************************
	> File Name: main.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2019年01月02日 星期三 20时33分31秒
 ************************************************************************/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int fd = -1, err = 0;
	fd = open("/dev/mywdg", O_RDWR);
	if (fd < 0)
	{
		perror("can't open the wdg!\n");
		exit(-1);
	}
	while (1);
}
