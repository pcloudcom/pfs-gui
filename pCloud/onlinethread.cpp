#include "onlinethread.h"
#include "pcloudapp.h"

OnlineThread::OnlineThread(PCloudApp *a, QObject *parent) :
    QThread(parent)
{
    app=a;
}

void OnlineThread::run()
{
    do {
      app->mount();
      sleep(10);
    } while(!app->isMounted());
    app->setOnlineStatus(true);
}
