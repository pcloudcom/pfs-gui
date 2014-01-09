#include "settingspage.h"
#include "pcloudwindow.h"
#include "ui_pcloudwindow.h"
#include "pcloudapp.h"
#include "common.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QTextStream>
#include <QFile>


SettingsPage::SettingsPage(PCloudWindow *w, PCloudApp *a, QObject* parent):
    QObject(parent)
{
    win = w;
    app = a;

    dir=win->app->settings->get("path");
    win->ui->label_fldrVal->setText("pCloud point: "+dir);
    win->ui->checkBox_ssl->setChecked(app->settings->geti("usessl"));
    win->ui->edit_cache->setText(app->settings->get("cachesize"));
 //   win->ui->tabWidgetSttngs->setTabText(0, tr("Mount Point "));
   // win->ui->tabWidgetSttngs->setTabText(1, tr("Cache "));
    //win->ui->tabWidgetSttngs->setTabText(2, tr("Secure Connection "));

#ifdef Q_OS_WIN
    win->ui->btnChangeDestFldr->setVisible(false);
#else
    connect(win->ui->btnChangeDestFldr, SIGNAL(clicked()), this, SLOT(dirChange()));
#endif    
    //connect(win->ui->btnCancelSttngs, SIGNAL(clicked()), win, SLOT(hide()));
    win->ui->btnCancelSttngs->setVisible(false);
    connect(win->ui->btnSaveSttngs, SIGNAL(clicked()), this, SLOT(saveSettings()));
}

void SettingsPage::dirChange() //NOT for win;  to add choose mount letter for win
{
    QString odir=app->settings->get("path");
 #ifdef Q_OS_WIN
    dir = "А:";
    dir[0] = getFirstFreeDevice();
 #else
    QString ndir=QFileDialog::getExistingDirectory(win->ui->pageSettings, "Select pCloud folder", dir, QFileDialog::ShowDirsOnly);
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
    win->ui->label_fldrVal->setText("pCloud point: "+dir);
}

void SettingsPage::saveSettings()
{
    bool cachechanged=false;
    int cachesize=atoi(win->ui->edit_cache->text().toUtf8());
    if (cachesize && cachesize!=app->settings->geti("cachesize")){
        cachechanged=true;
        app->settings->seti("cachesize", cachesize);
    }
    app->settings->seti("usessl", win->ui->checkBox_ssl->checkState()? 1: 0);
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
}

void SettingsPage::writeToFile(const QString &path, const QString &data)
{
    QFile file(path);
    if (file.open(QFile::WriteOnly | QFile::Truncate)){
        QTextStream out(&file);
        out << data;
    }
}
