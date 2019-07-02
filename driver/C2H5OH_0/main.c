/*************************************************************************
	> File Name: main.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2019年02月23日 星期六 19时38分49秒
 ************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int C2H5OH_0(void);
int C2H5OH_2(void);

int main(int argc, char **argv)
{
	pthread_t id_c0, id_c2;
	int ret = 1;

	ret = pthread_create(&id_c0, NULL, (void *)C2H5OH_0, NULL);
	if (ret)
	{
		perror("Create the thread of C2H5OH_0 error!\n");
		return -1;
	}
	printf("The thread of C2H5OH_0 ok!\n");
	ret = pthread_create(&id_c2, NULL, (void *)C2H5OH_2, NULL);
	if (ret)
	{
		perror("Create the thread of C2H5OH_2 error!\n");
		return -1;
	}
	printf("The thread of C2H5OH_2 ok!\n");
	while (1)
	{
		sleep(1);
		printf("main thread...\n");
	}
	return 0;
}
