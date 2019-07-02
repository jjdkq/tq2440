ifneq  ($(KERNELRELEASE),)	#Linux版本号不为空(第二次进入Makefile时不为空)
$(info "2nd")				#输出"2nd"
obj-m := led_driver.o led_device.o
else						#linux版本号为空(第一次进入Makefile时为空)
KDIR := /home/zhoubo/desktop/Linux内核移植/tq2440/linux-4.19-rc3	  
PWD:=$(shell pwd)
all: modules app

modules:
#输出"1st"
	$(info "1st")			
#跳转到KDIR路径下的Makefile去执行make M=$(shell pwd) modules命令
	make -C $(KDIR) M=$(PWD) modules	
	cp *.ko /work/nfs_4.19-rc3/driver
	cp *.ko /work/nfs_4.19-rc3/lib/modules/4.19.0-rc3/

CROSS := arm-linux-
OBJ := leds
app: main.c
	$(CROSS)gcc $^ -o $(OBJ)
	$(CROSS)strip $(OBJ)
	cp $(OBJ) /work/nfs_4.19-rc3/sbin
clean:
	rm -f $(OBJ) *.ko *.o *.symvers *.mod.c *.mod.o *.order
endif

