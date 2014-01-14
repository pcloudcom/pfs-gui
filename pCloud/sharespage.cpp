#include "sharespage.h"
#include "pcloudwindow.h"
#include "ui_pcloudwindow.h"
#include "pcloudapp.h"
#include "common.h"
#include "directorypickerdialog.h"
#include "ui_directorypickerdialog.h"
#include "changepermissionsdialog.h"
#include "ui_changepermissionsdialog.h"

#include<QItemDelegate>
#include<QTreeWidgetItem>
#include <QTableWidget>
#include <QLayout>
#include<QAbstractScrollArea>
#include <QDebug>
#include <QWidget>
#include <QTreeWidget>

#include <QtPlugin>


//const char *winnames2[2]={"My Shares", "Shared with Me"};
const char *lsparam2[2]={"noincoming", "nooutgoing"};
const char *lsfield2[2]={"outgoing", "incoming"};
const char *theader2[2]={"Shared with", "From"};
const char *mailfield2[2]={"tomail", "frommail"};
const char *stopmethod2[2]={"cancelsharerequest", "declineshare"};

SharesPage::SharesPage(PCloudWindow *w, PCloudApp *a,  QWidget *parent) :
    QMainWindow(parent)
{
    win = w;
    app = a;
    win->ui->tabWidgetShares->setTabText(0, tr("My Shares"));
    win->ui->tabWidgetShares->setTabIcon(0, QIcon(":/images/images/myshares.png"));
    win->ui->tabWidgetShares->setTabText(1, tr("Shared with me"));
    win->ui->tabWidgetShares->setTabIcon(1, QIcon(":/images/images/sharedwithme.png"));
    win->ui->tabWidgetShares->setCurrentIndex(0);

    //temp
    win->ui->treeMyShares->hideColumn(3);
    win->ui->treeMyRequest->hideColumn(3);
    win->ui->treeSharedWithMe->hideColumn(3);
    win->ui->treeRequestsWithMe->hideColumn(3);

    connect(win->ui->btnTempModify, SIGNAL(clicked()), this, SLOT(modifyShare()));
    connect(win->ui->btnTempStop, SIGNAL(clicked()), this, SLOT(btnStop0())); //stopshare
    connect(win->ui->btnTempCancel, SIGNAL(clicked()), this, SLOT(btnCancel0())); //cancelReq
    connect(win->ui->btnTempStop2, SIGNAL(clicked()), this, SLOT(btnStop1())); //stopshare
    connect(win->ui->btnTempAccept, SIGNAL(clicked()), this, SLOT(acceptRequest()));
    connect(win->ui->btnTempCancel2, SIGNAL(clicked()), this, SLOT(btnCancel1())); //cancelReq
    //temp end

    // remove scroll
    //win->ui->treeMyShares->QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //win->ui->treeMyShares->header()->setStretchLastSection(true);
    // win->ui->treeMyShares->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //win->ui->treeMyShares->header()->setStretchLastSection(true);
    connect(win->ui->tabWidgetShares, SIGNAL(currentChanged(int)), this, SLOT(load(int)));

    //test:
    connect(win->ui->treeMyShares, SIGNAL(itemSelectionChanged()), this, SLOT(testSelectedItem()));

    QPalette paletteRedText;
    paletteRedText.setColor(QPalette::WindowText, Qt::red);
    win->ui->label_errorMyShares->setVisible(false);
    win->ui->label_errorMyShares->setPalette(paletteRedText);
    win->ui->label_errorShrdWithMe->setVisible(false);
    win->ui->label_errorShrdWithMe->setPalette(paletteRedText);

    loadAll();
}

void SharesPage::btnStop0()
{
    type = 0;
    emit stopShare(win->ui->treeMyShares);
}

void SharesPage::btnStop1()
{
    type = 1;
    emit stopShare(win->ui->treeSharedWithMe);
}

void SharesPage::btnCancel0()
{
    type = 0;
    emit cancelRequest(win->ui->treeMyRequest);
}


void SharesPage::btnCancel1()
{
    type = 1;
    emit cancelRequest(win->ui->treeRequestsWithMe);
}


