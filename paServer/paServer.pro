#-------------------------------------------------
#
# Project created by QtCreator 2016-05-25T15:35:51
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = paServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    remoterq.cpp \
    brieffileinfo.cpp \
    headerresponce.cpp \
    responceabout.cpp \
    responcedirs.cpp \
    responcefileinfo.cpp \
    responcefiles.cpp \
    responcedrives.cpp \
    responcetempfile.cpp \
    responceread.cpp \
    responceerror.cpp \
    ../common/clienttcp.cpp \
    ../common/defines.cpp \
    ../common/logger.cpp \
    ../common/tcpheader.cpp \
    ../common/inireader.cpp \
    ../common/servertcp.cpp \
    ../forms/qled.cpp

HEADERS  += mainwindow.h \
    remoterq.h \
    brieffileinfo.h \
    headerresponce.h \
    responceabout.h \
    responcedirs.h \
    responcefileinfo.h \
    responcefiles.h \
    responcedrives.h \
    responcetempfile.h \
    responceread.h \
    responceerror.h \
    ../common/clienttcp.h \
    ../common/defines.h \
    ../common/logger.h \
    ../common/tcpheader.h \
    ../common/inireader.h \
    ../common/servertcp.h \
    ../forms/qled.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc
