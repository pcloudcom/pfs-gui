#include "pcloudapp.h"
#include "common.h"
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
    if (settingswin)
        settingswin->hide();
    if (incomingshareswin)
        incomingshareswin->hide();
    if (outgoingshareswin)
        outgoingshareswin->hide();
}

void PCloudApp::showRegLog(){
    hideAllWindows();
    if (!reglog)
        reglog=new RegLogWindow(this);
    reglog->showNormal();
}

void PCloudApp::setUser(binresult *userinfo){
    emit logInSignal(find_res(userinfo, "auth")->str, find_res(userinfo, "email")->str);
}

void PCloudApp::showWindow(QMainWindow *win)
{
    win->showNormal();
}

void PCloudApp::showRegister(){
    hideAllWindows();
    if (!regwin)
        regwin=new RegisterWindow(this);
    showWindow(regwin);
}

void PCloudApp::showLogin(){
    hideAllWindows();
    if (!logwin)
        logwin=new LoginWindow(this);
    showWindow(logwin);
}

void PCloudApp::showSettings(){
    hideAllWindows();
    if (!settingswin)
        settingswin=new SettingsWindow(this);
    showWindow(settingswin);
}

void PCloudApp::openCloudDir(){
    QDesktopServices::openUrl(QUrl::fromLocalFile(settings->get("path")));
}

void PCloudApp::shareFolder(){
    hideAllWindows();
    if (!sharefolderwin)
        sharefolderwin=new ShareFolderWindow(this);
    showWindow(sharefolderwin);
}

void PCloudApp::outgoingShares(){
    hideAllWindows();
    if (!outgoingshareswin)
        outgoingshareswin=new SharesWindow(this, 0);
    showWindow(outgoingshareswin);
}

void PCloudApp::incomingShares()
{
    hideAllWindows();
    if (!incomingshareswin)
        incomingshareswin=new SharesWindow(this, 1);
    showWindow(incomingshareswin);
}

void PCloudApp::logOut(){
    loggedin=false;
    username="";
    tray->setContextMenu(notloggedmenu);
    tray->setToolTip("pCloud");
    tray->setIcon(QIcon(OFFLINE_ICON));
    settings->unset("auth");
    unMount();
}

void PCloudApp::doExit(){
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
    settingsAction=new QAction("Settings", this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
    notloggedmenu->addAction(settingsAction);
    notloggedmenu->addSeparator();
    exitAction=new QAction("Exit", this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(doExit()));
    notloggedmenu->addAction(exitAction);

    openAction=new QAction("Open pCloud folder", this);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openCloudDir()));
    shareFolderAction=new QAction("Share folder", this);
    connect(shareFolderAction, SIGNAL(triggered()), this, SLOT(shareFolder()));
    outgoingSharesAction=new QAction("My Shares", this);
    connect(outgoingSharesAction, SIGNAL(triggered()), this, SLOT(outgoingShares()));
    incomingSharesAction=new QAction("Shared with Me", this);
    connect(incomingSharesAction, SIGNAL(triggered()), this, SLOT(incomingShares()));

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
    settingswin=NULL;
    sharefolderwin=NULL;
    incomingshareswin=NULL;
    outgoingshareswin=NULL;
    mthread=NULL;
    loggedin=false;
    createMenus();
    settings=new PSettings(this);
    tray=new QSystemTrayIcon(this);
    tray->setIcon(QIcon(OFFLINE_ICON));
    tray->setContextMenu(notloggedmenu);
    tray->setToolTip("pCloud");
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
    connect(this, SIGNAL(logInSignal(QString, QString)), this, SLOT(logIn(QString, QString)));
    tray->show();
    if (settings->isSet("auth")){
        othread=new OnlineThread(this);
        othread->start();
    }
    else
        othread=NULL;
}

PCloudApp::~PCloudApp(){
    if (othread){
        if (othread->isRunning())
            othread->terminate();
        othread->wait();
        delete othread;
    }
    if (mthread){
        if (mthread->isRunning()){
            mthread->doRun=false;
        }
        mthread->wait();
        delete mthread;
    }
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
    delete settingsAction;
    delete shareFolderAction;
    if (regwin)
        delete regwin;
    if (reglog)
        delete reglog;
    if (logwin)
        delete logwin;
    if (settingswin)
        delete settingswin;
    if (sharefolderwin)
        delete sharefolderwin;
    if (incomingshareswin)
        delete incomingshareswin;
    if (outgoingshareswin)
        delete outgoingshareswin;
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

#ifdef Q_OS_WIN

#define REGISTRY_KEY_PCLOUD    "SOFTWARE\\PCloud\\pCloud"
#define SZSERVICENAME          L"pfs"

static void storeKey(LPCSTR key, const char * val)
{
    HRESULT hr;
    HKEY hKey;
    hr = RegCreateKeyExA(HKEY_CURRENT_USER, REGISTRY_KEY_PCLOUD, 0, NULL, 0,
                        KEY_ALL_ACCESS, NULL, &hKey, NULL);
    if (!hr)
    {
        hr = RegSetValueExA(hKey, key, 0, REG_SZ, (LPBYTE)val, strlen(val)+1);
        RegCloseKey(hKey);
    }
}

static void stopService()
{
    SC_HANDLE       schService;
    SERVICE_STATUS  ssStatus;
    SC_HANDLE       schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (!schSCManager) return;
    schService = OpenService(schSCManager, SZSERVICENAME, SERVICE_ALL_ACCESS);
    if (schService)
    {
        ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus);
        int retry = 5;
        while(QueryServiceStatus(schService, &ssStatus) && retry)
        {
            if (ssStatus.dwCurrentState == SERVICE_STOPPED)
                break;
            Sleep(1000);
            --retry;
        }
        CloseServiceHandle(schService);
    }
    CloseServiceHandle(schSCManager);

}

