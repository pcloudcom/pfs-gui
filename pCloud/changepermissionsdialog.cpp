#include "changepermissionsdialog.h"
#include "ui_changepermissionsdialog.h"

ChangePermissionsDialog::ChangePermissionsDialog(quint32 perms, QString folderName, QString sharedWith, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePermissionsDialog)
{
    ui->setupUi(this);
    ui->foldername->setText(folderName);
    ui->sharedwith->setText(sharedWith);
    ui->permRead->setChecked(perms&1);
    ui->permModify->setChecked(perms&2);
    ui->permDelete->setChecked(perms&4);
    ui->permCreate->setChecked(perms&8);
}

ChangePermissionsDialog::~ChangePermissionsDialog()
{
    delete ui;
}
