#-------------------------------------------------
#
# Project created by QtCreator 2014-12-22T11:51:42
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES    += QT_NO_DEBUG_OUTPUT

TARGET = LKV373_RxTest
TEMPLATE = app

INCLUDEPATH += ../LKV373
LIBS += -L../LKV373 -lLKV373

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
