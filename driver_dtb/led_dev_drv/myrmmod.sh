#########################################################################
# File Name: myinsmod.sh
# Author: zhoubo
# mail: 1508540876@qq.com
# Created Time: 2019年05月17日 星期五 21时21分43秒
#########################################################################
#!/bin/bash


if [ -n "$1" ]
then
	val="$1"
	temp=${val#*/}			#去掉第1个输入参数(driver/*.ko)左边第1个'/'及其左边的字符,只剩*.ko
	temp1=${temp%.*}		#去掉*.ko右边第1个'.'及其右边的字符
	echo 'rmmod'$temp1
	rmmod $temp1
else
	echo 'usage: myinsmod *.ko'
fi
exit 0
