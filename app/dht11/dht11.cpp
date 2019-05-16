
#include "dht11.h"

#define DEVICE_NAME     "/dev/dht11"

dht11::dht11(QObject *parent) : QObject(parent)
{
    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0)
    {
        perror("Can't open "DEVICE_NAME"!\n");
        exit(EXIT_FAILURE);
    }
}

void dht11::dht11_read(int *data)
{
    int err = -1;
    err = read(fd, data, sizeof(data));
    if (err < 0)
    {
        perror("Read the dht11 error!\n");
        exit(EXIT_FAILURE);
    }
}

