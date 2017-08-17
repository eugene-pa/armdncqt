#-------------------------------------------------
#
# Project created by QtCreator 2017-08-09T10:26:43
#
#-------------------------------------------------

QT       += core gui serialport

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tryRsProxyDlg
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        dialog.cpp \
    ../common/rsasinc.cpp \
    ../common/pamessage.cpp \
    ../common/pasender.cpp \
    threadpolling.cpp \
    ../common/qrsasinc.cpp \
    ../common/common.cpp

HEADERS += \
        dialog.h \
    ../common/rsasinc.h \
    ../common/pamessage.h \
    ../common/pasender.h \
    ../common/qrsasinc.h

FORMS += \
        dialog.ui
