#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "binapi.h"
#include "pcloudapp.h"
#include "common.h"

LoginWindow::LoginWindow(PCloudApp *a, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    app=a;
    setWindowIcon(QIcon(WINDOW_ICON));
    ui->setupUi(this);
    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::red);
    ui->error->setPalette(palette);    
    ui->forgotPassBtn->setStyleSheet("QToolButton{background-color:transparent;} QToolButton:hover{text-decoration: underline; background-color: transparent;}");

    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(logIn()));
    //    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->password, SIGNAL(returnPressed()), this, SLOT(logIn()));
    connect(ui->email, SIGNAL(returnPressed()), this, SLOT(focusPass()));
    connect(ui->registerButton, SIGNAL(clicked()), app, SLOT(showRegister()));
    connect(ui->forgotPassBtn,SIGNAL(clicked()), this,SLOT(forgotPassword()));


}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void LoginWindow::focusPass(){
    ui->password->setFocus();
}

void LoginWindow::setError(const char *err)
{
    ui->error->setText(err);
}

void LoginWindow::logIn()
{
    QByteArray email=ui->email->text().toUtf8();
    QByteArray password=ui->password->text().toUtf8();
    apisock *conn;
    binresult *res, *result;
    QByteArray err;
    if (email.size() == 0 || password.size() == 0){
        setError("Login Failed. Enter login information.");
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!(conn=app->getAPISock())){
        setError("Connection to server failed.");
        QApplication::restoreOverrideCursor();
        return;
    }
    res=send_command(conn, "userinfo",
                     P_LSTR("username", email.constData(), email.size()),
                     P_LSTR("password", password.constData(), password.size()),
                     P_BOOL("getauth", 1));
    api_close(conn);
    result=find_res(res, "result");
    if (!result){
        setError("Connection to server failed.");
        free(res);
        QApplication::restoreOverrideCursor();
        return;
    }
    if (result->num!=0){
        //        setError(find_res(res, "error")->str);
        setError("Invalid E-mail and Password combination.");
        free(res);
        QApplication::restoreOverrideCursor();
        return;
    }
    if (!app->userLogged(res, err, ui->checkBox->isChecked()))
        setError(err);
    else{
        setError("");
        ui->password->clear();
        hide();
        app->openCloudDir();
    }
    free(res);
    QApplication::restoreOverrideCursor();
}

void LoginWindow::forgotPassword()
{
    QByteArray email=ui->email->text().toUtf8();
    apisock *conn;
    if (email.size() == 0){
        setError("Enter your email.");
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!(conn=app->getAPISock())){
        setError("Connection to server failed.");
        QApplication::restoreOverrideCursor();
        return;
    }
    free(send_command(conn, "lostpassword",
                      P_LSTR("mail", email.constData(), email.size())));
    api_close(conn);
    setError("");
    QMessageBox::information(NULL, "Reset password", "An email with passowrd reset instructions is sent to your address.");
    QApplication::restoreOverrideCursor();
}
