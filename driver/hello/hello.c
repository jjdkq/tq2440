/*************************************************************************
	> File Name: MyLinux_hello.c
	> Author: zhoubo
	> Mail: 1508540876@qq.com 
	> Created Time: 2018年11月20日 星期二 19时26分32秒
 ************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include "test.h"
extern void test(void);
//extern void test1(void);
static int __init MyLinux_hello_init(void)
{
	printk("This is my first module!\n");
	test();
	test1();
	return 0;
}
static void __exit MyLinux_hello_exit(void)
{
	printk("Exit the module\n");
}

module_init(MyLinux_hello_init);
module_exit(MyLinux_hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhouBo<1508540876@qq.com>");
MODULE_DESCRIPTION("first module");
MODULE_ALIAS("hello");

