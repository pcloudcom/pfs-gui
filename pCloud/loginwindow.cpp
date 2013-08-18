#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "binapi.h"
#include "pcloudapp.h"

LoginWindow::LoginWindow(PCloudApp *a, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    app=a;
    setWindowIcon(QIcon(":/images/images/icon_pcloud.png"));
    ui->setupUi(this);
    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(logIn()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(hide()));
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::setError(const char *err){
    ui->error->setText(err);
}

void LoginWindow::logIn(){
    QByteArray email=ui->email->text().toUtf8();
    QByteArray password=ui->password->text().toUtf8();
    apisock *conn=api_connect();
    binresult *res, *result;
    QByteArray err;
    if (!conn){
        setError("Connection to server failed.");
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
        return;
    }
    if (result->num!=0){
        setError(find_res(res, "error")->str);
        free(res);
        return;
    }
    if (!app->UserLogged(res, err))
        setError(err);
    else
        hide();
    free(res);
}