void SharesPage::testSelectedItem()
{
    qDebug()<< " selected page changed ";
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

void SharesPage::fillList(QTreeWidget *table, binresult *arr, const char *fieldname, const char *idname, int type, bool isRequest){
    //  QList<QTreeWidgetItem *> items;
    quint32 i;
    table->clear();
    if (isRequest)
    {
        if (!arr->length)
        {
            setRequestsVisibility(false, type);
            return;
        }
        else
            setRequestsVisibility(true, type);
    }


    for (i=0; i<arr->length; i++){
        QStringList row;

        QTreeWidgetItem *item;
         row <<find_res(arr->array[i], mailfield2[type])->str <<  find_res(arr->array[i], fieldname)->str << getPermissions(arr->array[i]);
        //tmp row <<find_res(arr->array[i], mailfield2[type])->str <<  find_res(arr->array[i], fieldname)->str;
        item=new QTreeWidgetItem((QTreeWidget*)0, row);
        item->setData(0, Qt::UserRole, (qulonglong)find_res(arr->array[i], idname)->num);
        item->setData(1, Qt::UserRole, (quint32)getPermissionsNum(arr->array[i]));
        table->insertTopLevelItem(i, item);
       //tmp setButtons(type,isRequest,table,item,arr->array[i]);

        //   items.append(item);

    }
    //   table->insertTopLevelItems(0, items);
    table->resizeColumnToContents(0);
    table->resizeColumnToContents(1);
    table->resizeColumnToContents(2);
    table->sortByColumn(0, Qt::AscendingOrder);
    //table->setMinimumWidth(700);

    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);


}
void SharesPage::setRequestsVisibility(bool visible, int type)
{
    if (!type) //My Shares tab
    {
        win->ui->treeMyRequest->setVisible(visible);
        win->ui->label_myreq->setVisible(visible);
        win->ui->btnTempCancel->setVisible(visible); //tmp
        win->ui->label_noMyRqsts->setVisible(!visible);
    }
    else //Shared with me tab
    {
        win->ui->treeRequestsWithMe->setVisible(visible);
        win->ui->label_requests->setVisible(visible);
        win->ui->btnTempAccept->setVisible(visible);
        win->ui->btnTempCancel2->setVisible(visible);
        win->ui->label_noRqstsWithMe->setVisible(!visible);
    }
}

void SharesPage::load(int type)
{
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->authentication.toUtf8();
    showError(type, "");
    if (!(conn=app->getAPISock())){
          showError(type, "Could not connect to server. Check your Internet connection.");
        return;
    }
    res=send_command(conn, "listshares",
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_BOOL(lsparam2[type], 1));

    api_close(conn);
    result=find_res(res, "result");
    if (!result){
        showError(type,"Could not connect to server. Check your Internet connection.");
        free(res);
        return;
    }
    if (result->num!=0){
         showError(type, find_res(res, "error")->str);
        free(res);
        return;
    }
    if (type) //Shared with me tab
    {
        this->fillList(win->ui->treeSharedWithMe, find_res(find_res(res, "shares"), lsfield2[type]), "foldername", "shareid", type, false);
        this->fillList(win->ui->treeRequestsWithMe, find_res(find_res(res, "requests"), lsfield2[type]), "sharename", "sharerequestid", type, true);
    }
    else // My shares tab
    {
        this->fillList(win->ui->treeMyShares, find_res(find_res(res, "shares"), lsfield2[type]), "foldername", "shareid", type, false);
        this->fillList(win->ui->treeMyRequest, find_res(find_res(res, "requests"), lsfield2[type]), "sharename", "sharerequestid", type, true);
    }

}

void SharesPage::loadAll()
{
    emit load(0);
    emit load(1);
}

void SharesPage::showEvent(QShowEvent*)
{
    loadAll();
}

//void MyTree::mousePressEvent(QMouseEvent *event)

void SharesPage::showError(int index, const QString &err )
{
    if (!index)
    {
        win->ui->label_errorMyShares->setVisible(true);
        win->ui->label_errorMyShares->setText(err);
    }
    else
    {
        win->ui->label_errorShrdWithMe->setVisible(true);
        win->ui->label_errorShrdWithMe->setText(err);
    }
    //tuka s dialog
}


