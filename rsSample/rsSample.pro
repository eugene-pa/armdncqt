#-------------------------------------------------
#
# Project created by QtCreator 2016-04-01T16:50:48
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rsSample
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    blockingrs.cpp \
    settingsdialog.cpp \
    defines.cpp \
    rasrs.cpp

HEADERS  += mainwindow.h \
    blockingrs.h \
    settingsdialog.h \
    defines.h \
    rasrs.h

FORMS    += mainwindow.ui \
    settingsdialog.ui
