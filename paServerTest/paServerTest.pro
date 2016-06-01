#-------------------------------------------------
#
# Project created by QtCreator 2016-06-01T09:28:40
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = paServerTest
TEMPLATE = app

INCLUDEPATH += ../forms/

SOURCES += main.cpp\
        mainwindow.cpp\
    ../forms/qled.cpp \
    ../common/clienttcp.cpp \
    ../common/defines.cpp \
    ../common/logger.cpp \
    ../common/tcpheader.cpp \
    ../common/inireader.cpp \
    ../common/servertcp.cpp \
    ../paServer/remoterq.cpp \
    ../paServer/rqabout.cpp \
    ../paServer/brieffileinfo.cpp \
    ../paServer/headerresponce.cpp


HEADERS  += mainwindow.h\
    ../forms/qled.h \
    ../common/clienttcp.h \
    ../common/defines.h \
    ../common/logger.h \
    ../common/tcpheader.h \
    ../common/inireader.h \
    ../common/servertcp.h \
    ../paServer/remoterq.h \
    ../paServer/rqabout.h \
    ../paServer/brieffileinfo.h \
    ../paServer/headerresponce.h


FORMS    += mainwindow.ui
