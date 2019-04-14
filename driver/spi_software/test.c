
/*************************************************************************
    > File Name: test.c
    > Author: zhoubo
    > Mail: 1508540876@qq.com
    > Created Time: 2019年03月18日 星期一 20时51分59秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEVICE_NAME		"/dev/w25q64"

const unsigned char data_w[] = "Taylor Swift";

int main(int argc, char **argv)
{
    int fd = -1, err = -1;

    unsigned char data_r[sizeof(data_w)];
    sleep(2);                       // 等待w25q64稳定
    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0)
    {
        perror("Can't open the w25q64!\n");
        return -1;
    }
    while(1)
    {

        err = write(fd, (unsigned char *)data_w, sizeof(data_w));
        if (err < 0)
        {
            perror("w25q64 write error!\n");
            return -1;
        }   

        err = read(fd, (unsigned char *)data_r, sizeof(data_r));
        if (err < 0)
        {
            perror("w25q64 read error!\n");
            return -1;
        }
        printf("read data: %s\n", data_r);
        sleep(2);
    }
}
