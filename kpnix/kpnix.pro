QT += core serialport
QT -= gui

CONFIG += c++11

TARGET = kpnix
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    threadmonitoring.cpp \
    threadpulse.cpp \
    threadsyscommand.cpp \
    threadtesttu.cpp \
    threadts.cpp \
    threadtu.cpp \
    threadupok.cpp \
    threadwatchdog.cpp \
    ../common/rsasinc.cpp \
    threadpolling.cpp \
    ../common/pamessage.cpp \
    ../common/pasender.cpp \
    objmain.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    main.h \
    ../common/rsasinc.h \
    kp2017.h \
    ../common/pamessage.h \
    ../common/pasender.h \
    objmain.h
