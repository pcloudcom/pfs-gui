#include "pcloudapp.h"
#include <QMenu>

void PCloudApp::doExit(){
    quit();
}

void PCloudApp::hideAllWindows(){
    if (reglog)
        reglog->hide();
    if (regwin)
        regwin->hide();
}

void PCloudApp::showRegLog(){
    hideAllWindows();
    if (!reglog)
        reglog=new RegLogWindow(this);
    reglog->showNormal();
}

void PCloudApp::showRegister(){
    hideAllWindows();
    if (!regwin)
        regwin=new RegisterWindow();
    regwin->showNormal();
}

void PCloudApp::trayClicked(QSystemTrayIcon::ActivationReason reason){
    if (reason==QSystemTrayIcon::Trigger){
        showRegLog();
    }
}

void PCloudApp::createMenus(){
    notloggedmenu=new QMenu();
    registerAction=new QAction("Register", this);
    connect(registerAction, SIGNAL(triggered()), this, SLOT(showRegister()));
    notloggedmenu->addAction(registerAction);
    exitAction=new QAction("Exit", this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(doExit()));
    notloggedmenu->addAction(exitAction);
}

PCloudApp::PCloudApp(int &argc, char **argv) :
    QApplication(argc, argv)
{
    reglog=NULL;
    regwin=NULL;
    createMenus();
    tray=new QSystemTrayIcon(this);
    tray->setIcon(QIcon(":/images/images/icon_pcloud.png"));
    tray->setContextMenu(notloggedmenu);
    tray->setToolTip("pCloud");
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
    tray->show();
}

PCloudApp::~PCloudApp(){
    delete tray;
    delete notloggedmenu;
    delete registerAction;
    delete exitAction;
    if (regwin)
        delete regwin;
    if (reglog)
        delete reglog;
}
