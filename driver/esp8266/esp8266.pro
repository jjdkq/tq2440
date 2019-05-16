TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    common.c \
    uart.c \
    wifiap.c

HEADERS += \
    uart.h \
    common.h

