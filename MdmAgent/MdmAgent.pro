#-------------------------------------------------
#
# Project created by QtCreator 2016-04-01T10:26:36
#
#-------------------------------------------------

QT       += core gui widgets sql serialport network
CONFIG   += console c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mdmagent
TEMPLATE = app
INCLUDEPATH += ../forms/

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
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
    kpframe.cpp \
    ../forms/qled.cpp \
    ../spr/pereezd.cpp \
    framebm.cpp \
    threadpolling.cpp \
    ../spr/raspacker.cpp \
    ../forms/dlgkpinfo.cpp \
    ../forms/bminfoframe.cpp \
    ../common/servertcp.cpp \
    ../spr/stationnetts.cpp \
    ../spr/stationnettu.cpp \
    ../common/sqlblackbox.cpp \
    ../common/sqlmessage.cpp \
    ../common/sqlserver.cpp \
    ../common/common.cpp \
    hwswitch.cpp


HEADERS  += mainwindow.h \
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
    kpframe.h \
    ../forms/qled.h \
    ../spr/pereezd.h \
    framebm.h \
    threadpolling.h \
    ../spr/raspacker.h \
    ../forms/dlgkpinfo.h \
    ../forms/bminfoframe.h \
    ../common/servertcp.h \
    ../spr/stationnetts.h \
    ../spr/stationnettu.h \
    ../common/sqlblackbox.h \
    ../common/sqlmessage.h \
    ../common/sqlserver.h \
    ../common/common.h \
    hwswitch.h



FORMS    += mainwindow.ui \
    kpframe.ui \
    framebm.ui \
    ../forms/dlgkpinfo.ui \
    ../forms/bminfoframe.ui

RESOURCES += \
    images.qrc
