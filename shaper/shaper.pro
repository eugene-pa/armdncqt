#-------------------------------------------------
#
# Project created by QtCreator 2015-04-23T10:59:57
#
#-------------------------------------------------

QT       += core gui widgets sql
CONFIG   += console c++11


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = shaper
TEMPLATE = app


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
    ../common/archiver.cpp
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
    ../common/archiver.h
    ../common/boolexpression.h \


FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc
