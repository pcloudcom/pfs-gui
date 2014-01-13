#ifndef PCLOUDWINDOW_H
#define PCLOUDWINDOW_H
#include "settingspage.h"
#include "sharespage.h"
#include "sharefolderwindow.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QListWidgetItem>

namespace Ui {
class PCloudWindow;
}

class PCloudApp;
class SettingsPage;
class SharesPage;

class PCloudWindow : public QMainWindow
{
    Q_OBJECT

public:
    friend class PCloudApp;
    friend class SettingsPage; // to access ui
    friend class SharesPage;
    friend class ShareFolderWindow;
    explicit PCloudWindow(PCloudApp *a, QWidget *parent = 0);
    ~PCloudWindow();

private:   
    Ui::PCloudWindow *ui;
    PCloudApp *app;
    SettingsPage *settngsPage;
    SharesPage *sharesPage;
    void closeEvent(QCloseEvent *event);
    void setStretch(int i, int j);
    void fillAcountNotLoggedPage();
    void fillAccountLoggedPage();
    void fillHelpPage();
    void fillAboutPage();
public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void showpcloudWindow(int index);
    void setOnlineItems(bool online);
    void setOnlinePages();  //when the user logs in    
    void openWebPage();
    void openOnlineTutorial();
    void openOnlineHelp();
    void sendFeedback();
    void contactUs();
    void changePass();
    void upgradePlan();
    void verifyEmail();
};

#endif // PCLOUDWINDOW_H
