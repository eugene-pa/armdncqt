#-------------------------------------------------
#
# Project created by QtCreator 2016-05-17T11:35:22
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BridgeTcp
TEMPLATE = app
INCLUDEPATH += ../forms/

SOURCES += main.cpp\
    ../forms/qled.cpp \
    ../common/clienttcp.cpp \
    ../common/defines.cpp \
    ../common/logger.cpp \
    ../common/tcpheader.cpp \
    ../common/inireader.cpp \
    ../common/servertcp.cpp \
    mainwindow.cpp

HEADERS  += \
    ../forms/qled.h \
    ../common/clienttcp.h \
    ../common/defines.h \
    ../common/logger.h \
    ../common/tcpheader.h \
    ../common/inireader.h \
    ../common/servertcp.h \
    mainwindow.h

FORMS    += \
    mainwindow.ui

DISTFILES += \
    BridgeTcp.ini \
    history.txt

RESOURCES += \
    images.qrc
