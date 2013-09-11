#ifndef SHAREFOLDERWINDOW_H
#define SHAREFOLDERWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>

namespace Ui {
class ShareFolderWindow;
}

class PCloudApp;

class ShareFolderWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ShareFolderWindow(PCloudApp *a, QWidget *parent = 0);
    ~ShareFolderWindow();
private:
    Ui::ShareFolderWindow *ui;
    PCloudApp *app;
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent * event);
    void showError(const QString &err);
    void verifyEmail();
public slots:
    void shareFolder();
    void dirSelected(QTreeWidgetItem *dir);
};

#endif // SHAREFOLDERWINDOW_H
