#ifndef PCLOUDAPP_H
#define PCLOUDAPP_H

#include "reglogwindow.h"
#include "registerwindow.h"
#include "loginwindow.h"
#include "settingswindow.h"
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
    QMenu *notloggedmenu;
    QMenu *loggedmenu;
    QSystemTrayIcon *tray;
    RegLogWindow *reglog;
    RegisterWindow *regwin;
    LoginWindow *logwin;
    SettingsWindow *settingswin;
    QString username;
    bool loggedin;
    void createMenus();
    void hideAllWindows();
    void showRegLog();
    void setUser(binresult *userinfo);
public:
    PSettings *settings;
    explicit PCloudApp(int &argc, char **argv);
    ~PCloudApp();
    bool userLogged(binresult *userinfo, QByteArray &err);
    apisock *getAPISock();
    bool isMounted();
    void mount();
    void unMount();
signals:
    
public slots:
    void trayClicked(QSystemTrayIcon::ActivationReason reason);
    void showRegister();
    void showLogin();
    void showSettings();
    void openCloudDir();
    void logOut();
    void doExit();
};

#endif // PCLOUDAPP_H
