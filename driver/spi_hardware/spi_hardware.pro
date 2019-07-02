TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    w25q64.c

INCLUDEPATH += /home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4/include
INCLUDEPATH += /home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4/arch/arm/include
INCLUDEPATH += \
/home/zhoubo/desktop/Linux内核移植/tq2440/linux-2.6.30.4/arch/arm/mach-s3c2410/include

HEADERS += \
    w25q64.h
