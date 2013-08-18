#include "pcloudapp.h"
#include <QMenu>
#include <QProcess>

void PCloudApp::hideAllWindows(){
    if (reglog)
        reglog->hide();
    if (regwin)
        regwin->hide();
    if (logwin)
        logwin->hide();
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

void PCloudApp::showLogin(){
    hideAllWindows();
    if (!logwin)
        logwin=new LoginWindow(this);
    logwin->showNormal();
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
    loginAction=new QAction("Login", this);
    connect(loginAction, SIGNAL(triggered()), this, SLOT(showLogin()));
    notloggedmenu->addAction(loginAction);
    notloggedmenu->addSeparator();
    exitAction=new QAction("Exit", this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(quit()));
    notloggedmenu->addAction(exitAction);
}

PCloudApp::PCloudApp(int &argc, char **argv) :
    QApplication(argc, argv)
{
    reglog=NULL;
    regwin=NULL;
    logwin=NULL;
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
    delete loginAction;
    delete exitAction;
    if (regwin)
        delete regwin;
    if (reglog)
        delete reglog;
    if (logwin)
        delete logwin;
}

bool PCloudApp::UserLogged(binresult *userinfo, QByteArray &err){
    QProcess process;
    process.start("mount.pfs", QStringList() << "--auth" << find_res(userinfo, "auth")->str << "/home/virco/pfs");
    process.waitForFinished();
    if (process.exitCode()==0)
      return true;
    err=process.readAllStandardError();
    return false;
}
