#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

namespace Ui {
class LoginWindow;
}

class PCloudApp;

class LoginWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit LoginWindow(PCloudApp *a, QWidget *parent = 0);
    ~LoginWindow();
private:
    PCloudApp *app;
    Ui::LoginWindow *ui;
    void setError(const char *err);
    void closeEvent(QCloseEvent *event);
public slots:
    void logIn();
    void focusPass();
};

#endif // LOGINWINDOW_H
