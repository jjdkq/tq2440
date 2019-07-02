#ifndef DEVICE_H
#define DEVICE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

class device
{
public:
    device(int *, const char *, int);
    int myread(int, void *, int);
    int mywrite(int, void *, int);
    int myioctl(int, unsigned, unsigned long);
};
#endif // DEVICE_H

