#ifndef REGLOGWINDOW_H
#define REGLOGWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QCloseEvent>

namespace Ui {
class RegLogWindow;
}

class RegLogWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit RegLogWindow(QApplication *app, QWidget *parent = 0);
    ~RegLogWindow();
    
private:
    Ui::RegLogWindow *ui;
    void closeEvent(QCloseEvent *event);
};

#endif // REGLOGWINDOW_H
