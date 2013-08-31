#ifndef CHANGEPERMISSIONSDIALOG_H
#define CHANGEPERMISSIONSDIALOG_H

#include <QDialog>

namespace Ui {
class ChangePermissionsDialog;
}

class ChangePermissionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ChangePermissionsDialog(quint32 perms, QString folderName, QString sharedWith, QWidget *parent = 0);
    ~ChangePermissionsDialog();
    Ui::ChangePermissionsDialog *ui;
};

#endif // CHANGEPERMISSIONSDIALOG_H
