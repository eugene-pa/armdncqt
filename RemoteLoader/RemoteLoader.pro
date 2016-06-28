#-------------------------------------------------
#
# Project created by QtCreator 2016-06-27T15:20:16
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RemoteLoader
TEMPLATE = app


SOURCES +=dialog.cpp \
    ../common/clienttcp.cpp \
    ../paServer/remoterq.cpp \
    ../paServer/responceabout.cpp \
    ../paServer/responcedirs.cpp \
    ../paServer/responcedrives.cpp \
    ../paServer/responceerror.cpp \
    ../paServer/responcefileinfo.cpp \
    ../paServer/responcefiles.cpp \
    ../paServer/responceread.cpp \
    ../paServer/responcetempfile.cpp \
    ../common/tcpheader.cpp \
    ../common/defines.cpp \
    ../common/logger.cpp \
    ../paServer/headerresponce.cpp \
    ../paServer/brieffileinfo.cpp \
    mainbody.cpp \
    ../forms/qled.cpp

HEADERS  += dialog.h \
    ../common/clienttcp.h \
    ../paServer/remoterq.h \
    ../paServer/responceabout.h \
    ../paServer/responcedirs.h \
    ../paServer/responcedrives.h \
    ../paServer/responceerror.h \
    ../paServer/responcefileinfo.h \
    ../paServer/responcefiles.h \
    ../paServer/responceread.h \
    ../paServer/responcetempfile.h \
    ../common/tcpheader.h \
    ../common/defines.h \
    ../common/logger.h \
    ../paServer/headerresponce.h \
    ../paServer/brieffileinfo.h \
    ../forms/qled.h

FORMS    += dialog.ui
