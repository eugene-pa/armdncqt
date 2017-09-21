#-------------------------------------------------
#
# Project created by QtCreator 2016-03-28T09:40:56
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets serialport sql
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestRs
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    settingsdialog.cpp \
    rsbase.cpp \
    ../common/blockingrs.cpp \
    ../common/defines.cpp \
    ../common/rsasinc.cpp

HEADERS  += dialog.h \
    settingsdialog.h \
    rsbase.h \
    ../common/blockingrs.h \
    ../common/defines.h \
    ../common/rsasinc.h

FORMS    += dialog.ui \
    settingsdialog.ui
