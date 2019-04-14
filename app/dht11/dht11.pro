#-------------------------------------------------
#
# Project created by QtCreator 2019-03-21T20:33:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dht11
TEMPLATE = app


SOURCES += main.cpp\
        mywidget.cpp \
    dht11.cpp

HEADERS  += mywidget.h \
    dht11.h

FORMS    += mywidget.ui
