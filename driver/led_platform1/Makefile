ifneq  ($(KERNELRELEASE),)	#Linux版本号不为空(第二次进入Makefile时不为空)
obj-m:=led_driver.o led_device.o
$(info "2nd")				#输出"2nd"
else						#linux版本号为空(第一次进入Makefile时为空)
KDIR :=/home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4	  
PWD:=$(shell pwd)
all:
	$(info "1st")			#输出"1st"
	make -C $(KDIR) M=$(PWD) modules	#跳转到KDIR路径下的Makefile去执行make M=$(shell pwd) modules命令
	cp *.ko /work/nfs_tq
clean:
	rm -f *.ko *.o *.symvers *.mod.c *.mod.o *.order
endif

