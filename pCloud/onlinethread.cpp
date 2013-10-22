#include "onlinethread.h"
#include "pcloudapp.h"

OnlineThread::OnlineThread(PCloudApp *a, QObject *parent) :
    QThread(parent)
{
    app=a;
    connect(this, SIGNAL(setOnlineStatus(bool)), app, SLOT(setOnlineStatus(bool)));
}

void OnlineThread::run()
{

    while (1) {
      app->mount();
      if (app->isMounted())
          break;
      else
          sleep(5);
    };
    emit setOnlineStatus(true);
}

