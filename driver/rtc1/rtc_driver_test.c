#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
int main()
{
	volatile int i, j, k;
	int fd;
	struct rtc_time time={0};	
	fd = open("/dev/misc_rtc", O_RDWR, S_IRUSR | S_IWUSR);
	if(ioctl(fd, 999999) != 0)
	{
		printf("打开滴答中断失败\n");
		close(fd);
		return -1;
	}
	for(i = 0; i < 1000; i++)
		for(j = 0; j < 1000; j++)
			for(k = 0; k < 1000; k++);
	if(ioctl(fd, 1000000) != 0)
		printf("关闭滴答中断失败\n");
	close(fd);
	return 0;
}

