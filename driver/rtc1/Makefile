KERNELDIR = /home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4
#驱动程序编译时需要从目标平台的内核获取一些信息
PWD := $(shell pwd)	#输出到当前目录
obj-m := rtc_driver.o	
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	cp *.ko /work/nfs_tq
clean:
	rm -rf *.ko *.o *.moc.o *.mod.c *.sysvers *.order *~
