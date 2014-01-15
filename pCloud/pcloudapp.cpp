#include "pcloudapp.h"
#include "common.h"
#include <QMenu>
#include <QProcess>
#include <QUrl>
#include <QDir>
#include <QDesktopServices>
#include <QDebug> //temp

#ifdef Q_OS_MAC
#include <objc/objc.h>
#include <objc/message.h>
#endif


void PCloudApp::hideAllWindows(){
    if (reglog)
        reglog->hide();
    if (regwin)
        regwin->hide();
    if (logwin)
        logwin->hide();
    if (settingswin) // to del
        settingswin->hide();
    if (incomingshareswin)
        incomingshareswin->hide();
    if (outgoingshareswin)
        outgoingshareswin->hide();
    if (pCloudWin)
        pCloudWin->hide();
}

void PCloudApp::setUser(binresult *userinfo, bool rememebr){    
    emit logInSignal(find_res(userinfo, "auth")->str, find_res(userinfo, "email")->str, (quint64)find_res(userinfo, "userid")->num,
                     find_res(userinfo, "emailverified")->num, find_res(userinfo, "premium")->num,
                     (quint64)find_res(userinfo, "quota")->num,  (quint64)find_res(userinfo, "usedquota")->num,
                     rememebr);
    /*  emitting signal should be enough
 *  it seems that it is very important to have q-type parameters to this one
 *
    QString auth = find_res(userinfo, "auth")->str;
    QString email = find_res(userinfo, "email")->str;
    uint64_t uid = find_res(userinfo, "userid")->num;
    emit logInSignal(auth, email, uid, remember);
    logIn(auth, email, uid, remember);
    */
}

void PCloudApp::showWindow(QMainWindow *win)
{
    win->raise();
    win->activateWindow();
    win->showNormal();
    win->setWindowState(Qt::WindowActive);
    this->setActiveWindow(win);
}

