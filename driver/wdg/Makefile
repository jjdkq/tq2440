KERNEL := /home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4
PWD := $(shell pwd)
obj-m := wdg_driver.o
modules:
	$(MAKE) -C $(KERNEL) M=$(PWD) modules
	cp *.ko /work/nfs_tq
clean:
	rm -f *.ko *.o *.symvers *.mod.c *.mod.o *.order
.PHONY:modules clean
	
