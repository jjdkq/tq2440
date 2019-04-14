KERNELDIR=/home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4   
PWD:=$(shell pwd)   
INSTALLDIR=/work/nfs_tq    

#模块依赖:依次insmod test.ko test1.ko hello.ko
#注意:此时.c文件中要使用EXPORT_SYMBOL()
obj-m := hello.o  
obj-m += test.o  
obj-m += test1.o  
#或者
#obj-m=hello.o test.o test1.o

#不使用模块依赖:直接insmod myhello.ko
#obj-m := myhello.o  
#myhello-objs=hello.o test.o test1.o

#modules:   
#	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules  
#	cp *.ko $(INSTALLDIR)   
#modules_install:   
#	$(MAKE) -C $(KERNELDIR) M=$(PWD) INSTALL_MOD_PATH=$(INSTALLDIR) modules_install   
#.PHONY:modules modules_install clean  

all:   
	$(MAKE) -C $(KERNELDIR) M=$(PWD)    
	cp *.ko $(INSTALLDIR)   
clean:
	rm -rf *.o *.ko *.mod.c *.markers *.order *.symvers   
.PHONY:all clean  
