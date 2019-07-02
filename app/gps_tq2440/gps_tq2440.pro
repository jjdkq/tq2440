#-------------------------------------------------
#
# Project created by QtCreator 2019-06-06T13:52:00
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gps
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    myitem.cpp \
    uart.cpp \
    gps.cpp \
    connection.cpp \
    device.cpp \
    mydialog.cpp

HEADERS  += mainwindow.h \
    myitem.h \
    uart.h \
    gps.h \
    device.h \
    rtc.h \
    mydialog.h \
    connection.h \
    ui_mydialog.h

FORMS    += mainwindow.ui \
    mydialog.ui

INCLUDEPATH += /home/zhoubo/desktop/Qt/pinyin_inc
QMAKE_LIBS += -L /work/Syszuxpinyin/lib -lsyszuxpinyin
