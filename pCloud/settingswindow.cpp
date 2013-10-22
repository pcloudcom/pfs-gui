#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "pcloudapp.h"
#include "common.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QTextStream>
#include <QFile>

SettingsWindow::SettingsWindow(PCloudApp *a, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsWindow)
{
    app=a;
    setWindowIcon(QIcon(WINDOW_ICON));
    ui->setupUi(this);
#ifdef Q_OS_WIN
    ui->dirchangebutton->setVisible(false);
#else
    connect(ui->dirchangebutton , SIGNAL(clicked()), this, SLOT(dirChange()));
#endif
    connect(ui->cancelbutton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->savebutton, SIGNAL(clicked()), this, SLOT(saveSettings()));
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::showEvent(QShowEvent *event)
{
    dir=app->settings->get("path");
    ui->dir->setText("pCloud folder: "+dir);
    ui->usessl->setChecked(app->settings->geti("usessl"));
    ui->cachesize->setText(app->settings->get("cachesize"));
    event->accept();
}

void SettingsWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void SettingsWindow::dirChange()
{
    QString odir=app->settings->get("path");
 #ifdef Q_OS_WIN
    dir = "А:";
    dir[0] = getFirstFreeDevice();
 #else
    QString ndir=QFileDialog::getExistingDirectory(this, "Select pCloud folder", dir, QFileDialog::ShowDirsOnly);
    if (ndir!="")
        dir=ndir;
    else
        return;
    if (app->isMounted() && dir!=odir && odir==dir.left(odir.size())){
        QMessageBox msgBox;
        msgBox.setText("You can not choose subdirectory of your current pCloud folder.");
        msgBox.exec();
        dir=odir;
    }
    else if (dir!=odir){
        QDir d(dir);
        if (d.entryList(QDir::NoDotAndDotDot|QDir::AllEntries|QDir::System).count()>0){
            QMessageBox msgBox;
            msgBox.setText("Please select or create an empty directory.");
            msgBox.exec();
            dir=odir;
        }
    }
#endif
    ui->dir->setText("pCloud folder: "+dir);
}

void SettingsWindow::writeToFile(const QString &path, const QString &data){
    QFile file(path);
    if (file.open(QFile::WriteOnly | QFile::Truncate)){
        QTextStream out(&file);
        out << data;
    }
}

void SettingsWindow::saveSettings()
{
    bool cachechanged=false;
    int cachesize=atoi(ui->cachesize->text().toUtf8());
    if (cachesize && cachesize!=app->settings->geti("cachesize")){
        cachechanged=true;
        app->settings->seti("cachesize", cachesize);
    }
    app->settings->seti("usessl", ui->usessl->checkState()?1:0);
    if (dir!=app->settings->get("path")){
        bool mounted=app->isMounted();
        if (mounted)
            app->unMount();
        app->settings->set("path", dir);
        if (mounted)
            app->mount();
    }
    else if (app->isMounted()){
        QString spath=app->settings->get("path")+"/.pfs_settings/";
        writeToFile(spath+"use_ssl", app->settings->get("usessl"));
        if (cachechanged)
            writeToFile(spath+"cache_size", QString("%1").arg((quint64)cachesize*1024*1024));
    }
    hide();
}
