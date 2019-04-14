KERNELDIR :=/home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4
PWD:=$(shell pwd)
INSTALLDIR:=/work/nfs_tq
obj-m:=C2H5OH.o
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)
	cp *.ko $(INSTALLDIR)
clean:
	rm -rf *.o *.ko *.mod.c *.markers *.order *.symvers
.PHONY:all clean
