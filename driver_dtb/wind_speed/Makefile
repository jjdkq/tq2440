

OBJ := wind_speed
#OBJ_1 := led
ifneq  ($(KERNELRELEASE), )		#Linux版本号不为空(第二次进入Makefile时不为空)
#输出"2nd"	
$(info "2nd")				
obj-m := $(OBJ)_drv.o 

ifneq ($(OBJ_1), )
obj-m += $(OBJ_1)_drv.o 
endif

else							#linux版本号为空(第一次进入Makefile时为空)			
KDIR := /home/zhoubo/desktop/Linux内核移植/tq2440/linux-4.19-rc3	  
PWD := $(shell pwd)
all: modules dtbs app

#编译模块
modules:
#输出"1st"
	$(info "1st")
#跳转到KDIR路径下的Makefile去执行make M=$(shell pwd) modules命令			
	make -C $(KDIR) M=$(PWD) modules
	cp *.ko /work/nfs_4.19-rc3/driver
#为了可以正常rmmod
	cp *.ko /work/nfs_4.19-rc3/lib/modules/4.19.0-rc3

device = s3c2440-tq2440
src_dts = $(device).dts
tmp_dts = $(device).tmp.dts
dst_dtb = $(device).dtb

#编译设备树
dtbs:
	$(info "1st")			
#方法1:采用这种方法时,.dts文件中不能有宏定义,文件包含#include "xxx"
#要用/include/ "xxx"来代替
#使用dtc(位于内核目录下的scripts/dtc/中)编译成.dtb文件
#dtc -I dts -O dtb -o s3c2440-tq2440.dtb $(src_dts)	
	
#方法2:.dts中可以像什.c文件那样用#define和#include
#使用cpp命令对.dts文件进行处理,完成文件包含和宏替换的工作
	cpp -nostdinc -I. -undef -x assembler-with-cpp $(src_dts) > $(tmp_dts)
#将cpp处理得到的中间文件通过dtc命令编译成.dtb文件
	dtc -O dtb -o $(dst_dtb) $(tmp_dts)	

	cp *.dtb /work/tftp
	rm $(tmp_dts)

CROSS := arm-linux-

#编译应用
app: main.c 
	$(CROSS)gcc $^ -o $(OBJ)
	$(CROSS)strip $(OBJ)	
#去除编译程序中的调试信息,使可执行文件减小	
	cp $(OBJ) /work/nfs_4.19-rc3/sbin
clean:
	rm -f $(OBJ) *.dtb *.ko *.o *.symvers *.mod.c *.mod.o *.order
endif