void SharesPage::setButtons(int type, bool isRequest, QTreeWidget *table, QTreeWidgetItem *item,  binresult *req)
{
    QListWidget *lvButtons = new QListWidget(table);
    lvButtons->setMaximumHeight(30);
    lvButtons->setFlow(QListWidget::LeftToRight);
    lvButtons->setFrameStyle(QFrame::Sunken);
    //lvButtons->setFrameStyle(QFrame::Plain);
    // to set items hover shadow

    QListWidget *lvPermissions = new QListWidget(table);
    lvPermissions->setMaximumHeight(30);
    lvPermissions->setMinimumWidth(280);
    lvPermissions->setFlow(QListWidget::LeftToRight);
    lvPermissions->setFrameStyle(QFrame::Sunken);
    lvPermissions->adjustSize();

    QListWidgetItem* read = new QListWidgetItem("read", lvPermissions);
    QListWidgetItem* create = new QListWidgetItem("create", lvPermissions);
    QListWidgetItem* modify = new QListWidgetItem("modify", lvPermissions);
    QListWidgetItem* del  = new QListWidgetItem("delete", lvPermissions);


    // in another function
    read->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
    if (find_res(req, "canread")->num)
    {
        read->setCheckState(Qt::Checked);
        //item->setData(2,Qt::UserRole,Qt::Checked);
        read->setData(Qt::UserRole,true);
    }
    else
        read->setCheckState((Qt::Unchecked));

    create->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    if (find_res(req, "cancreate")->num)
    {
        create->setCheckState(Qt::Checked);
        //item->setData(2,Qt::UserRole,Qt::Checked);
        create->setData(Qt::UserRole,true);
    }
    else
        create->setCheckState((Qt::Unchecked));

    modify->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    if (find_res(req, "canmodify")->num)
    {
        modify->setCheckState(Qt::Checked);
        //item->setData(2,Qt::UserRole,Qt::Checked);
        modify->setData(Qt::UserRole,true);
    }
    else
        modify->setCheckState(Qt::Unchecked);

    del->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    if (find_res(req, "candelete")->num)
    {
        del->setCheckState(Qt::Checked);
        //item->setData(2,Qt::UserRole,Qt::Checked);
        del->setData(Qt::UserRole,true);
    }
    else
        del->setCheckState(Qt::Unchecked);

    table->setItemWidget(item,2,lvPermissions);


    if (!type && !isRequest) //tab My Shares, table my shares
    {
        QListWidgetItem *btnSave = new QListWidgetItem(tr("Save"), lvButtons);
        btnSave->setFlags(Qt::NoItemFlags);
        new QListWidgetItem(tr("Stop"), lvButtons);

        connect(lvButtons, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(btnsMofidyStop(QListWidgetItem*)));
        connect(lvPermissions, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(btnSaveEnable(QListWidgetItem*)));

        table->setItemWidget(item,3,lvButtons);
        return;
    }

    if(!type && isRequest) // table my requests
    {
        new QListWidgetItem(tr("Cancel"), lvButtons);
        connect(lvButtons, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(btnCancel(QListWidgetItem*)));
        table->setItemWidget(item,3,lvButtons);
        lvPermissions->setEnabled(false);
        return;
    }

    if (type && !isRequest) //tab Shared with me
    {
        new QListWidgetItem(tr("Stop"), lvButtons);
        connect(lvButtons, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(btnStop(QListWidgetItem*)));
        table->setItemWidget(item,3,lvButtons);
        lvPermissions->setEnabled(false);
        return;
    }

    if (type && isRequest)
    {
        new QListWidgetItem(tr("Accept"), lvButtons);
        new QListWidgetItem(tr("Reject"), lvButtons);
        connect(lvButtons, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(btnsAcceptReject(QListWidgetItem*)));
        table->setItemWidget(item, 3, lvButtons);
        lvPermissions->setEnabled(false);
        return;
    }

}

void SharesPage::btnsMofidyStop(QListWidgetItem *current) // my shares
{
    QListWidget *lv = current->listWidget();
    int index = lv->row(current);

    if (index)
    {
        //QWidget *t = lv->parentWidget(); // ili nativeparent // i za my requests da smenq mestata na butonite
        // QTreeWidget *table = qobject_cast<QTreeWidget *>(t);
        type = 0;
        emit stopShare(win->ui->treeMyShares);
    }
   // else
        //emit modifyShare(lv);
}

void SharesPage::btnCancel(QListWidgetItem *)
{
    type = 0;
    emit cancelRequest(win->ui->treeMyRequest);
}

void SharesPage::btnStop(QListWidgetItem *) // shared with me
{
    type = 1;
    emit stopShare(win->ui->treeSharedWithMe);
}

void SharesPage::btnsAcceptReject(QListWidgetItem *current) //shared with me requests
{
    QListWidget *lv = current->listWidget();
    int index = lv->row(current);
    if (!index)
        emit acceptRequest();
    else
    {
        type = 1;
        emit cancelRequest(win->ui->treeRequestsWithMe);
    }
}

void SharesPage::btnSaveEnable(QListWidgetItem *current) // to do check buttons
{
    QTreeWidgetItem *row = win->ui->treeMyShares->currentItem();
    QWidget *lvShares = win->ui->treeMyShares->itemWidget(row,3);
    QListWidget *lv = qobject_cast<QListWidget *>(lvShares);
    lv->item(0)->setFlags(Qt::ItemIsEnabled);
}


