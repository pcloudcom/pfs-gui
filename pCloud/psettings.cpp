#include "common.h"
#include "psettings.h"
#include "pcloudapp.h"
#include <QDir>

PSettings::PSettings(PCloudApp *a){
    app=a;
    settings=new QSettings("PCloud", "pCloud");
    if (!settings->contains("path")){
        QString path(QDir::homePath()+"/" DEFAULT_PCLOUD_DIR);
        QDir dir(path);
        if (!dir.exists())
            dir.mkpath(path);
        settings->setValue("path", path);
    }
    if (!settings->contains("usessl"))
        settings->setValue("usessl", DEFAULT_USE_SSL);
    if (!settings->contains("cachesize"))
        settings->setValue("cachesize", DEFAULT_CACHE_SIZEMB);
}

PSettings::~PSettings(){
    delete settings;
}

bool PSettings::isSet(const QString &key){
    return settings->contains(key);
}

QString PSettings::get(const QString &key){
    return settings->value(key).toString();
}

void PSettings::set(const QString &key, const QString &val){
    settings->setValue(key, val);
}

void PSettings::unset(const QString &key){
    settings->remove(key);
}

int PSettings::geti(const QString &key){
    return settings->value(key).toInt();
}

void PSettings::seti(const QString &key, int val){
    settings->setValue(key, val);
}
