
#ifndef DHT11_H
#define DHT11_H

#include <QObject>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

class dht11 : public QObject
{
    Q_OBJECT
private:
    int fd;
public:
    explicit dht11(QObject *parent = 0);
    void dht11_read(int *);
    ~dht11() { }
};

#endif
