#-------------------------------------------------
#
# Project created by QtCreator 2014-12-22T11:48:20
#
#-------------------------------------------------

QT       += network gui

TARGET = LKV373
TEMPLATE = lib

DEFINES += LKV373_LIBRARY

SOURCES += lkv373.cpp \
    datareceiver.cpp \
    hdmireceiver.cpp

HEADERS += lkv373.h\
        lkv373_global.h \
    datareceiver.h \
    hdmireceiver.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
