#include "directorypickerdialog.h"
#include "ui_directorypickerdialog.h"
#include "pcloudapp.h"

DirectoryPickerDialog::DirectoryPickerDialog(PCloudApp *a, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DirectoryPickerDialog)
{
    onlyMine=false;
    showRoot=false;
    app=a;
    ui->setupUi(this);
}

DirectoryPickerDialog::~DirectoryPickerDialog()
{
    delete ui;
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

void DirectoryPickerDialog::showEvent(QShowEvent *)
{
    apisock *conn;
    binresult *res, *result, *rn;
    binresult root;
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
                     P_BOOL("noshares", onlyMine));
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
    if (showRoot){
        root.type=PARAM_ARRAY;
        root.length=1;
        root.array=&rn;
        rn=find_res(res, "metadata");
        result=&root;
    }
    else
        result=find_res(find_res(res, "metadata"), "contents");
    ui->dirtree->insertTopLevelItems(0, binresToQList(result));
    free(res);
}

void DirectoryPickerDialog::showError(const QString &err)
{
    ui->error->setText(err);
}