void SharesPage::acceptRequest()
{
    this->type = 1;
    if (!win->ui->treeRequestsWithMe->currentItem())
        return selectErr();
    quint64 sharerequestid = win->ui->treeRequestsWithMe->currentItem()->data(0, Qt::UserRole).toULongLong();
    DirectoryPickerDialog dir(app, win);
    dir.onlyMine=true;
    dir.showRoot=true;
    dir.setWindowTitle("Select a directory to accept share to...");
    if (dir.exec()==QDialog::Rejected || !dir.ui->dirtree->currentItem())
        return;
    quint64 folderid=dir.ui->dirtree->currentItem()->data(1, Qt::UserRole).toULongLong();
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->authentication.toUtf8();
    if (!(conn=app->getAPISock())){
        showError(1,"Could not connect to server. Check your Internet connection.");
        return;
    }
    res=send_command(conn, "acceptshare",
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_NUM("sharerequestid", sharerequestid),
                     P_NUM("folderid", folderid));
    api_close(conn);
    result=find_res(res, "result");
    if (!result){
        showError(1,"Could not connect to server. Check your Internet connection.");
        free(res);
        return;
    }
    if (result->num!=0){
        showError(1,find_res(res, "error")->str);
        free(res);
        return;
    }
    free(res);
    load(1);
}

void SharesPage::cancelRequest(QTreeWidget *table)
{
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->authentication.toUtf8();

    QTreeWidgetItem *current = table->currentItem();
    QModelIndex ind  = table->currentIndex();
    qDebug()<< current << ind;

    if (!table->currentItem())
        return selectErr();
    quint64 sharerequestid=table->currentItem()->data(0, Qt::UserRole).toULongLong();
    if (!(conn=app->getAPISock())){
        showError(type,"Could not connect to server. Check your Internet connection.");
        return;
    }
    res=send_command(conn, stopmethod2[type],
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_NUM("sharerequestid", sharerequestid));
    api_close(conn);
    result=find_res(res, "result");
    if (!result){
        showError(type,"Could not connect to server. Check your Internet connection.");
        free(res);
        return;
    }
    if (result->num!=0){
        showError(type,find_res(res, "error")->str);
        free(res);
        return;
    }
    free(res);
    load(type);
}

void SharesPage::stopShare(QTreeWidget *table)
{
    if (!table->currentItem())
      return selectErr();
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->authentication.toUtf8();

   // QTreeWidgetItem *current = table->currentItem();
    //QModelIndex ind  = table->currentIndex();
    //qDebug()<< current << ind;

    quint64 shareid = table->currentItem()->data(0, Qt::UserRole).toULongLong();
    if (!(conn=app->getAPISock())){
        showError(type,"Could not connect to server. Check your Internet connection.");
        return;
    }
    res=send_command(conn, "removeshare",
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_NUM("shareid", shareid));
    api_close(conn);
    result=find_res(res, "result");
    if (!result){
        showError(type,"Could not connect to server. Check your Internet connection.");
        free(res);
        return;
    }
    if (result->num!=0){
        showError(type,find_res(res, "error")->str);
        free(res);
        return;
    }
    free(res);
    load(type);
}

void SharesPage::modifyShare()
{
    this->type = 0;
    QTreeWidgetItem *item= win->ui->treeMyShares->currentItem();
    if (!item)
        return selectErr();
    ChangePermissionsDialog perms(item->data(1, Qt::UserRole).toUInt(), item->text(0), item->text(1), win);
    if (perms.exec()==QDialog::Rejected)
    return;
    quint64 shareid=item->data(0, Qt::UserRole).toULongLong();
    quint64 prms=(perms.ui->permCreate->isChecked()?1:0)+
          (perms.ui->permModify->isChecked()?2:0)+
        (perms.ui->permDelete->isChecked()?4:0);

   // QVariant d = lv->item(0)->data(Qt::UserRole); // dont work

    //quint64 prms0 = ( (lv->item(0)->data(Qt::UserRole)) ? 1: 0) ; // dont work
    //quint64 prms = ( (lv->item(1)->data(Qt::UserRole)) ? 1: 0) ;
    //quint64 prms2 = ( (lv->item(3)->checkState() == (Qt::Checked)) ? 1: 0) ;
    // ((lv->item(2)->checkState() == (Qt::Checked)) ? 2: 0)+
    //((lv->item(3)->checkState() == (Qt::Checked)) ? 4: 0);
    apisock *conn;
    binresult *res, *result;
    QByteArray auth=app->authentication.toUtf8();
    if (!(conn=app->getAPISock())){
        showError(0,"Could not connect to server. Check your Internet connection.");
        return;
    }
    res=send_command(conn, "changeshare",
                     P_LSTR("auth", auth.constData(), auth.size()),
                     P_NUM("shareid", shareid),
                     P_NUM("permissions", prms));
    api_close(conn);
    result=find_res(res, "result");
    if (!result){
        showError(0,"Could not connect to server. Check your Internet connection.");
        free(res);
        return;
    }
    if (result->num!=0){
        showError(0,find_res(res, "error")->str);
        free(res);
        return;
    }
    free(res);
    load(0);
}

void SharesPage::selectErr(){
    QMessageBox::information(this,"", "Please select a share." );
}

//Q_EXPORT_PLU(customTree, MyTree)
