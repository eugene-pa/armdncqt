#-------------------------------------------------
#
# Project created by QtCreator 2015-06-29T09:53:53
#
#-------------------------------------------------

QT       += core gui sql
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestTcp
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    ../common/tcpheader.cpp \
    ../common/defines.cpp \
    ../common/clienttcp.cpp \
    ../common/logger.cpp

HEADERS  += dialog.h \
    ../common/tcpheader.h \
    ../common/defines.h \
    ../common/clienttcp.h \
    ../common/logger.h

FORMS    += dialog.ui
