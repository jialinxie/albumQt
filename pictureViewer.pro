#-------------------------------------------------
#
# Project created by QtCreator 2018-07-25T12:10:15
#
#-------------------------------------------------

QT       += core gui
VERSION = "v0.2.1"
#DEFINES += EY_Version
CONFIG(QT_PC):DEFINES += QT_PC
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = album
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    albumwidget.cpp \
    mainwidget.cpp \
    readimgthread.cpp

HEADERS += \
    albumwidget.h \
    mainwidget.h \
    readimgthread.h

CONFIG += console

RESOURCES += \
    resource.qrc
