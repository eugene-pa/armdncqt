#-------------------------------------------------
#
# Project created by QtCreator 2015-05-18T17:09:47
#
#-------------------------------------------------

QT       += core
QT       += testlib
QT       -= gui

TARGET = TestExpression
CONFIG   += console
CONFIG   += testlib
CONFIG   += c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../common/boolexpression.cpp \
    testexpression.cpp

HEADERS += \
    ../common/boolexpression.h \
    testexpression.h
