
#include <common.h>
#include <command.h>
#include <nand.h>

extern char console_buffer[];
extern int readline (const char *const prompt);

extern char awaitkey(unsigned long delay, int* error_p);
//extern void download_nkbin_to_flash(void);


char awaitkey(unsigned long delay, int* error_p)
{
	int i;
	char c;
	if (delay == -1) 
	{
		while (1) 
		{
			if (tstc()) /* we got a key press	*/
				return getc();
		}
	}
	for (i = 0; i < delay; i++) 
	{
		if (tstc()) /* we got a key press	*/
			return getc();
		udelay (10*1000);
	}
	if (error_p)
		*error_p = -1;
	return 0;
}

void main_menu_usage(void)
{
    printf("\r\n#############  My Uboot Menu  #############\r\n");

    printf("[b] Boot the system\r\n");
    printf("[e] Erase the Nand Flash\r\n");
    printf("[f] Download file system(root.bin) to Nand Flash by tftp\r\n");
    printf("[k] Download linux kernel(uImage) to Nand Flash by tftp\r\n");
    printf("[u] Download bootloader(u-boot.bin) to Nand Flash by tftp\r\n");
    printf("[n] Boot the system from nfs file system\r\n");
    printf("[y] Boot the system from yaffs file system\r\n");
    printf("[q] Quit from menu\r\n");

    printf("Enter your selection: ");
}


void menu_shell(void)
{
    char c;
    char cmd_buf[200];
	char param_buf[64];
    while (1)
    {
        main_menu_usage();
        c = awaitkey(-1, NULL);
        printf("%c\n", c);
        switch (c)
        {
			case 'b':
				sprintf(cmd_buf, "ping 192.168.1.8");
				run_command(cmd_buf,0);

				printf("Booting Linux ...\n");
	strcpy(cmd_buf, "nand read.jffs2 0x30000000 kernel;bootm 0x30000000");
				run_command(cmd_buf, 0); 
				break;
            case 'k':
                strcpy(cmd_buf, "tftp 0x30000000 uImage;nand erase kernel;nand write.jffs2 0x30000000 kernel $(filesize)");
                run_command(cmd_buf, 0);
                break;
            case 'e':
                strcpy(cmd_buf, "nand erase");
                run_command(cmd_buf, 0);
                break;
			case 'f':
				strcpy(cmd_buf,"tftp 0x30000000 root.bin;nand erase root;nand write.yaffs 0x30000000 root $(filesize);");
				run_command(cmd_buf,0);
				break;
			case 'n':		
				sprintf(cmd_buf, "ping 192.168.1.8");
				run_command(cmd_buf,0);

				sprintf(cmd_buf, "setenv bootargs ");

				printf("Enter nfs directory:\n");
				readline(NULL);
				strcpy(param_buf,console_buffer);
				sprintf(cmd_buf, "setenv bootargs console=ttySAC0 console=tty1 root=/dev/nfs nfsroot=192.168.1.8:%s ip=192.168.1.6:192.168.1.8:192.168.1.6:255.255.255.0:SKY2440.embedsky.net:eth0:off",param_buf);

				run_command(cmd_buf, 0);
				sprintf(cmd_buf,"saveenv");
				run_command(cmd_buf,0);
				printf("Booting Linux ...\n");
	strcpy(cmd_buf, "nand read.jffs2 0x30000000 kernel;bootm 0x30000000");
				run_command(cmd_buf, 0); 
				break;
			case 'y':
				strcpy(cmd_buf,"setenv bootargs noinitrd root=/dev/mtdblock2 init=/linuxrc console=ttySAC0");
				run_command(cmd_buf,0);
				sprintf(cmd_buf,"saveenv");
				run_command(cmd_buf,0);
				printf("Booting Linux ...\n");
	strcpy(cmd_buf, "nand read.jffs2 0x30000000 kernel;bootm 0x30000000");
				run_command(cmd_buf, 0); 
				break;
            case 'u':
                strcpy(cmd_buf, "tftp 0x30000000 u-boot.bin;nand erase bootloader;nand write.jffs2 0x30000000 bootloader $(filesize)");
                run_command(cmd_buf, 0);
                break;
            case 'q':
                return;    
        }
                
    }
}

int do_menu (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    menu_shell();
    return 0;
}

U_BOOT_CMD(
	menu,	3,	0,	do_menu,
	"menu - display a menu, to select the items to do something\n",
	" - display a menu, to select the items to do something"
);

