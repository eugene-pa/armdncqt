#-------------------------------------------------
#
# Project created by QtCreator 2016-03-28T09:40:56
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets serialport
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestRs
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    settingsdialog.cpp \
    rsbase.cpp

HEADERS  += dialog.h \
    settingsdialog.h \
    rsbase.h

FORMS    += dialog.ui \
    settingsdialog.ui
