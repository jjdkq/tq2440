#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/io.h>
#include<errno.h>


#define LEN 256
#define PRINT_ERROR printf("error: %d:%s",errno,strerror(errno))


/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
	int ret,fd,i,j;
	char read_data[LEN];
	char write_data[LEN] ="<Sky_handy@outlook.com>\n";
	char offset[LEN];

	memset(offset,0,sizeof(offset));
	fd = open("/sys/devices/platform/s3c2440-i2c/i2c-adapter/i2c-0/0-0050/eeprom",O_RDWR);
	if(fd < 0)
	{
		printf("Open AT24C02 fail.\n");
		return -1;
	}
	/*  
   ret = read(fd, offset,256);
   if(ret < 0)
   {
       printf("Read AT24C02 fail.\n"); 
        return -1;
   }
    strncpy(read_data, offset,sizeof(offset));
    for(i = 0 ; i<sizeof(read_data); i++)
    {
      printf("%c\n",read_data[i]);
    }//读出当前EEPROM中的数据
*/
	ret = lseek(fd, 0, SEEK_SET);//调整偏移量，从开始位置写入
	if(-1 == ret)
	{
		PRINT_ERROR;
		return -1;
	}
	ret = write(fd ,write_data, sizeof(write_data));
	if(ret < 0)
	{
		printf("Write error\n");
		return -1;
	}//将指定数据写入当前EEPROM中
	lseek(fd, 0, SEEK_SET);//重新设置偏移量从起始位置读取
	ret = read(fd , read_data, LEN);
	if(ret < 0)
	{
		printf("Read error\n");
		return -1;
	}
	else if(ret < 256)
	{
		printf("Incomplete read\n");
		return -1;
	}
	printf("%s\n",read_data);
	return 0;
} /* ----- End of main() ----- */


