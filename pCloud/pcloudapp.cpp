#include "pcloudapp.h"
#include <QMenu>
#include <QProcess>
#include <QUrl>
#include <QDir>
#include <QDesktopServices>

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

void PCloudApp::setUser(binresult *userinfo){
    settings->set("auth", find_res(userinfo, "auth")->str);
    loggedin=true;
    username=find_res(userinfo, "email")->str;
    tray->setToolTip(username);
    if (loggedmenu)
        delete loggedmenu;
    loggedmenu=new QMenu();
    loggedmenu->addAction(username);
    loggedmenu->addAction(openAction);
    loggedmenu->addSeparator();
    loggedmenu->addAction(logoutAction);
    loggedmenu->addAction(exitAction);
    tray->setContextMenu(loggedmenu);
}

void PCloudApp::showRegister(){
    hideAllWindows();
    if (!regwin)
        regwin=new RegisterWindow(this);
    regwin->showNormal();
}

void PCloudApp::showLogin(){
    hideAllWindows();
    if (!logwin)
        logwin=new LoginWindow(this);
    logwin->showNormal();
}

void PCloudApp::openCloudDir(){
    QDesktopServices::openUrl(QUrl::fromLocalFile(settings->get("path")));
}

void PCloudApp::logOut(){
    loggedin=false;
    username="";
    tray->setContextMenu(notloggedmenu);
    tray->setToolTip("pCloud");
    settings->unset("auth");
    if (isMounted())
        unMount();
}

void PCloudApp::doExit(){
    if (isMounted())
        unMount();
    quit();
}

void PCloudApp::trayClicked(QSystemTrayIcon::ActivationReason reason){
    if (reason==QSystemTrayIcon::Trigger){
        if (loggedin)
            openCloudDir();
        else
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
    connect(exitAction, SIGNAL(triggered()), this, SLOT(doExit()));
    notloggedmenu->addAction(exitAction);

    openAction=new QAction("Open pCloud folder", this);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openCloudDir()));
    logoutAction=new QAction("Logout", this);
    connect(logoutAction, SIGNAL(triggered()), this, SLOT(logOut()));
}

PCloudApp::PCloudApp(int &argc, char **argv) :
    QApplication(argc, argv)
{
    reglog=NULL;
    regwin=NULL;
    logwin=NULL;
    loggedmenu=NULL;
    loggedin=false;
    createMenus();
    settings=new PSettings(this);
    tray=new QSystemTrayIcon(this);
    tray->setIcon(QIcon(":/images/images/icon_pcloud.png"));
    tray->setContextMenu(notloggedmenu);
    tray->setToolTip("pCloud");
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
    tray->show();
    if (settings->isSet("auth")){
        QByteArray auth=settings->get("auth").toUtf8();
        apisock *conn=getAPISock();
        binresult *res, *result;
        QByteArray err;
        if (!conn)
            return;
        res=send_command(conn, "userinfo",
                         P_LSTR("auth", auth.constData(), auth.size()),
                         P_BOOL("getauth", 1));
        api_close(conn);
        result=find_res(res, "result");
        if (!result){
            free(res);
            return;
        }
        if (result->num!=0){
            free(res);
            return;
        }
        userLogged(res, err);
        free(res);
    }
}

PCloudApp::~PCloudApp(){
    delete settings;
    delete tray;
    if (loggedmenu)
        delete loggedmenu;
    delete notloggedmenu;
    delete registerAction;
    delete loginAction;
    delete exitAction;
    delete logoutAction;
    delete openAction;
    if (regwin)
        delete regwin;
    if (reglog)
        delete reglog;
    if (logwin)
        delete logwin;
}

apisock *PCloudApp::getAPISock(){
    if (settings->geti("usessl"))
        return api_connect_ssl();
    else
        return api_connect();
}

bool PCloudApp::isMounted(){
    QDir dir(settings->get("path")+"/.pfs_settings");
    return dir.exists();
}

void PCloudApp::unMount(){
    QProcess process;
    QString path=settings->get("path");
    process.start("umount", QStringList() << "-l" << path);
    if (process.waitForFinished() && process.exitCode()==0)
        return;
    process.start("fusermount", QStringList() << "-z" << "-u" << path);
    if (process.waitForFinished() && process.exitCode()==0)
        return;
}

bool PCloudApp::userLogged(binresult *userinfo, QByteArray &err){
    if (isMounted()){
        setUser(userinfo);
        return true;
    }
    else{
        QProcess process;
        QStringList params;
        params.append("--auth");
        params.append(find_res(userinfo, "auth")->str);
        if (settings->geti("usessl"))
            params.append("--ssl");
        params.append(settings->get("path"));
        process.start("mount.pfs", params);
        if (!process.waitForFinished()){
            err="Error mounting filesystem.";
            return false;
        }
        if (process.exitCode()==0){
            setUser(userinfo);
            return true;
        }
        else {
            err=process.readAllStandardError();
            return false;
        }
    }
}
