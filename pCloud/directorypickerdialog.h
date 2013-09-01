#ifndef DIRECTORYPICKERDIALOG_H
#define DIRECTORYPICKERDIALOG_H

#include <QDialog>

namespace Ui {
class DirectoryPickerDialog;
}

class PCloudApp;

class DirectoryPickerDialog : public QDialog
{
    Q_OBJECT
    
public:
    bool onlyMine;
    bool showRoot;
    Ui::DirectoryPickerDialog *ui;
    explicit DirectoryPickerDialog(PCloudApp *a, QWidget *parent = 0);
    ~DirectoryPickerDialog();
    
private:
    PCloudApp *app;
    void showEvent(QShowEvent *);
    void showError(const QString &err);
};

#endif // DIRECTORYPICKERDIALOG_H
