#include "registerwindow.h"
#include "ui_registerwindow.h"

RegisterWindow::RegisterWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RegisterWindow)
{
    setWindowIcon(QIcon(":/images/images/icon_pcloud.png"));
    ui->setupUi(this);
}

RegisterWindow::~RegisterWindow()
{
    delete ui;
}
