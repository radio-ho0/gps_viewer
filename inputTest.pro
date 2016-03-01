#-------------------------------------------------
#
# Project created by QtCreator 2015-08-27T17:04:33
#
#-------------------------------------------------

QT       += core gui serialport
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = inputTest
TEMPLATE = app


SOURCES += main.cpp\
        inputtest.cpp \
    gpsclass.cpp

HEADERS  += inputtest.h \
    gpsclass.h

FORMS    += inputtest.ui

RESOURCES += \
    icos.qrc
