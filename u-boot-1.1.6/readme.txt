
1. 通过在./include/configs/tq24x0.h增加#define CONFIGURE_DTB 1来实现uboot对设备树的支持，屏蔽这句则不支持设备树。
2. 由于合并文件时产生冲突,导致修改过的文件和编译产生的一些中间文件中产生了很多类似"HEAD"这样的非法字符，因此编译时依次make distclean，make tq24x0_defconfig,make,如果编译时产生错误，则删除非法字符（主要出现在include/configs/tq24x0.h、common/main.c、lib_arm/armlinux.c和common/cmd_menu.c中）即可。
