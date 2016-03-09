#-------------------------------------------------
#
# Project created by QtCreator 2015-04-23T10:59:57
#
#-------------------------------------------------

QT       += core gui widgets sql
CONFIG   += console
QT       += network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = shaper
TEMPLATE = app

INCLUDEPATH += ../forms/

SOURCES += main.cpp\
        mainwindow.cpp \
        shapechild.cpp \
    ../shapes/shape.cpp \
    ../shapes/shapeset.cpp \
    ../common/logger.cpp \
    ../shapes/shaperc.cpp \
    ../shapes/shapestrl.cpp \
    ../shapes/shapesvtf.cpp \
    ../shapes/shapetrnsp.cpp \
    ../shapes/colorscheme.cpp \
    ../spr/esr.cpp \
    ../spr/rc.cpp \
    ../spr/sprbase.cpp \
    ../spr/station.cpp \
    ../spr/ts.cpp \
    ../common/boolexpression.cpp \
    ../spr/properties.cpp \
    ../spr/strl.cpp \
    ../spr/svtf.cpp \
    ../spr/sysinfo.cpp \
    ../spr/tu.cpp \
    ../spr/streamts.cpp \
    ../spr/dstdatafromfonitor.cpp \
    ../spr/dras.cpp \
    ../common/defines.cpp \
    ../common/archiver.cpp \
    ../common/clienttcp.cpp \
    ../common/tcpheader.cpp \
    ../spr/datafrommonitor.cpp \
    ../spr/peregon.cpp \
    ../spr/train.cpp \
    ../spr/route.cpp \
    ../forms/dlgrcinfo.cpp \
    ../forms/dlgstrlinfo.cpp \
    ../forms/dlgstationsinfo.cpp \
    ../forms/dlgtsinfo.cpp \
    ../forms/tsstatuswidget.cpp \
    ../forms/dlgkpinfo.cpp \
    ../forms/bminfoframe.cpp \
    ../spr/abtcminfo.cpp \
    ../spr/rpcdialoginfo.cpp \
    ../spr/ecmpkinfo.cpp \
    ../forms/qled.cpp \
    ../spr/krug.cpp \
    ../forms/dlgroutes.cpp \
    ../forms/dlgtuinfo.cpp \
    ../shapes/trnspdescription.cpp \
    ../shapes/shapetext.cpp \
    ../shapes/shapeprzd.cpp \
    ../shapes/shapebox.cpp \
    ../forms/dlgtrains.cpp
    ../common/boolexpression.cpp \


HEADERS  += mainwindow.h \
        shapechild.h \
    ../shapes/shape.h \
    ../common/defines.h \
    ../shapes/shapeset.h \
    ../common/logger.h \
    ../shapes/shaperc.h \
    ../shapes/shapestrl.h \
    ../shapes/shapesvtf.h \
    ../shapes/shapetrnsp.h \
    ../shapes/colorscheme.h \
    ../spr/enums.h \
    ../spr/esr.h \
    ../spr/rc.h \
    ../spr/sprbase.h \
    ../spr/station.h \
    ../spr/ts.h \
    ../common/boolexpression.h \
    ../spr/properties.h \
    ../spr/strl.h \
    ../spr/svtf.h \
    ../spr/sysinfo.h \
    ../spr/tu.h \
    ../spr/streamts.h \
    ../spr/dstdatafromfonitor.h \
    ../spr/dras.h \
    ../common/archiver.h \
    ../common/clienttcp.h \
    ../common/tcpheader.h \
    ../spr/datafrommonitor.h \
    ../spr/peregon.h \
    ../spr/train.h \
    ../spr/route.h \
    ../forms/dlgrcinfo.h \
    ../forms/dlgstrlinfo.h \
    ../forms/dlgstationsinfo.h \
    ../forms/dlgtsinfo.h \
    ../forms/tsstatuswidget.h \
    ../forms/dlgkpinfo.h \
    ../forms/bminfoframe.h \
    ../spr/abtcminfo.h \
    ../spr/rpcdialoginfo.h \
    ../spr/ecmpkinfo.h \
    ../forms/qled.h \
    ../spr/krug.h \
    ../forms/dlgroutes.h \
    ../forms/dlgtuinfo.h \
    ../shapes/palitra.h \
    ../shapes/trnspdescription.h \
    ../shapes/shapetext.h \
    ../shapes/shapeprzd.h \
    ../shapes/shapebox.h \
    ../forms/dlgtrains.h
    ../common/boolexpression.h \


FORMS    += mainwindow.ui \
    ../forms/dlgrcinfo.ui \
    ../forms/dlgstrlinfo.ui \
    ../forms/dlgstationsinfo.ui \
    ../forms/dlgtsinfo.ui \
    ../forms/dlgkpinfo.ui \
    ../forms/bminfoframe.ui \
    ../forms/dlgroutes.ui \
    ../forms/dlgtuinfo.ui \
    ../forms/dlgtrains.ui

RESOURCES += \
    images.qrc
