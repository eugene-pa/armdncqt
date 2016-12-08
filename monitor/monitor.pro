#-------------------------------------------------
#
# Project created by QtCreator 2016-11-29T09:21:12
#
#-------------------------------------------------

QT       += core gui network widgets sql serialport
CONFIG   += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = monitor
TEMPLATE = app
INCLUDEPATH += ../forms/

SOURCES += main.cpp\
        mainwindow.cpp\
    ../common/boolexpression.cpp \
    ../common/defines.cpp \
    ../common/archiver.cpp \
    ../common/clienttcp.cpp \
    ../common/tcpheader.cpp \
    ../common/logger.cpp \
    ../common/inireader.cpp \
    ../spr/esr.cpp \
    ../spr/rc.cpp \
    ../spr/sprbase.cpp \
    ../spr/station.cpp \
    ../spr/ts.cpp \
    ../spr/properties.cpp \
    ../spr/strl.cpp \
    ../spr/svtf.cpp \
    ../spr/sysinfo.cpp \
    ../spr/tu.cpp \
    ../spr/streamts.cpp \
    ../spr/dstdatafromfonitor.cpp \
    ../spr/dras.cpp \
    ../spr/datafrommonitor.cpp \
    ../spr/peregon.cpp \
    ../spr/train.cpp \
    ../spr/route.cpp \
    ../spr/abtcminfo.cpp \
    ../spr/rpcdialoginfo.cpp \
    ../spr/ecmpkinfo.cpp \
    ../spr/krug.cpp \
    ../common/blockingrs.cpp \
    ../forms/qled.cpp \
    ../common/qled.cpp \
    ../spr/pereezd.cpp


HEADERS  += mainwindow.h\
    ../common/boolexpression.h \
    ../common/inireader.h \
    ../common/defines.h \
    ../common/logger.h \
    ../common/archiver.h \
    ../common/clienttcp.h \
    ../common/tcpheader.h \
    ../spr/enums.h \
    ../spr/esr.h \
    ../spr/rc.h \
    ../spr/sprbase.h \
    ../spr/station.h \
    ../spr/ts.h \
    ../spr/properties.h \
    ../spr/strl.h \
    ../spr/svtf.h \
    ../spr/sysinfo.h \
    ../spr/tu.h \
    ../spr/streamts.h \
    ../spr/dstdatafromfonitor.h \
    ../spr/dras.h \
    ../spr/datafrommonitor.h \
    ../spr/peregon.h \
    ../spr/train.h \
    ../spr/route.h \
    ../spr/abtcminfo.h \
    ../spr/rpcdialoginfo.h \
    ../spr/ecmpkinfo.h \
    ../spr/krug.h \
    ../common/blockingrs.h \
    ../forms/qled.h \
    ../common/qled.h \
    ../spr/pereezd.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc
