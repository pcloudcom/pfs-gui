#include "common.h"
#include "psettings.h"
#include "pcloudapp.h"
#include <QDir>

#ifdef Q_OS_LINUX
#include <sys/sysinfo.h>
#endif

#ifdef Q_OS_WIN
char getFirstFreeDevice()
{
    DWORD devices = GetLogicalDrives();
    for (int i = 3; i < 32; ++i)
        if ((devices & (1<<i))==0)
            return i + 'A';
    return 0;
}

bool isConnected(char drive)
{
    DWORD devices = GetLogicalDrives();
    return devices & (1<<drive);
}

#endif


PSettings::PSettings(PCloudApp *a){
    app=a;
    settings=new QSettings("PCloud", "pCloud");
    if (!settings->contains("path")){
#ifdef Q_OS_WIN
        QString path("a:");
        path[0] = getFirstFreeDevice();
#else
        QString path(QDir::homePath()+"/" DEFAULT_PCLOUD_DIR);
        QDir dir(path);
        if (!dir.exists())
            dir.mkpath(path);
#endif
        settings->setValue("path", path);
    }
    if (!settings->contains("usessl"))
        settings->setValue("usessl", DEFAULT_USE_SSL);
    if (!settings->contains("cachesize"))
    {
        qint32 cacheSize = getCacheSize();
        settings->setValue("cachesize", cacheSize);
    }

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

quint64 PSettings::getu64(const QString &key){
    return settings->value(key).toULongLong();
}

void PSettings::setu64(const QString &key, quint64 val){
    settings->setValue(key, val);
}

qint32 PSettings::getCacheSize(){

#ifdef Q_OS_WIN
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    qint32 totalPhysRAM = (qint32)status.ullTotalPhys/1024/1024/10;
    qint32 totalAvailRAM = (qint32)status.ullAvailPhys/1024/1024;
#else
    struct sysinfo sys_info;
    sysinfo(&sys_info);
    qint32  totalPhysRAM=(qint32)(sys_info.totalram/1024/1024/10);
    qint32  totalAvailRAM=(qint32)(sys_info.freeram/1024/1024);
#endif

    qint32 totalRAM = (totalPhysRAM < totalAvailRAM)? totalPhysRAM:totalAvailRAM ;
    return totalRAM;
}
