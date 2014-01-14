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
#include <QRegExp>

SettingsPage::SettingsPage(PCloudWindow *w, PCloudApp *a, QObject* parent):
    QObject(parent)
{
    win = w;
    app = a;

    initSSL = app->settings->geti("usessl");
    dir=win->app->settings->get("path");
    initFlrd = "pCloud point: " + dir;
    initCache = app->settings->get("cachesize");
    initSettingsPage();

    QRegExp regExp("[1-9][0-9]{0,3}");
    win->ui->edit_cache->setValidator(new QRegExpValidator(regExp, this));

#ifdef Q_OS_WIN
    win->ui->btnChangeDestFldr->setVisible(false);
#else
    connect(win->ui->btnChangeDestFldr, SIGNAL(clicked()), this, SLOT(dirChange()));
#endif           
    connect(win->ui->edit_cache, SIGNAL(textEdited(QString)),this, SLOT(setSaveBtnEnable()));
    connect(win->ui->checkBox_ssl, SIGNAL(stateChanged(int)), this, SLOT(setSaveBtnEnable()));
    connect(win->ui->btnSaveSttngs, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(win->ui->btnCancelSttngs, SIGNAL(clicked()), this, SLOT(cancelSettings()));
}

void SettingsPage::initSettingsPage()
{    
    dir=win->app->settings->get("path");    
    win->ui->label_fldrVal->setText(initFlrd);
    win->ui->checkBox_ssl->setChecked(initSSL);
    win->ui->edit_cache->setText(initCache);
    win->ui->btnSaveSttngs->setEnabled(false);
    win->ui->btnCancelSttngs->setEnabled(false);
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
        return;
    }
    else if (dir!=odir){
        QDir d(dir);
        if (d.entryList(QDir::NoDotAndDotDot|QDir::AllEntries|QDir::System).count()>0){
            QMessageBox msgBox;
            msgBox.setText("Please select or create an empty directory.");
            msgBox.exec();
            dir=odir;
            return;
        }
    }
#endif
    win->ui->label_fldrVal->setText("pCloud point: "+dir);
    emit setSaveBtnEnable();
}

void SettingsPage::saveSettings()
{
    bool cachechanged=false;
    int cachesize=atoi(win->ui->edit_cache->text().toUtf8());
    if (cachesize && cachesize!=app->settings->geti("cachesize")){
        cachechanged=true;
        app->settings->seti("cachesize", cachesize);
        initCache = win->ui->edit_cache->text();
    }
    initSSL = win->ui->checkBox_ssl->checkState()? 1: 0;
    app->settings->seti("usessl", initSSL);
    if (dir!=app->settings->get("path")){
        bool mounted=app->isMounted();
        if (mounted)
            app->unMount();
        app->settings->set("path", dir);
        initFlrd = "pCloud point: " + dir;
        if (mounted)
            app->mount();
    }
    else if (app->isMounted()){
        QString spath=app->settings->get("path")+"/.pfs_settings/";
        writeToFile(spath+"use_ssl", app->settings->get("usessl"));
        if (cachechanged)
            writeToFile(spath+"cache_size", QString("%1").arg((quint64)cachesize*1024*1024));
    }
    win->ui->btnCancelSttngs->setEnabled(false);
    win->ui->btnSaveSttngs->setEnabled(false);
}

void SettingsPage::writeToFile(const QString &path, const QString &data)
{
    QFile file(path);
    if (file.open(QFile::WriteOnly | QFile::Truncate)){
        QTextStream out(&file);
        out << data;
    }
}

void SettingsPage::setSaveBtnEnable()
{
    bool checked = win->ui->checkBox_ssl->checkState();
    if (win->ui->label_fldrVal->text() != initFlrd ||
            win->ui->edit_cache->text() != initCache ||
            checked != initSSL )
    {
        win->ui->btnSaveSttngs->setEnabled(true);
        win->ui->btnCancelSttngs->setEnabled(true);
    }
    else
    {
        win->ui->btnSaveSttngs->setEnabled(false);
        win->ui->btnCancelSttngs->setEnabled(false);
    }
}

void SettingsPage::cancelSettings()
{
    initSettingsPage();
}
