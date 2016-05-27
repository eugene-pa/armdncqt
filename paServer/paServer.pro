#-------------------------------------------------
#
# Project created by QtCreator 2016-05-25T15:35:51
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = paServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    remoterq.cpp \
    rqabout.cpp

HEADERS  += mainwindow.h \
    remoterq.h \
    rqabout.h

FORMS    += mainwindow.ui
