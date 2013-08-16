#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QMainWindow>

namespace Ui {
class RegisterWindow;
}

class RegisterWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit RegisterWindow(QWidget *parent = 0);
    ~RegisterWindow();
    
private:
    Ui::RegisterWindow *ui;
};

#endif // REGISTERWINDOW_H
