TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    at24c02_test.c \
    bmp180_drv.c \
    main.c \
    test.c \
    myi2c_dev.c \
    myi2c_dev.c \
    myi2c_drv.c \
    bmp180.c

DISTFILES +=

HEADERS += \
    bmp180.h \
    myi2c.h


INCLUDEPATH += /home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4/include
INCLUDEPATH += /home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4/arch/arm/include
INCLUDEPATH += \
/home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4/arch/arm/mach-s3c2410/include
