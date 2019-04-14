KERNELDIR=/home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4
INSTALLDIR=/work/nfs_tq
PWD=$(shell pwd)
obj-m:=text.o
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules 
	cp *.ko $(INSTALLDIR)
modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) INSTALL_MOD_PATH=$(INSTALLDIR) modules_install
clean:
	 rm -rf *.o *.ko *.mod.c *.markers *.order *.symvers
.PHONY:modules modules_install

