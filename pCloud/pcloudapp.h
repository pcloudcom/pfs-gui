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
#include "revnotifythread.h"
//#include "pcloudinfodialog.h" //del
//#include "pcloudform.h" //del
#include "pcloudwindow.h"
#include "ui_pcloudwindow.h" //temp

#include <QApplication>
#include <QAction>
#include <QSystemTrayIcon>

class PCloudWindow;

class PCloudApp : public QApplication
{
    Q_OBJECT
private:   
    QAction *registerAction;
    QAction *loginAction;
    QAction *exitAction;
    QAction *logoutAction;
    QAction *openAction;
    QAction *accountAction;
    QAction *settingsAction;  //Settings page
    QAction *sharesAction; // Shares page(3)
  //  QAction *shareFolderAction; // to del
    QAction *outgoingSharesAction; // to del
    QAction *incomingSharesAction; // to del
    QAction *openPCloudWinAction;
    QAction *helpAction; // Help Tab (5)
    QAction *aboutPCloudAction; // About tab(6)
    QMenu *notloggedmenu;
    QMenu *loggedmenu;    
    QSystemTrayIcon *tray;
    RegLogWindow *reglog;
    RegisterWindow *regwin;
    LoginWindow *logwin;
    SettingsWindow *settingswin; //to del
    ShareFolderWindow *sharefolderwin;
    SharesWindow *incomingshareswin; // del
    SharesWindow *outgoingshareswin;    // del   
    MonitoringThread *mthread;

#ifdef Q_OS_WIN
    RevNotifyThread *notifythread;
#endif
    OnlineThread *othread;
    bool loggedin;
    void createMenus();
    void hideAllWindows();
    void showRegLog();
    void setUser(binresult *userinfo, bool rememebr);
    void showWindow(QMainWindow *win);
public:    
    PCloudWindow *pCloudWin;
    QString username;
    quint64 userid;
    QString authentication;
    bool isPremium;
    bool isVerified;
    quint64 freeSpacePercentage;
    qreal usedSpace;
    QString plan;
    PSettings *settings;
    uint32_t lastMessageType;
    explicit PCloudApp(int &argc, char **argv);
    ~PCloudApp();
    bool userLogged(binresult *userinfo, QByteArray &err, bool remember);
    apisock *getAPISock();
    bool isMounted();
    void mount();
    void unMount();
    void showError(QString err);
    void showOnClick();
    bool isLogedIn();
signals:
    void logInSignal(QString, QString, quint64, bool, bool, quint64, quint64, bool);
    void showLoginSignal();
public slots:
    void showTrayMessage(QString title, QString msg);
    void trayClicked(QSystemTrayIcon::ActivationReason reason);
    void showRegister();
    void showLogin();
    void showAccount();
    void showShares();
    void showSettings();
    void showpCloudAbout();
    void showpcloudHelp();
    void openCloudDir();
    void shareFolder();
    void outgoingShares();
    void incomingShares();
    void logOut();
    void doExit();
    void logIn(QString auth, QString uname, quint64 uid, bool verified, bool premium, quint64 quota, quint64 usedquota, bool remember);
    void trayMsgClicked();
    void setOnlineStatus(bool online);
    void openpCloudWindow(); // new main window   

};

#endif // PCLOUDAPP_H
