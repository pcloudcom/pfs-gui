#ifndef PCLOUDAPP_H
#define PCLOUDAPP_H

#include "reglogwindow.h"
#include "registerwindow.h"
#include "loginwindow.h"
#include "binapi.h"

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
    QMenu *notloggedmenu;
    QSystemTrayIcon *tray;
    RegLogWindow *reglog;
    RegisterWindow *regwin;
    LoginWindow *logwin;
    void createMenus();
    void hideAllWindows();
    void showRegLog();
public:
    explicit PCloudApp(int &argc, char **argv);
    ~PCloudApp();
    bool UserLogged(binresult *userinfo, QByteArray &err);
    
signals:
    
public slots:
    void trayClicked(QSystemTrayIcon::ActivationReason reason);
    void showRegister();
    void showLogin();
};

#endif // PCLOUDAPP_H
