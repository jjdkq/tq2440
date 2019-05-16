/*************************************************************************
	> File Name: test.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年11月20日 星期二 21时35分54秒
 ************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
static void test(void)			//使用模块依赖的时候可以加static,否则,不能加static
{
	printk("This is a test!\n");
}
EXPORT_SYMBOL(test);
