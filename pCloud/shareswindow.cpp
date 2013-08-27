#include "shareswindow.h"
#include "ui_shareswindow.h"
#include "pcloudapp.h"
#include "common.h"

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
    setWindowIcon(QIcon(REGULAR_ICON));
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
}

SharesWindow::~SharesWindow()
{
    delete ui;
}

static QString getPermissions(binresult *req){
    QStringList list;
    if (find_res(req, "canread")->num)
        list << "Read";
    if (find_res(req, "canmodify")->num)
        list << "Write";
    if (find_res(req, "cancreate")->num)
        list << "Create";
    if (find_res(req, "candelete")->num)
        list << "Delete";
    return list.join(",");
}

static void fillList(QTreeWidget *table, binresult *arr, const char *fieldname, const char *idname, int type){
    QStringList headers;
    QList<QTreeWidgetItem *> items;
    uint32_t i;
    headers << "Folder Name" << theader[type] << "Permissions";
    table->clear();
    table->setHeaderLabels(headers);
    for (i=0; i<arr->length; i++){
        QStringList row;
        QTreeWidgetItem *item;
        row << find_res(arr->array[i], fieldname)->str << find_res(arr->array[i], mailfield[type])->str << getPermissions(arr->array[i]);
        item=new QTreeWidgetItem((QTreeWidget*)0, row);
        item->setData(0, Qt::UserRole, (qulonglong)find_res(arr->array[i], idname)->num);
        items.append(item);
    }
    table->insertTopLevelItems(0, items);
    table->resizeColumnToContents(0);
    table->resizeColumnToContents(1);
    table->resizeColumnToContents(2);
}

void SharesWindow::showEvent(QShowEvent *event)
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

void SharesWindow::cancelRequest()
{
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->settings->get("auth").toUtf8();
    if (!ui->pending->currentItem())
        return;
    uint64_t sharerequestid=ui->pending->currentItem()->data(0, Qt::UserRole).toULongLong();
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
