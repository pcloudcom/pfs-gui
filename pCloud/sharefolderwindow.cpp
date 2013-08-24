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
    setWindowIcon(QIcon(REGULAR_ICON));
    ui->setupUi(this);
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
    uint32_t i;
    for (i=0; i<res->length; i++){
        e=res->array[i];
        item=new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(find_res(e, "name")->str)));
        item->setData(1, Qt::UserRole, (qulonglong)find_res(e, "folderid")->num);
        item->addChildren(binresToQList(find_res(e, "contents")));
        items.append(item);
    }
    return items;
}

void ShareFolderWindow::showEvent(QShowEvent *event)
{
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->settings->get("auth").toUtf8();
    if (!(conn=app->getAPISock())){
        app->showError("Could not connect to server. Check your Internet connection.");
        return;
    }

    ui->dirtree->setColumnCount(1);
    ui->dirtree->setHeaderLabels(QStringList("Name"));
    res=send_command(conn, "listfolder",
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_STR("filtermeta", "contents,folderid,name"),
                     P_NUM("folderid", 0),
                     P_BOOL("recursive", 1),
                     P_BOOL("nofiles", 1),
                     P_BOOL("noshares", 1));
    result=find_res(res, "result");
    if (!result){
        app->showError("Could not connect to server. Check your Internet connection.");
        free(res);
        api_close(conn);
        return;
    }
    if (result->num!=0){
        app->showError(find_res(res, "error")->str);
        free(res);
        api_close(conn);
        return;
    }
    result=find_res(find_res(res, "metadata"), "contents");
    ui->dirtree->insertTopLevelItems(0, binresToQList(result));
    free(res);
    api_close(conn);
}
