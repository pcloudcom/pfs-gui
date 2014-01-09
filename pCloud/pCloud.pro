#-------------------------------------------------
#
# Project created by QtCreator 2013-08-15T15:37:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pCloud
TEMPLATE = app
#TEMPLATE = lib
target.path = /usr/bin
#target.path += $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target

VERSION = 1.0.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"


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
    onlinethread.cpp \
    revnotifythread.cpp \
    #pcloudinfodialog.cpp \
    #pcloudform.cpp \
    pcloudwindow.cpp \
    settingspage.cpp \
    sharespage.cpp

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
    onlinethread.h \
    revnotifythread.h \
    #pcloudinfodialog.h \
    #pcloudform.h \
    pcloudwindow.h \
    settingspage.h \
    sharespage.h

FORMS    += reglogwindow.ui \
    registerwindow.ui \
    loginwindow.ui \
    settingswindow.ui \
    sharefolderwindow.ui \
    shareswindow.ui \
    directorypickerdialog.ui \
    changepermissionsdialog.ui \
    #pcloudinfodialog.ui \
    #pcloudform.ui \
    pcloudwindow.ui

win32:RC_FILE= pCloud.rc
RESOURCES += \
    Resources.qrc

LIBS += -lssl -lcrypto

#CONFIG += designer plugin

win32 {
    LIBS +=  -lWs2_32
    QMAKE_LFLAGS += -static-libgcc
    CONFIG += static
    WINSDK_DIR = C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A
    WIN_PWD = $$replace(PWD, /, \\)
    OUT_PWD_WIN = $$replace(OUT_PWD, /, \\)
    QMAKE_POST_LINK = "$$WINSDK_DIR/bin/x64/mt.exe -manifest $$quote($$WIN_PWD\\$$basename(TARGET).manifest) -outputresource:$$quote($$OUT_PWD_WIN\\${DESTDIR_TARGET};1)"
}

macx {
    ICON = images/mac-desktop.icns
    LIBS += -framework Cocoa
}
