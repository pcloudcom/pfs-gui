#ifndef PCLOUDAPP_H
#define PCLOUDAPP_H

#include "reglogwindow.h"
#include "registerwindow.h"
#include "loginwindow.h"
#include "settingswindow.h"
#include "sharefolderwindow.h"
#include "shareswindow.h"
#include "binapi.h"
#include "psettings.h"
#include "monitoringthread.h"
#include "onlinethread.h"

#include <QApplication>
#include <QAction>
#include <QSystemTrayIcon>

class PCloudApp : public QApplication
{
    Q_OBJECT
private:
    QAction *registerAction;
    QAction *loginAction;
    QAction *exitAction;
    QAction *logoutAction;
    QAction *openAction;
    QAction *settingsAction;
    QAction *shareFolderAction;
    QAction *outgoingSharesAction;
    QAction *incomingSharesAction;
    QMenu *notloggedmenu;
    QMenu *loggedmenu;
    QSystemTrayIcon *tray;
    RegLogWindow *reglog;
    RegisterWindow *regwin;
    LoginWindow *logwin;
    SettingsWindow *settingswin;
    ShareFolderWindow *sharefolderwin;
    SharesWindow *incomingshareswin;
    SharesWindow *outgoingshareswin;
    MonitoringThread *mthread;
    OnlineThread *othread;
    QString username;
    bool loggedin;
    void createMenus();
    void hideAllWindows();
    void showRegLog();
    void setUser(binresult *userinfo);
    void showWindow(QMainWindow *win);
public:
    PSettings *settings;
    explicit PCloudApp(int &argc, char **argv);
    ~PCloudApp();
    bool userLogged(binresult *userinfo, QByteArray &err);
    apisock *getAPISock();
    bool isMounted();
    void mount();
    void unMount();
    void showError(QString err);
signals:
    void logInSignal(QString, QString);
public slots:
    void showTrayMessage(QString title, QString msg);
    void trayClicked(QSystemTrayIcon::ActivationReason reason);
    void showRegister();
    void showLogin();
    void showSettings();
    void openCloudDir();
    void shareFolder();
    void outgoingShares();
    void incomingShares();
    void logOut();
    void doExit();
    void logIn(QString auth, QString uname);
};

#endif // PCLOUDAPP_H
