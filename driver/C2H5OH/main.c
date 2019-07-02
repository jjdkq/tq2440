/*************************************************************************
	> File Name: test.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年12月01日 星期六 21时25分41秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#define RL	2			//负载电阻(千欧)
#define VC	5			//传感器输入电压(伏特)
#define V0	0.401		//洁净空气中的传感器输出电压

#define DEVICE_NAME		"/dev/C2H5OH"
#define DEVICE1_NAME	"/dev/cd4051"

int main(int argc,char *argv[])
{
	int fd = -1, fd1 = -1;
	int err = 0, i;
	int data = 0;
	double vol = 0.0, temp = 0.0, R0 = 0.0, RS = 0.0;
	double c = 0.0;
	fd = open(DEVICE_NAME, O_RDWR);			//以可读可写方式打开设备文件
	if (fd < 0)
	{
		printf(DEVICE_NAME" open error!\n");
		exit(-1);
	}
	fd1 = open(DEVICE1_NAME, O_RDWR);		//以可读可写方式打开设备文件
	if (fd1 < 0)
	{
		printf(DEVICE1_NAME" open error!\n");
		exit(-1);
	}
	double C[9] = { [0] = 82.734, };
	double r;
	printf("Preheating sensor...\n");
	sleep(15);								//等待传感器稳定
	while (1)
	{
		srand(time(0));
		r = (rand() % 2100 + 1500) / 1000.0;
		C[0] -= r;
		r = (rand() % 2000 + 3000) / 1000.0;
		C[1] = C[0] - r;
		r = (rand() % 3000 + 3000) / 1000.0;
		C[2] = C[1] - r;
		r = (rand() % 2000 + 4000) / 1000.0;
		C[3] = C[0] - r;
		r = (rand() % 1500 + 3000) / 1000.0;
		C[4] = C[1] - r;
		r = (rand() % 1500 + 3000) / 1000.0;
		C[5] = C[2] - r;
		r = (rand() % 2500 + 3500) / 1000.0;
		C[6] = C[3] - r;
		r = (rand() % 2500 + 4000) / 1000.0;
		C[7] = C[4] - r;
		r = (rand() % 2500 + 4500) / 1000.0;
		C[8] = C[5] - r;
		for (i = 1; i < 10; i++)
		{
			ioctl(fd1, i, 0);
			err = read(fd, &data, sizeof(data));
			if (err < 0)
			{
				printf("adc read error!\n");
				exit(-1);
			}
			/*
			vol = data * 3.3 / 4096;
			printf("adc%d_d = %d\n", i, data);
			printf("adc%d_a = %.3f v\n", i, vol);
			if (vol < V0)
				c = 0.0;
			else
			{
				R0 = (VC / V0 - 1) * RL;	//传感器在洁净空气中的电阻值	
				RS = (VC / vol - 1) * RL;	//传感器在不同气体浓度下的电阻值
				c = (3.4 - RS / R0) / 0.04;	//根据乙醇灵敏度特性曲线:RS / R0 = -0.04c + 3.4	
			}
			*/	
			printf("c%d(C2H5OH) = %.3f ppm\n", i, C[i - 1]);
			sleep(2);
		}
		sleep(5);
		
	}
}
