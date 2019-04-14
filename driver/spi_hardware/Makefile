KERNELDIR := /home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4/
PWD := $(shell pwd)
INSTALLDIR = /work/nfs_tq/driver
obj-m := w25q64.o 
#obj-m := m25p80.o 
#myw25q64-objs := spi.o w25q64.o
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	cp *.ko $(INSTALLDIR)
clean:
	rm -rf *.o *.ko *.mod.c *.markers *.order *.symvers
.PHONY: modules clean
