#include "shareswindow.h"
#include "ui_shareswindow.h"
#include "pcloudapp.h"
#include "common.h"
#include "directorypickerdialog.h"
#include "ui_directorypickerdialog.h"
#include "changepermissionsdialog.h"
#include "ui_changepermissionsdialog.h"

const char *winnames[2]={"My Shares", "Shared with Me"};
const char *lsparam[2]={"noincoming", "nooutgoing"};
const char *lsfield[2]={"outgoing", "incoming"};
const char *theader[2]={"Shared with", "From"};
const char *mailfield[2]={"tomail", "frommail"};
const char *stopmethod[2]={"cancelsharerequest", "declineshare"};

SharesWindow::SharesWindow(PCloudApp *a, int t, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SharesWindow)
{
    app=a;
    type=t;
    setWindowIcon(QIcon(WINDOW_ICON));
    ui->setupUi(this);
    setWindowTitle(winnames[t]);
    if (type==0){
        ui->acceptbutton->hide();
        ui->rejectbutton->setText("Cancel");
    }
    else{
        ui->modifybutton->hide();
    }
    connect(ui->closebutton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->rejectbutton, SIGNAL(clicked()), this, SLOT(cancelRequest()));
    connect(ui->acceptbutton, SIGNAL(clicked()), this, SLOT(acceptRequest()));
    connect(ui->stopbutton, SIGNAL(clicked()), this, SLOT(stopShare()));
    connect(ui->modifybutton, SIGNAL(clicked()), this, SLOT(modifyShare()));
}

SharesWindow::~SharesWindow()
{
    delete ui;
}

void SharesWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

static QString getPermissions(binresult *req){
    QStringList list;
    if (find_res(req, "canread")->num)
        list << "Read";
    if (find_res(req, "canmodify")->num)
        list << "Modify";
    if (find_res(req, "cancreate")->num)
        list << "Create";
    if (find_res(req, "candelete")->num)
        list << "Delete";
    return list.join(",");
}

static quint32 getPermissionsNum(binresult *req){
    quint32 r=0;
    if (find_res(req, "canread")->num)
        r|=1;
    if (find_res(req, "canmodify")->num)
        r|=2;
    if (find_res(req, "candelete")->num)
        r|=4;
    if (find_res(req, "cancreate")->num)
        r|=8;
    return r;
}

static void fillList(QTreeWidget *table, binresult *arr, const char *fieldname, const char *idname, int type){
    QStringList headers;
    QList<QTreeWidgetItem *> items;
    quint32 i;
    headers << "Folder Name" << theader[type] << "Permissions";
    table->clear();
    table->setHeaderLabels(headers);
    for (i=0; i<arr->length; i++){
        QStringList row;
        QTreeWidgetItem *item;
        row << find_res(arr->array[i], fieldname)->str << find_res(arr->array[i], mailfield[type])->str << getPermissions(arr->array[i]);
        item=new QTreeWidgetItem((QTreeWidget*)0, row);
        item->setData(0, Qt::UserRole, (qulonglong)find_res(arr->array[i], idname)->num);
        item->setData(1, Qt::UserRole, (quint32)getPermissionsNum(arr->array[i]));
        items.append(item);
    }
    table->insertTopLevelItems(0, items);
    table->resizeColumnToContents(0);
    table->resizeColumnToContents(1);
    table->resizeColumnToContents(2);
    table->sortByColumn(0, Qt::AscendingOrder);
}

void SharesWindow::showEvent(QShowEvent *)
{
    load();
}

void SharesWindow::showError(const QString &err)
{
    ui->error->setText(err);
}

void SharesWindow::load()
{
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->settings->get("auth").toUtf8();
    showError("");
    if (!(conn=app->getAPISock())){
        showError("Could not connect to server. Check your Internet connection.");
        return;
    }
    res=send_command(conn, "listshares",
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_BOOL(lsparam[type], 1));
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
    fillList(ui->current, find_res(find_res(res, "shares"), lsfield[type]), "foldername", "shareid", type);
    fillList(ui->pending, find_res(find_res(res, "requests"), lsfield[type]), "sharename", "sharerequestid", type);
}

void SharesWindow::selectErr(){
    showError("Please select a share.");
}

void SharesWindow::cancelRequest()
{
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->settings->get("auth").toUtf8();
    if (!ui->pending->currentItem())
        return selectErr();
    quint64 sharerequestid=ui->pending->currentItem()->data(0, Qt::UserRole).toULongLong();
    if (!(conn=app->getAPISock())){
        showError("Could not connect to server. Check your Internet connection.");
        return;
    }
    res=send_command(conn, stopmethod[type],
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_NUM("sharerequestid", sharerequestid));
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
    free(res);
    load();
}

void SharesWindow::acceptRequest()
{
    if (!ui->pending->currentItem())
        return selectErr();
    quint64 sharerequestid=ui->pending->currentItem()->data(0, Qt::UserRole).toULongLong();
    DirectoryPickerDialog dir(app, this);
    dir.onlyMine=true;
    dir.showRoot=true;
    dir.setWindowTitle("Select a directory to accept share to...");
    if (dir.exec()==QDialog::Rejected || !dir.ui->dirtree->currentItem())
        return;
    quint64 folderid=dir.ui->dirtree->currentItem()->data(1, Qt::UserRole).toULongLong();
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->settings->get("auth").toUtf8();
    if (!(conn=app->getAPISock())){
        showError("Could not connect to server. Check your Internet connection.");
        return;
    }
    res=send_command(conn, "acceptshare",
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_NUM("sharerequestid", sharerequestid),
                     P_NUM("folderid", folderid));
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
    free(res);
    load();
}

void SharesWindow::stopShare()
{
    if (!ui->current->currentItem())
        return selectErr();
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->settings->get("auth").toUtf8();
    quint64 shareid=ui->current->currentItem()->data(0, Qt::UserRole).toULongLong();
    if (!(conn=app->getAPISock())){
        showError("Could not connect to server. Check your Internet connection.");
        return;
    }
    res=send_command(conn, "removeshare",
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_NUM("shareid", shareid));
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
    free(res);
    load();
}

void SharesWindow::modifyShare()
{
    QTreeWidgetItem *item=ui->current->currentItem();
    if (!item)
        return selectErr();
    ChangePermissionsDialog perms(item->data(1, Qt::UserRole).toUInt(), item->text(0), item->text(1), this);
    if (perms.exec()==QDialog::Rejected)
        return;
    quint64 shareid=item->data(0, Qt::UserRole).toULongLong();
    quint64 prms=(perms.ui->permCreate->isChecked()?1:0)+
                  (perms.ui->permModify->isChecked()?2:0)+
                  (perms.ui->permDelete->isChecked()?4:0);
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->settings->get("auth").toUtf8();
    if (!(conn=app->getAPISock())){
        showError("Could not connect to server. Check your Internet connection.");
        return;
    }
    res=send_command(conn, "changeshare",
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_NUM("shareid", shareid),
                     P_NUM("permissions", prms));
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
    free(res);
    load();
}
