KERNELDIR=/home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4   
PWD:=$(shell pwd)   
INSTALLDIR=/work/nfs_tq/driver    

#使用这种方法的时候,myi2c.c里要屏蔽EXPORT_SYMBOL(),并且函数不能加static!!!
#obj-m := mybmp180.o  
#mybmp180-objs= myi2c.o bmp180.o

#使用这种方法的时候,myi2c.c里要有EXPORT_SYMBOL(),然后依次insmod myi2c.ko,
#insmod bmp180.ko,卸载的时候依次rmmod bmp180,rmmod myi2c
obj-m:=bmp180.o
obj-m+=myi2c.o

modules:  
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules     
	cp *.ko $(INSTALLDIR)   
clean:
	rm -rf *.o *.ko *.mod.c *.markers *.order *.symvers  
.PHONY:modules clean  
