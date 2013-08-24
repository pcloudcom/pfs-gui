#ifndef PCLOUDAPP_H
#define PCLOUDAPP_H

#include "reglogwindow.h"
#include "registerwindow.h"
#include "loginwindow.h"
#include "settingswindow.h"
#include "sharefolderwindow.h"
#include "binapi.h"
#include "psettings.h"

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
    QMenu *notloggedmenu;
    QMenu *loggedmenu;
    QSystemTrayIcon *tray;
    RegLogWindow *reglog;
    RegisterWindow *regwin;
    LoginWindow *logwin;
    SettingsWindow *settingswin;
    ShareFolderWindow *sharefolderwin;
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
    
public slots:
    void trayClicked(QSystemTrayIcon::ActivationReason reason);
    void showRegister();
    void showLogin();
    void showSettings();
    void openCloudDir();
    void shareFolder();
    void logOut();
    void doExit();
};

#endif // PCLOUDAPP_H
