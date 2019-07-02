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

int C2H5OH_2(void)
{
	int fd = -1;
	int err =0, i;
	int data = 0;
	double vol = 0.0, temp = 0.0, R0 = 0.0, RS = 0.0;
	double c = 0.0;
	fd = open(DEVICE_NAME, O_RDWR);			//以可读可写方式打开设备文件
	if (fd < 0)
	{
		printf(DEVICE_NAME" open error!\n");
		exit(0);
	}
	printf("Preheating sensor2...\n");
	sleep(15);								//等待传感器稳定
	while(1)
	{
		err = read(fd,&data,sizeof(data));
		if (err < 0)
		{
			printf("adc read error!\n");
			exit(-1);
		}
		vol = data * 3.3 / 4096;
		printf("adc2_a = %d\n",data);
		printf("adc2_d = %.3f v\n",vol);
		/*
		i = 150;
		for (; i > 0; i--)	//测量计算传感器在洁净空气中的输出电压
		{

			err = read(fd,&data,sizeof(data));
			if (err < 0)
			{
				printf("adc read error!\n");
				exit(-1);
			}
	//		printf("adc = %d\n",data);
			temp = data * 3.3 / 1024;
			if (i > 140)
				printf("temp = %.3f v\n", temp);
			vol += temp;
			sleep(2);
		}
		*/
		if (vol < V0)
			c = 0.0;
		else
		{
			R0 = (VC / V0 - 1) * RL;		//传感器在洁净空气中的电阻值	
			RS = (VC / vol - 1) * RL;	//传感器在不同气体浓度下的电阻值
			c = (3.4 - RS / R0) / 0.04;		//根据乙醇灵敏度特性曲线:RS / R0 = -0.04c + 3.4	
		}
	/*		
		if(vol > 0.5)				//检测到乙醇
			c = vol * 0.4 / 0.5;	//洁净空气中的乙醇浓度是0.4ppm,0.5为此时对应的电压值
		else
			c = 0.0;
	*/
	/*
		if(vol < 0.39)
			c = 0.0;
		else
			c = vol * 1000 / 5;
	*/
    //		c = 0.0034 * exp(1.2789 * vol);
	//	printf("AIN0 = %.3f v\n",vol / 150);
	
		printf("c2(C2H5OH) = %.3f ppm\n", c);
		sleep(2);
	}
	return 0;
}