static bool restartService(QByteArray &err)
{
    SC_HANDLE       schService;
    SERVICE_STATUS  ssStatus;
    SC_HANDLE       schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (!schSCManager) return false;
    schService = OpenService(schSCManager, SZSERVICENAME, SERVICE_ALL_ACCESS);
    if (schService)
    {
        ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus);
        int retry = 5;
        while(QueryServiceStatus(schService, &ssStatus) && retry)
        {
            if (ssStatus.dwCurrentState == SERVICE_STOPPED)
                break;
            Sleep(1000);
            --retry;
        }
        if (!retry){
            err = "Failed to stop PCloud fs.";
        }

        if (StartService(schService, 0, NULL))
        {
            Sleep(1000);
            int retry = 5;
            while(QueryServiceStatus(schService, &ssStatus) && retry)
            {
                --retry;
                if (ssStatus.dwCurrentState == SERVICE_START_PENDING)
                    Sleep(1000);
                else break;
            }
            if (!retry){
                err = "Failed to start PCloud fs.";
                return false;
            }
            else{
                err = "";
                return true;
            }
        }
        CloseServiceHandle(schService);
    }
    CloseServiceHandle(schSCManager);
    return false;
}
#endif



void PCloudApp::mount()
{
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

void PCloudApp::unMount(){
    if (mthread){
        mthread->terminate();
        delete mthread;
        mthread=NULL;
    }
#ifdef Q_OS_WIN
    stopService();
#else
    QProcess process;
    QString path=settings->get("path");
    process.start("umount", QStringList() << "-f" << path);
    if (process.waitForFinished() && process.exitCode()==0)
        return;
    process.start("fusermount", QStringList() << "-u" << path);
    if (process.waitForFinished() && process.exitCode()==0)
        return;
    process.start("fusermount", QStringList() << "-z" << "-u" << path);
    if (process.waitForFinished() && process.exitCode()==0)
        return;
#endif
}

void PCloudApp::showError(QString err){
    tray->showMessage("Error", err, QSystemTrayIcon::Warning);
}

void PCloudApp::showTrayMessage(QString title, QString msg)
{
    tray->showMessage(title, msg, QSystemTrayIcon::Information);
}

void PCloudApp::logIn(QString auth, QString uname)
{
    settings->set("auth", auth);
    loggedin=true;
    username=uname;
    tray->setToolTip(username);
    if (loggedmenu)
        delete loggedmenu;
    loggedmenu=new QMenu();
    loggedmenu->addAction(username);
    loggedmenu->addAction(openAction);
    loggedmenu->addSeparator();
    loggedmenu->addAction(shareFolderAction);
    loggedmenu->addAction(outgoingSharesAction);
    loggedmenu->addAction(incomingSharesAction);
    loggedmenu->addSeparator();
    loggedmenu->addAction(settingsAction);
    loggedmenu->addSeparator();
    loggedmenu->addAction(logoutAction);
    loggedmenu->addAction(exitAction);
    tray->setIcon(QIcon(REGULAR_ICON));
    tray->setContextMenu(loggedmenu);
    if (!mthread){
        mthread=new MonitoringThread(this);
        mthread->start();
    }
}

bool PCloudApp::userLogged(binresult *userinfo, QByteArray &err){
    if (isMounted()){
        setUser(userinfo);
        return true;
    }
    else{
#ifdef Q_OS_WIN
        if (find_res(userinfo, "auth")){
            storeKey("auth", find_res(userinfo, "auth")->str);
            return restartService(err);
        }
        else {
            err = "User not logged in.";
            return false;
        }
#else
        QProcess process;
        QStringList params;
        params.append("--auth");
        params.append(find_res(userinfo, "auth")->str);
        if (settings->geti("usessl"))
            params.append("--ssl");
        params.append("--cache");
        params.append(settings->get("cachesize"));
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
#endif
    }
}
