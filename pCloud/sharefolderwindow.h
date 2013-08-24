#ifndef SHAREFOLDERWINDOW_H
#define SHAREFOLDERWINDOW_H

#include <QMainWindow>

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
};

#endif // SHAREFOLDERWINDOW_H
