#-------------------------------------------------
#
# Project created by QtCreator 2015-04-23T10:59:57
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = shaper
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        shapechild.cpp

HEADERS  += mainwindow.h \
        shapechild.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc
