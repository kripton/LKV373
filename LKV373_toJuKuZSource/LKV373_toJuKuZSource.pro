#-------------------------------------------------
#
# Project created by QtCreator 2014-12-22T11:51:42
#
#-------------------------------------------------

QT       += core network gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#DEFINES    += QT_NO_DEBUG_OUTPUT

TARGET = LKV373_toJuKuZSource
TEMPLATE = app

INCLUDEPATH += ../LKV373
LIBS += -L../LKV373 -lLKV373

SOURCES += main.cpp \
    worker.cpp \
    lkv373videoappsrc.cpp

HEADERS  += \
    worker.h \
    lkv373videoappsrc.h

# Tell qmake to use pkg-config to find QtGStreamer
CONFIG += link_pkgconfig

# Now tell qmake to link to QtGStreamer and also use its include path and Cflags.
contains(QT_VERSION, ^4\\..*) {
  PKGCONFIG += QtGStreamer-1.0 QtGStreamerUtils-1.0
}
contains(QT_VERSION, ^5\\..*) {
  PKGCONFIG += Qt5GStreamer-1.0 Qt5GStreamerUtils-1.0
}
