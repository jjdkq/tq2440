#########################################################################
# File Name: /work/nfs_tq/led.sh
# Author: zhoubo
# mail: 1508540876@qq.com
# Created Time: 2019年03月10日 星期日 10时53分27秒
#########################################################################

#!/bin/sh

#led开机自启动的脚本文件

#for tslib
export HOME=/root
export TSLIB_ROOT=/usr/local/tslib
export TSLIB_TSDEVICE=/dev/input/event0
export TSLIB_CALIBFILE=/etc/pointercal
export TSLIB_CONFFILE=/usr/local/tslib/etc/ts.conf
export TSLIB_PLUGINDIR=/usr/local/tslib/lib/ts
export TSLIB_ts_bin=/usr/local/tslib/bin
export TSLIB_FBDEVICE=/dev/fb0
export TSLIB_CONSOLEDEVICE=none

#for qt-4.7.3
export QTDIR=/usr/local/qt-4.7.3-arm
export QT_QWS_FONTDIR=$QTDIR/lib/fonts     
export QT_PLUGIN_PATH=$QTDIR/plugins       
export QWS_MOUSE_PROTO=Tslib:/dev/input/event0

export LD_LIBRARY_PATH=$TSLIB_ROOT/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH

base=/app/gps						#(注意:=两边不能有空格)
case "$1" in							#$1表示传入shell脚本的第1个参数
	start)								#传入start参数
		$base -qws -font unifont &		#后台运行led的qt程序
		;;
	stop)
		val=$(ps | grep led_arm | grep qws)
		#或者
	#	val=`ps | grep led_arm | grep qws`  #`是反引号,不是单引号
		pid=${val#0*}					#得到led程序的pid号(val#0*表示从左边开始删除第一个0及其右边的所有字符)
		if [ -n "$pid" ]				#pid号不为空(注意:[和-n之间要有空格)
		then
			kill $pid					#杀死进程
		fi								#if语句结束
		;;
esac									#case语句结束
exit 0									#退出该shell脚本
