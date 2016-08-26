#-------------------------------------------------
#
# Project created by QtCreator 2016-03-17T17:20:44
#
#-------------------------------------------------

QT       += core gui widgets sql
CONFIG   += console c++11
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = armtools
TEMPLATE = app
INCLUDEPATH += ../forms/

SOURCES += main.cpp\
        mainwindow.cpp \
        shapechild.cpp \
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
    ../shapes/shape.cpp \
    ../shapes/shapeset.cpp \
    ../shapes/shaperc.cpp \
    ../shapes/shapestrl.cpp \
    ../shapes/shapesvtf.cpp \
    ../shapes/shapetrnsp.cpp \
    ../shapes/colorscheme.cpp \
    ../shapes/trnspdescription.cpp \
    ../shapes/shapetext.cpp \
    ../shapes/shapeprzd.cpp \
    ../shapes/shapebox.cpp \
    ../shapes/shapetrain.cpp \
    ../forms/dlgrcinfo.cpp \
    ../forms/dlgstrlinfo.cpp \
    ../forms/dlgstationsinfo.cpp \
    ../forms/dlgtsinfo.cpp \
    ../forms/tsstatuswidget.cpp \
    ../forms/dlgkpinfo.cpp \
    ../forms/bminfoframe.cpp \
    ../forms/qled.cpp \
    ../forms/dlgroutes.cpp \
    ../forms/dlgtuinfo.cpp \
    ../forms/dlgtrains.cpp \
    ../forms/dlgsvtfinfo.cpp \
    ../spr/otu.cpp \
    ../forms/dlgotu.cpp


HEADERS  += mainwindow.h \
    shapechild.h \
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
    ../shapes/shape.h \
    ../shapes/shapeset.h \
    ../shapes/shaperc.h \
    ../shapes/shapestrl.h \
    ../shapes/shapesvtf.h \
    ../shapes/shapetrnsp.h \
    ../shapes/colorscheme.h \
    ../shapes/palitra.h \
    ../shapes/trnspdescription.h \
    ../shapes/shapetext.h \
    ../shapes/shapeprzd.h \
    ../shapes/shapebox.h \
    ../forms/dlgtrains.h \
    ../shapes/shapetrain.h \
    ../forms/dlgroutes.h \
    ../forms/dlgtuinfo.h \
    ../forms/dlgrcinfo.h \
    ../forms/dlgstrlinfo.h \
    ../forms/dlgstationsinfo.h \
    ../forms/dlgtsinfo.h \
    ../forms/tsstatuswidget.h \
    ../forms/dlgkpinfo.h \
    ../forms/bminfoframe.h \
    ../forms/qled.h \
    ../forms/dlgsvtfinfo.h \
    ../spr/otu.h \
    ../forms/dlgotu.h


FORMS    += mainwindow.ui \
    ../forms/dlgrcinfo.ui \
    ../forms/dlgstrlinfo.ui \
    ../forms/dlgstationsinfo.ui \
    ../forms/dlgtsinfo.ui \
    ../forms/dlgkpinfo.ui \
    ../forms/bminfoframe.ui \
    ../forms/dlgroutes.ui \
    ../forms/dlgtuinfo.ui \
    ../forms/dlgtrains.ui \
    ../forms/dlgsvtfinfo.ui \
    ../forms/dlgotu.ui

RESOURCES += \
    images.qrc

DISTFILES += \
    armtools.ini \
    history.txt \
    ../todo.txt