void PCloudApp::showRegLog(){
    hideAllWindows();
    if (!reglog)
        reglog=new RegLogWindow(this);
    showWindow(reglog);
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
void PCloudApp::showAccount(){
    hideAllWindows();
    pCloudWin->showpcloudWindow(1);
}

void PCloudApp::showShares()
{
    hideAllWindows();
    pCloudWin->showpcloudWindow(2);
}

void PCloudApp::showSettings(){
    hideAllWindows();
    // if (!settingswin)
    // settingswin=new SettingsWindow(this);
    //showWindow(settingswin);
    pCloudWin->showpcloudWindow(4);

}

void PCloudApp::showpcloudHelp()
{
    pCloudWin->showpcloudWindow(5);
}

void PCloudApp::showpCloudAbout(){
    hideAllWindows();
    pCloudWin->showpcloudWindow(6);
}

void PCloudApp::openpCloudWindow(){
    hideAllWindows();
}


void PCloudApp::openCloudDir(){
    QString path = settings->get("path");

#ifdef Q_OS_WIN
    int retray = 5;
    char drive = path.toUtf8().at(0);
    if (drive >= 'A' && drive <= 'Z')
        drive -= 'A';
    else if (drive >= 'a' && drive <= 'z')
        drive -= 'a';
    else return;

    while (retray-- && !isConnected(drive)){
        Sleep(1000);
    }

    if (!QProcess::startDetached("explorer.exe", QStringList(path))){
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }

#else
    if (isMounted()){
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
#endif
}

void PCloudApp::shareFolder(){
    //  hideAllWindows();
    if (!sharefolderwin)
        sharefolderwin=new ShareFolderWindow(this);
    showWindow(sharefolderwin);
}

void PCloudApp::outgoingShares(){ // to del
    hideAllWindows();
    if (!outgoingshareswin)
        outgoingshareswin=new SharesWindow(this, 0);
    showWindow(outgoingshareswin);
}

void PCloudApp::incomingShares() // to del
{
    hideAllWindows();
    if (!incomingshareswin)
        incomingshareswin=new SharesWindow(this, 1);
    showWindow(incomingshareswin);
}

void PCloudApp::logOut(){
    loggedin=false;
#ifdef Q_OS_WIN
    if (notifythread){
        notifythread->terminate();
        notifythread->wait();
        delete notifythread;
        notifythread = NULL;
    }
#endif
    username="";
    userid=0;
    tray->setContextMenu(notloggedmenu);
    tray->setToolTip("pCloud");
    tray->setIcon(QIcon(OFFLINE_ICON));
    pCloudWin->setOnlineItems(false);
    pCloudWin->hide();
    settings->unset("auth");
    this->authentication = "";
    unMount();
}


void PCloudApp::doExit(){
    unMount();
    quit();
}

void PCloudApp::showOnClick(){
    if (loggedin)
        openCloudDir();
    else
        showLogin();
}

void PCloudApp::trayClicked(QSystemTrayIcon::ActivationReason reason){
    if (reason==QSystemTrayIcon::Trigger)
        showOnClick();
}

void PCloudApp::createMenus(){
    notloggedmenu=new QMenu();

    registerAction=new QAction(tr ("Register"), this);
    connect(registerAction, SIGNAL(triggered()), this, SLOT(showRegister()));
    loginAction=new QAction(tr("Login"), this);
    connect(loginAction, SIGNAL(triggered()), this, SLOT(showLogin()));
    settingsAction=new QAction(tr("Settings"), this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
    helpAction = new QAction(tr("Help"),this);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(showpcloudHelp()));
    aboutPCloudAction = new QAction(tr("About"), this);
    connect(aboutPCloudAction, SIGNAL(triggered()), this, SLOT(showpCloudAbout()));
    exitAction=new QAction(tr("Exit"), this); // to be hidden in account tab
    connect(exitAction, SIGNAL(triggered()), this, SLOT(doExit()));

    notloggedmenu->addAction(registerAction);
    notloggedmenu->addAction(loginAction);
    notloggedmenu->addAction(settingsAction);
    notloggedmenu->addAction(helpAction);
    notloggedmenu->addAction(aboutPCloudAction);
    notloggedmenu->addSeparator();
    notloggedmenu->addAction(exitAction); // to be hidden in account tab or settings


    accountAction = new QAction(tr("Account"), this); // Account tab // to remove username from menu
    connect(accountAction, SIGNAL(triggered()),this, SLOT(showAccount()));
    openAction=new QAction("Open pCloud folder", this);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openCloudDir()));
    //shareFolderAction=new QAction("Share folder", this); //to del
    //connect(shareFolderAction, SIGNAL(triggered()), this, SLOT(shareFolder()));
    outgoingSharesAction=new QAction("My Shares", this); //to del
    connect(outgoingSharesAction, SIGNAL(triggered()), this, SLOT(outgoingShares()));
    incomingSharesAction=new QAction("Shared with Me", this); //to del
    connect(incomingSharesAction, SIGNAL(triggered()), this, SLOT(incomingShares()));

    sharesAction = new QAction(tr("Shares"),this);
    connect(sharesAction, SIGNAL(triggered()), this, SLOT(showShares()));


    logoutAction=new QAction("Logout", this);
    connect(logoutAction, SIGNAL(triggered()), this, SLOT(logOut()));

    openPCloudWinAction = new QAction("Open PCloud Window", this); // to del
    connect(openPCloudWinAction, SIGNAL(triggered()), this, SLOT(openpCloudWindow()));

    loggedmenu = new QMenu();
    loggedmenu->addAction(openAction);
    loggedmenu->addAction(accountAction);
    // loggedmenu->addSeparator();
    //loggedmenu->addAction(shareFolderAction); // to del
    // loggedmenu->addAction(outgoingSharesAction);
    //loggedmenu->addAction(incomingSharesAction);
    loggedmenu->addAction(sharesAction);
    loggedmenu->addAction(settingsAction);
    loggedmenu->addAction(helpAction);
    loggedmenu->addAction(aboutPCloudAction);
    loggedmenu->addSeparator();
    loggedmenu->addAction(logoutAction); // to hide in acc tab
    loggedmenu->addAction(exitAction);
    // loggedmenu->addAction(openpCloudFormAction); // first form

}

#ifdef Q_OS_MAC

bool dockClickHandler(id self,SEL _cmd,...)
{
    Q_UNUSED(self)
    Q_UNUSED(_cmd)
    ((PCloudApp*)qApp)->showOnClick();
    return true;
}

#endif

