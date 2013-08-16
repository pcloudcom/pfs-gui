#ifndef PCLOUDAPP_H
#define PCLOUDAPP_H

#include "reglogwindow.h"
#include "registerwindow.h"

#include <QApplication>
#include <QAction>
#include <QSystemTrayIcon>

class PCloudApp : public QApplication
{
    Q_OBJECT
private:
    QAction *exitAction;
    QAction *registerAction;
    QMenu *notloggedmenu;
    QSystemTrayIcon *tray;
    RegLogWindow *reglog;
    RegisterWindow *regwin;
    void createMenus();
    void hideAllWindows();
    void showRegLog();
public:
    explicit PCloudApp(int &argc, char **argv);
    ~PCloudApp();
    
signals:
    
public slots:
    void doExit();
    void trayClicked(QSystemTrayIcon::ActivationReason reason);
    void showRegister();
};

#endif // PCLOUDAPP_H
