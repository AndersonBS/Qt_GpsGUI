#-------------------------------------------------
#
# Project created by QtCreator 2016-04-28T15:51:29
#
#-------------------------------------------------

QT       += core gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GPS
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gps.cpp \
    serial.cpp \
    socket.cpp

HEADERS  += mainwindow.h \
    gps.h \
    serial.h \
    socket.h