PCloudApp::PCloudApp(int &argc, char **argv) :
    QApplication(argc, argv)
{
#ifdef Q_OS_MAC

    objc_class * cls = objc_getClass("NSApplication");
    SEL sharedApplication = sel_registerName("sharedApplication");
    objc_object* appInst = objc_msgSend((objc_object*) cls,sharedApplication);

    if(appInst != NULL)
    {
        objc_object* delegate = objc_msgSend(appInst, sel_registerName("delegate"));
        objc_object* delClass = objc_msgSend(delegate,  sel_registerName("class"));
        class_addMethod((Class)delClass,
                        sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:"),
                        (IMP)dockClickHandler,"B@:");
    }

#endif

#ifdef Q_OS_WIN
    notifythread = NULL;
#endif

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
    lastMessageType=-1;
    createMenus();
    settings=new PSettings(this);
    tray=new QSystemTrayIcon(this);
    tray->setIcon(QIcon(OFFLINE_ICON));
    tray->setContextMenu(notloggedmenu);
    tray->setToolTip("pCloud");
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
    connect(tray, SIGNAL(messageClicked()), this, SLOT(trayMsgClicked()));
    connect(this, SIGNAL(logInSignal(QString, QString, quint64, bool, bool, quint64, quint64, bool)),
            this, SLOT(logIn(QString, QString, quint64,bool, bool, quint64, quint64, bool)));
    connect(this, SIGNAL(showLoginSignal()), this, SLOT(showLogin()));
    pCloudWin = new PCloudWindow(this);
    pCloudWin->layout()->setSizeConstraint(QLayout::SetFixedSize); //for auto resize
    pCloudWin->setOnlineItems(false);
    tray->show();
    if (settings->isSet("auth") && settings->get("auth").length() > 0){
        this->authentication = settings->get("auth");
        othread=new OnlineThread(this);
        othread->start();
    }
    else{
        othread=NULL;
        emit showLoginSignal();
    }
}

