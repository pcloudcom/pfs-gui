#-------------------------------------------------
#
# Project created by QtCreator 2013-08-15T15:37:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pCloud
TEMPLATE = app


SOURCES += main.cpp\
        reglogwindow.cpp \
    pcloudapp.cpp \
    registerwindow.cpp

HEADERS  += reglogwindow.h \
    pcloudapp.h \
    registerwindow.h

FORMS    += reglogwindow.ui \
    registerwindow.ui

RESOURCES += \
    Resources.qrc
