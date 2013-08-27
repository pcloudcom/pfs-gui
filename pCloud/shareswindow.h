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
    
private:
    PCloudApp *app;
    Ui::SharesWindow *ui;
    int type;
    void showEvent(QShowEvent *event);
    void showError(const QString &err);
};

#endif // SHARESWINDOW_H
