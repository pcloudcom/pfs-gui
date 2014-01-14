#ifndef SHARESPAGE_H
#define SHARESPAGE_H

#include "pcloudwindow.h"
#include "pcloudapp.h"
#include "common.h"

#include <QObject>
#include <QWidget>
#include <QItemDelegate>
#include<QListWidgetItem>
#include<QTreeWidget>
//#include<QDesignerCustomWidgetInterface>

class PCloudWindow; // allows access to ui

class SharesPage : public QMainWindow
{
    Q_OBJECT
public:
    friend class ShareFolderWindow;
    explicit SharesPage(PCloudWindow *w, PCloudApp *a, QWidget *parent = 0);
    void selectErr();
    void loadAll();    
private:
    PCloudWindow *win;
    PCloudApp *app;
    int type;

    void fillList(QTreeWidget *table, binresult *arr, const char *fieldname, const char *idname, int type, bool isRequest);
    void showError(int index, const QString &err);
    void showEvent(QShowEvent*);
    void setButtons(int type, bool isRequest,QTreeWidget* table, QTreeWidgetItem* item, binresult *req);  
    void setRequestsVisibility(bool visible, int type);
//protected:
    // void mousePressEvent(QMouseEvent *event);
    // void QTreeWidget::mousePressEvent(QMouseEvent *eventtest);

public slots:      
    void load(int type);
    void btnsMofidyStop(QListWidgetItem*current);
    void btnSaveEnable(QListWidgetItem *current);
    void btnCancel(QListWidgetItem*);
    void btnStop(QListWidgetItem*);
    void btnsAcceptReject(QListWidgetItem *current);   

    void acceptRequest();
    void stopShare(QTreeWidget *table);
    void cancelRequest(QTreeWidget *table); //reject
  //--temp  void modifyShare(QListWidget *lv);
    void modifyShare();
    void testSelectedItem();

    //temp
    void btnStop0();
    void btnStop1();
    void btnCancel0();
    void btnCancel1();
};


#endif // SHARESPAGE_H
