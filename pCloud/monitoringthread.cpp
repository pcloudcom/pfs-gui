#include <QFile>
#include "monitoringthread.h"
#include "pcloudapp.h"

MonitoringThread::MonitoringThread(PCloudApp *a)
{
    app=a;
    connect(this, SIGNAL(sendMessageSignal(QString, QString)), app, SLOT(showTrayMessage(QString, QString)));
    connect(this, SIGNAL(setOnlineStatus(bool)), app, SLOT(setOnlineStatus(bool)));
}

typedef struct {
    quint64 diffid;
    quint32 type;
    quint32 length;
    char buff[4096];
} msg;

void MonitoringThread::run()
{
    msg m;
    sleep(2);
    while (1){
        QFile file(app->settings->get("path")+"/.pfs_settings/events");

        if (file.open(QFile::ReadOnly)){
            if (file.read((char *)&m, sizeof(m))>=offsetof(msg, buff)){
                m.buff[m.length]=0;
                if (m.type==0 || m.type==1){
                    QString diffid=QString("sdiffidu%1").arg(app->userid);
                    if (!app->settings->isSet(diffid) || m.diffid>app->settings->getu64(diffid)){
                        app->settings->setu64(diffid, m.diffid);
                        app->lastMessageType=m.type;
                        emit sendMessageSignal(m.buff, "");
                    }
                }
                else if (m.type==2 || m.type==3)
                    app->setOnlineStatus(m.type==3);
            }
            file.close();
        }
        sleep(1);
    }
}

