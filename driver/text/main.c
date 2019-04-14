/*************************************************************************
	> File Name: main.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年12月17日 星期一 14时10分43秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define DEVICE_NAME	"/dev/text"

int main(int argc,char *argv[])
{
	FILE *fp;
	char dat=10;
	int ret=0,fd=0;
	char ch[]="hello";
	fp=fopen("text.txt","wb");
	fd=open(DEVICE_NAME,O_RDWR);
/*	if(!fp)
	{
		printf("can't open the text!\n'");
		return -1;
	}
	*/
	sprintf(ch,"%d",dat);
	fwrite(ch,sizeof(char),strlen(ch),fp);
	write(fd,&dat,sizeof(dat));
/*	if(!ret)
	{
		printf("can't write the text!\n");
		return -1;
	}
	*/
	fclose(fp);
	return 0;
}
