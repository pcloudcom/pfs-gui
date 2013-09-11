#ifndef SHARESWINDOW_H
#define SHARESWINDOW_H

#include <QMainWindow>

namespace Ui {
class SharesWindow;
}

class PCloudApp;

class SharesWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit SharesWindow(PCloudApp *a, int t, QWidget *parent = 0);
    ~SharesWindow();
    
    void selectErr();
private:
    PCloudApp *app;
    Ui::SharesWindow *ui;
    int type;
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *);
    void showError(const QString &err);
    void load();
public slots:
    void cancelRequest();
    void acceptRequest();
    void stopShare();
    void modifyShare();
};

#endif // SHARESWINDOW_H
