#-------------------------------------------------
#
# Project created by QtCreator 2016-11-29T09:21:12
#
#-------------------------------------------------

QT       += core gui network widgets sql serialport
CONFIG   += console c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = monitor
TEMPLATE = app
INCLUDEPATH += ../forms/

SOURCES += main.cpp\
        mainwindow.cpp\
    ../common/archiver.cpp \
    ../common/blockingrs.cpp \
    ../common/boolexpression.cpp \
    ../common/clienttcp.cpp \
    ../common/defines.cpp \
    ../common/inireader.cpp \
    ../common/logger.cpp \
    ../common/tcpheader.cpp \
    ../spr/abtcminfo.cpp \
    ../spr/datafrommonitor.cpp \
    ../spr/dras.cpp \
    ../spr/dstdatafromfonitor.cpp \
    ../spr/ecmpkinfo.cpp \
    ../spr/esr.cpp \
    ../spr/krug.cpp \
    ../spr/otu.cpp \
    ../spr/peregon.cpp \
    ../spr/pereezd.cpp \
    ../spr/properties.cpp \
    ../spr/rc.cpp \
    ../spr/route.cpp \
    ../spr/rpcdialoginfo.cpp \
    ../spr/sprbase.cpp \
    ../spr/station.cpp \
    ../spr/streamts.cpp \
    ../spr/strl.cpp \
    ../spr/sysinfo.cpp \
    ../spr/svtf.cpp \
    ../spr/ts.cpp \
    ../spr/tu.cpp \
    ../spr/train.cpp \
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
    ../forms/bminfoframe.cpp \
    ../forms/dlgkpinfo.cpp \
    ../forms/dlgotu.cpp \
    ../forms/dlgpereezd.cpp \
    ../forms/dlgperegoninfo.cpp \
    ../forms/dlgrcinfo.cpp \
    ../forms/dlgroutes.cpp \
    ../forms/dlgstationsinfo.cpp \
    ../forms/dlgstrlinfo.cpp \
    ../forms/dlgsvtfinfo.cpp \
    ../forms/dlgtrains.cpp \
    ../forms/dlgtsinfo.cpp \
    ../forms/dlgtuinfo.cpp \
    ../forms/qled.cpp \
    ../forms/tsstatuswidget.cpp \
    ../armtools/shapechild.cpp

HEADERS  += mainwindow.h\
    ../common/archiver.h \
    ../common/blockingrs.h \
    ../common/boolexpression.h \
    ../common/clienttcp.h \
    ../common/defines.h \
    ../common/logger.h \
    ../common/inireader.h \
    ../common/tcpheader.h \
    ../spr/abtcminfo.h \
    ../spr/datafrommonitor.h \
    ../spr/dras.h \
    ../spr/dstdatafromfonitor.h \
    ../spr/ecmpkinfo.h \
    ../spr/enums.h \
    ../spr/esr.h \
    ../spr/krug.h \
    ../spr/otu.h \
    ../spr/peregon.h \
    ../spr/pereezd.h \
    ../spr/properties.h \
    ../spr/rc.h \
    ../spr/route.h \
    ../spr/rpcdialoginfo.h \
    ../spr/sprbase.h \
    ../spr/station.h \
    ../spr/streamts.h \
    ../spr/strl.h \
    ../spr/svtf.h \
    ../spr/sysinfo.h \
    ../spr/ts.h \
    ../spr/tu.h \
    ../spr/train.h \
    ../spr/pereezd.h\
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
    ../forms/bminfoframe.h \
    ../forms/dlgkpinfo.h \
    ../forms/dlgotu.h \
    ../forms/dlgpereezd.h \
    ../forms/dlgperegoninfo.h \
    ../forms/dlgrcinfo.h \
    ../forms/dlgroutes.h \
    ../forms/dlgstationsinfo.h \
    ../forms/dlgstrlinfo.h \
    ../forms/dlgsvtfinfo.h \
    ../forms/dlgtrains.h \
    ../forms/dlgtsinfo.h \
    ../forms/dlgtuinfo.h \
    ../forms/qled.h \
    ../forms/tsstatuswidget.h \
    ../armtools/shapechild.h


FORMS    += mainwindow.ui\
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
    ../forms/dlgotu.ui \
    ../forms/dlgperegoninfo.ui \
    ../forms/dlgpereezd.ui

RESOURCES += \
    images.qrc

DISTFILES += \
    Monitor.ini
