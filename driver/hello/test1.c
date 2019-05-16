/*************************************************************************
	> File Name: src/test1.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年11月20日 星期二 22时02分41秒
 ************************************************************************/

//#include "test.h"
#include <linux/module.h>
#include <linux/kernel.h>
static int bau=9600;
static int port[]={0,1,2,3};
static char name[]="serial port";
//module_param(bau,int,S_IRUGO);
//module_param_array(port,int,NULL,S_IRUGO);
//module_param(name,charp,S_IRUGO);
void test1(void)
{
	int i=0;
	printk("bau=%d\n",bau);
	printk("port:");
	for(i=0;i<ARRAY_SIZE(port);i++)
	{
		printk("%d  ",port[i]);
	}
	printk("\n");
	printk("name:%s\n",name);
}
EXPORT_SYMBOL(test1);
