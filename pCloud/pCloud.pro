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
    registerwindow.cpp \
    loginwindow.cpp \
    binapi.c \
    psettings.cpp \
    settingswindow.cpp \
    sharefolderwindow.cpp \
    shareswindow.cpp \
    directorypickerdialog.cpp \
    changepermissionsdialog.cpp \
    monitoringthread.cpp \
    onlinethread.cpp

HEADERS  += reglogwindow.h \
    pcloudapp.h \
    registerwindow.h \
    loginwindow.h \
    binapi.h \
    sockets.h \
    psettings.h \
    settingswindow.h \
    common.h \
    sharefolderwindow.h \
    shareswindow.h \
    directorypickerdialog.h \
    changepermissionsdialog.h \
    monitoringthread.h \
    onlinethread.h

FORMS    += reglogwindow.ui \
    registerwindow.ui \
    loginwindow.ui \
    settingswindow.ui \
    sharefolderwindow.ui \
    shareswindow.ui \
    directorypickerdialog.ui \
    changepermissionsdialog.ui

RESOURCES += \
    Resources.qrc

LIBS += -lssl -lcrypto

win32 {
LIBS +=  -lWs2_32
QMAKE_LFLAGS += -static-libgcc
CONFIG += static
}
