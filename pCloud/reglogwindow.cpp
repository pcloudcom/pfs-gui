#include "reglogwindow.h"
#include "ui_reglogwindow.h"
#include "common.h"

RegLogWindow::RegLogWindow(QApplication *app, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RegLogWindow)
{
    setWindowIcon(QIcon(WINDOW_ICON));
    ui->setupUi(this);
    connect(ui->registerButton, SIGNAL(clicked()), app, SLOT(showRegister()));
    connect(ui->loginButton, SIGNAL(clicked()), app, SLOT(showLogin()));
}

RegLogWindow::~RegLogWindow()
{
    delete ui;
}

void RegLogWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}
