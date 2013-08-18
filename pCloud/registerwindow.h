#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QMainWindow>

namespace Ui {
class RegisterWindow;
}

class PCloudApp;

class RegisterWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit RegisterWindow(PCloudApp *a, QWidget *parent = 0);
    ~RegisterWindow();
private:
    PCloudApp *app;
    Ui::RegisterWindow *ui;
    void setError(const char *err);
public slots:
    void focusPass();
    void focusConfirm();
    void focusTOS();
    void doRegister();
};

#endif // REGISTERWINDOW_H
