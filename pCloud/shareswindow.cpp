#include "shareswindow.h"
#include "ui_shareswindow.h"
#include "pcloudapp.h"
#include "common.h"

const char *winnames[2]={"My Shares", "Shared with Me"};
const char *lsparam[2]={"noincoming", "nooutgoing"};
const char *lsfield[2]={"outgoing", "incoming"};
const char *theader[2]={"Shared with", "From"};
const char *mailfield[2]={"tomail", "frommail"};

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
    connect(ui->closebutton, SIGNAL(clicked()), this, SLOT(hide()));
}

SharesWindow::~SharesWindow()
{
    delete ui;
}

void fillList(QTreeWidget *table, binresult *arr, const char *fieldname, const char *idname, int type){
    QStringList headers;
    QList<QTreeWidgetItem *> items;
    uint32_t i;
    headers << "Folder Name" << theader[type];
    table->clear();
    table->setHeaderLabels(headers);
    for (i=0; i<arr->length; i++){
        QStringList row;
        QTreeWidgetItem *item;
        row << find_res(arr->array[i], fieldname)->str << find_res(arr->array[i], mailfield[type])->str;
        item=new QTreeWidgetItem((QTreeWidget*)0, row);
        item->setData(0, Qt::UserRole, (qulonglong)find_res(arr->array[i], idname)->num);
        items.append(item);
    }
    table->insertTopLevelItems(0, items);
    table->resizeColumnToContents(0);
    table->resizeColumnToContents(1);
}

void SharesWindow::showEvent(QShowEvent *event)
{
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->settings->get("auth").toUtf8();
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

void SharesWindow::showError(const QString &err)
{
}
