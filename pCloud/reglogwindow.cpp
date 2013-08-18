#include "reglogwindow.h"
#include "ui_reglogwindow.h"

RegLogWindow::RegLogWindow(QApplication *app, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RegLogWindow)
{
    setWindowIcon(QIcon(":/images/images/icon_pcloud.png"));
    ui->setupUi(this);
    connect(ui->registerButton, SIGNAL(clicked()), app, SLOT(showRegister()));
    connect(ui->loginButton, SIGNAL(clicked()), app, SLOT(showLogin()));
}

RegLogWindow::~RegLogWindow()
{
    delete ui;
}
