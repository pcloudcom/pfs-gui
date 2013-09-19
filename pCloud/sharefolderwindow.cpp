#include <QMessageBox>
#include "sharefolderwindow.h"
#include "ui_sharefolderwindow.h"
#include "binapi.h"
#include "pcloudapp.h"
#include "common.h"

ShareFolderWindow::ShareFolderWindow(PCloudApp *a, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ShareFolderWindow)
{
    app=a;
    setWindowIcon(QIcon(WINDOW_ICON));
    ui->setupUi(this);
    connect(ui->cancelbutton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->sharebutton, SIGNAL(clicked()), this, SLOT(shareFolder()));
    connect(ui->dirtree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(dirSelected(QTreeWidgetItem *)));
}

ShareFolderWindow::~ShareFolderWindow()
{
    delete ui;
}

void ShareFolderWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}


static QList<QTreeWidgetItem *> binresToQList(binresult *res){
    QList<QTreeWidgetItem *> items;
    QTreeWidgetItem *item;
    binresult *e;
    quint32 i;
    for (i=0; i<res->length; i++){
        e=res->array[i];
        item=new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(find_res(e, "name")->str)));
        item->setData(1, Qt::UserRole, (qulonglong)find_res(e, "folderid")->num);
        item->addChildren(binresToQList(find_res(e, "contents")));
        items.append(item);
    }
    return items;
}

void ShareFolderWindow::showEvent(QShowEvent *)
{
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->settings->get("auth").toUtf8();
    if (!(conn=app->getAPISock())){
        showError("Could not connect to server. Check your Internet connection.");
        return;
    }
    ui->dirtree->clear();
    ui->dirtree->setColumnCount(1);
    ui->dirtree->setHeaderLabels(QStringList("Name"));
    res=send_command(conn, "listfolder",
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_STR("filtermeta", "contents,folderid,name"),
                     P_NUM("folderid", 0),
                     P_BOOL("recursive", 1),
                     P_BOOL("nofiles", 1),
                     P_BOOL("noshares", 1));
    api_close(conn);
    result=find_res(res, "result");
    if (!result){
        showError("Could not connect to server. Check your Internet connection.");
        free(res);
        return;
    }
    if (result->num!=0){
        showError(find_res(res, "error")->str);
        free(res);
        return;
    }
    result=find_res(find_res(res, "metadata"), "contents");
    ui->dirtree->insertTopLevelItems(0, binresToQList(result));
    free(res);
}

void ShareFolderWindow::showError(const QString &err){
    ui->error->setText(err);
}

void ShareFolderWindow::verifyEmail(apisock *conn){
  QMessageBox::StandardButton reply;
  reply=QMessageBox::question(this, "Please verify your e-mail address",
                                    "E-mail address verification is required to share folders. Send verification email now?",
                                    QMessageBox::Yes|QMessageBox::No);
  if (reply==QMessageBox::Yes){
      QByteArray auth=app->settings->get("auth").toUtf8();
      binresult *res;
      res=send_command(conn, "sendverificationemail",
                       P_LSTR("auth", auth.constData(), auth.size()));
      free(res);
      QMessageBox::information(this, "Please check your e-mail", "E-mail verification sent to: "+app->username);
  }
}

void ShareFolderWindow::shareFolder()
{
    QByteArray auth=app->settings->get("auth").toUtf8();
    QStringList mails=ui->email->text().split(",");
    QByteArray name=ui->sharename->text().toUtf8();
    quint64 folderid=ui->dirtree->currentItem()->data(1, Qt::UserRole).toULongLong();
    quint64 perms=(ui->permCreate->isChecked()?1:0)+
                   (ui->permModify->isChecked()?2:0)+
                   (ui->permDelete->isChecked()?4:0);
    apisock *conn=app->getAPISock();
    binresult *res, *result;
    if (!conn){
        showError("Could not connect to server. Check your Internet connection.");
        return;
    }
    while (!mails.empty()){
        QByteArray mail=mails[0].trimmed().toUtf8();
        ui->email->setText(mails.join(","));
        res=send_command(conn, "sharefolder",
                         P_LSTR("auth", auth.constData(), auth.size()),
                         P_LSTR("mail", mail.constData(), mail.size()),
                         P_LSTR("name", name.constData(), name.size()),
                         P_NUM("folderid", folderid),
                         P_NUM("permissions", perms));
        result=find_res(res, "result");
        if (!result){
            showError("Could not connect to server. Check your Internet connection.");
            free(res);
            api_close(conn);
            return;
        }
        if (result->num==2014){
            free(res);
            verifyEmail(conn);
            api_close(conn);
            return;
        }
        if (result->num!=0){
            showError(find_res(res, "error")->str);
            api_close(conn);
            free(res);
            return;
        }
        free(res);
        mails.removeFirst();
    }
    api_close(conn);
    ui->error->setText("");
    ui->email->setText("");
    hide();
}

void ShareFolderWindow::dirSelected(QTreeWidgetItem *dir)
{
    ui->sharename->setText(dir->text(0));
}
