KERNELDIR=/home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4   
PWD:=$(shell pwd)   
INSTALLDIR=/work/nfs_tq    
obj-m := cd4051.o  

#all:
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules      
	cp *.ko $(INSTALLDIR)   
modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) INSTALL_MOD_PATH=$(INSTALLDIR) modules_install      
clean:
	rm -rf *.o *.ko *.mod.c *.markers *.order *.symvers   
.PHONY:all clean  
