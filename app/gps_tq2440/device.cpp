
#include "device.h"

device::device(int *fd, const char *dev_path, int flag)
{
    *fd = open(dev_path, flag);
    if (*fd < 0)
    {
        printf("Can't open %s\n", dev_path);
        exit(-1);
    }
}

int device::myread(int fd, void *datap, int size)
{
    return read(fd, datap, size);
}

int device::mywrite(int fd, void *datap, int size)
{
    return write(fd, datap, size);
}

int device::myioctl(int fd, unsigned cmd, unsigned long arg)
{
    return ioctl(fd, cmd, arg);
}