PCloudApp::~PCloudApp(){
    if (othread){
        if (othread->isRunning())
            othread->terminate();
        othread->wait();
        delete othread;
    }
    if (mthread){
        if (mthread->isRunning())
            mthread->terminate();
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
    delete sharesAction;
    delete helpAction;
    //delete shareFolderAction;
    delete aboutPCloudAction;
    delete openPCloudWinAction;
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
    if (pCloudWin)
        delete pCloudWin;
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

#define OPT_MASK_COMMAND    0x00FF0000
#define OPT_MASK_OPTS       0x0000FF00
#define OPT_MASK_LETTER     0x000000FF

#define OPT_USE_SSL         0x00000100
#define OPT_COMMAND_MOUNT   0x00010000
#define OPT_COMMAND_UMOUNT  0x00000000

#define  PIPE_NAME L"\\\\.\\pipe\\pfsservicepipe"

typedef struct
{
    uint32_t options; // mount / umount, ssl, mount letter
    uint32_t cache;
    char auth[120];
}mount_params;


static bool send_command_pipe(const mount_params* params, QByteArray &err)
{
    DWORD  mode, written, total = 0;
    const wchar_t* pipename = PIPE_NAME;
    HANDLE hPipe = CreateFile(pipename, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    while (hPipe == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() != ERROR_PIPE_BUSY)
        {
            err = "Unable to connect with mount service.";
            return false;
        }
        if (!WaitNamedPipe(PIPE_NAME, 2000))
        {
            err = "Unable to comunicate with mount service.";
            return false;
        }
        hPipe = CreateFile(pipename, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    }

    mode = PIPE_READMODE_BYTE;
    if (SetNamedPipeHandleState(hPipe, &mode,  NULL, NULL))
    {
        while (total < sizeof(mount_params))
        {
            if (!WriteFile(hPipe, (void*)((char*)params+total), sizeof(mount_params)-total, &written, NULL))
            {
                err = "Communication with mount service is broken.";
                break;
            }
            total += written;
        }
        FlushFileBuffers(hPipe);
    }

    CloseHandle(hPipe);

    if (total != sizeof(mount_params))
    {
        err = "Communication with mount service is incomplete.";
        return false;
    }

    err = "";
    return true;
}

static void unmountCmd(QByteArray &err)
{
    mount_params params = {0, 0, ""};
    send_command_pipe(&params, err);
}


static bool mountCmd(size_t cache, const char* auth, int useSSL, char mountPoint, QByteArray &err)
{
    if (mountPoint >= 'a' && mountPoint <= 'z') mountPoint -= 'a';
    else if (mountPoint >= 'A' && mountPoint <= 'Z') mountPoint -= 'A';
    else mountPoint = 2; // C:

    mount_params params;
    params.options = OPT_COMMAND_MOUNT | (useSSL?OPT_USE_SSL:0) | mountPoint;
    params.cache = cache;
    strncpy(params.auth, auth, sizeof(params.auth));

    return send_command_pipe(&params, err);
}
#endif



void PCloudApp::mount()
{
    if (this->authentication != ""){
        QByteArray auth=this->authentication.toUtf8();
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
        userLogged(res, err, true);
        free(res);
    }
}

void PCloudApp::unMount(){
#ifdef Q_OS_WIN
    QByteArray err;
    unmountCmd(err);
    if (err.size() > 0)
        showError(err);
#else
    QProcess process;
    QString path=settings->get("path");
    process.start("umount", QStringList() << "-f" << path);
    if (process.waitForFinished() && process.exitCode()==0)
        return;
    process.start("/sbin/umount", QStringList() << "-f" << path);
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

void PCloudApp::logIn(QString auth, QString uname,  quint64 uid,  bool verified, bool premium, quint64 quota, quint64 usedquota, bool remember)
{
    this->authentication = auth;
    if (remember)
        settings->set("auth", auth);
    loggedin=true;
#ifdef Q_OS_WIN
    if (!settings->isSet("path") || !settings->get("path").toUtf8()[0]){
        QString path("a:");
        path[0] = getFirstFreeDevice();
        settings->set("path", path);
    }
    if (this->notifythread){
        notifythread->terminate();
        notifythread->wait();
        delete notifythread;
        notifythread = NULL;
    }
    notifythread = new RevNotifyThread(this);
    notifythread->start();
#endif
    username=uname;
    userid=uid;
    isVerified = verified;
    isPremium = premium;
    usedSpace = (usedquota/1024.0/1024.0/1024.0);
    freeSpacePercentage = (100*(quota - usedquota))/quota;
    plan = QString::number(quota/1024/1024/1024) + " GB";
    tray->setToolTip(username);
    //if (loggedmenu){
    //loggedmenu->actions()[0]->setText(username);
    //}
    pCloudWin->setOnlineItems(true);
    pCloudWin->setOnlinePages();
    tray->setIcon(QIcon(ONLINE_ICON));
    tray->setContextMenu(loggedmenu);
    if (!mthread){
        mthread=new MonitoringThread(this);
        mthread->start();
    }
}

void PCloudApp::trayMsgClicked()
{    
    if (lastMessageType == 0 || lastMessageType == 1 )
    {
        emit showShares();
        pCloudWin->ui->tabWidgetShares->setCurrentIndex(lastMessageType);
        pCloudWin->sharesPage->load(lastMessageType);
    }
}

void PCloudApp::setOnlineStatus(bool online)
{
    static bool lastStatus = false;
    if (online){
        tray->setIcon(QIcon(ONLINE_ICON));
        if (online != lastStatus) {
            lastMessageType = 2;
            tray->showMessage("PCloud connected", "", QSystemTrayIcon::Information);
            lastStatus = online;
        }
    }
    else{
        tray->setIcon(QIcon(OFFLINE_ICON));
        if (online != lastStatus){
            lastMessageType = 2;
            tray->showMessage("PCloud disconnected", "", QSystemTrayIcon::Information);
            lastStatus = online;
        }
    }
}

bool PCloudApp::userLogged(binresult *userinfo, QByteArray &err, bool remember){
    if (isMounted()){
        setUser(userinfo, remember);
        return true;
    }
    else{
#ifdef Q_OS_WIN
        if (find_res(userinfo, "auth")){
            if (!settings->isSet("path") || !settings->get("path").toUtf8()[0]){
                QString path("a:");
                path[0] = getFirstFreeDevice();
                settings->set("path", path);
            }

            QString auth(find_res(userinfo, "auth")->str);
            if (remember){
                settings->set("auth", auth);
                this->authentication = auth;
            }
            else
                settings->set("auth", "");

            if (mountCmd(atol(settings->get("cachesize").toUtf8()),
                         auth.toUtf8(),
                         settings->geti("usessl"),
                         settings->get("path").toUtf8()[0], err)){
                Sleep(5*1000);
                setUser(userinfo, remember);
                return true;
            }
            return false;
        }
        else {
            err = "Invalid E-mail and Password combination.";
            return false;
        }
#else
        QProcess process;
        QStringList params;
#ifdef Q_OS_MAC
        params.append("-o");
        params.append("volname=pCloud");
        // Adding -o local may or may not be a good idea
        params.append("-o");
        params.append("local");
#endif
        params.append("--auth");
        params.append(find_res(userinfo, "auth")->str);
        if (settings->geti("usessl"))
            params.append("--ssl");
        params.append("--cache");
        params.append(settings->get("cachesize"));
        params.append(settings->get("path"));
#ifdef Q_OS_MAC
        process.start("/usr/local/bin/mount.pfs", params);
#else
        process.start("mount.pfs", params);
#endif
        if (!process.waitForFinished()){
            err="Error mounting filesystem.";
            return false;
        }
        if (process.exitCode()==0){
            setUser(userinfo, remember);
            return true;
        }
        else {
            err=process.readAllStandardError();
            return false;
        }
#endif
    }
}

bool PCloudApp::isLogedIn()
{
    return loggedin;
}
