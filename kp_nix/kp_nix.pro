#-------------------------------------------------
#
# Project created by QtCreator 2017-08-18T16:29:16
#
#-------------------------------------------------

QT       += core gui serialport
CONFIG   += console c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kp_nix
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
        mainwindow.cpp \
        threads/threadpolling.cpp \
        threads/threadmonitoring.cpp \
        threads/threadpulse.cpp \
        threads/threadsyscommand.cpp \
        threads/threadtesttu.cpp \
        threads/threadts.cpp \
        threads/threadtu.cpp \
        threads/threadupok.cpp \
        threads/threadwatchdog.cpp \
        common/blockingrs.cpp \
        common/common.cpp \
        common/pamessage.cpp \
        qled.cpp \
        framebase.cpp \
    framemvv.cpp \
    framebt.cpp \
    framemvvstatus.cpp \
    farmeline.cpp



HEADERS += \
        mainwindow.h \
        threads/threadpolling.h \
        threads/threadts.h \
        threads/threadtu.h \
        common/acksenum.h \
        common/blockingrs.h \
        common/common.h \
        common/pamessage.h \
        qled.h \
        framebase.h \
    framemvv.h \
    framebt.h \
    framemvvstatus.h \
    farmeline.h

FORMS += \
        mainwindow.ui \
        framebase.ui \
    framemvv.ui \
    framebt.ui \
    framemvvstatus.ui \
    farmeline.ui

DISTFILES +=

RESOURCES += \
    images.qrc \
