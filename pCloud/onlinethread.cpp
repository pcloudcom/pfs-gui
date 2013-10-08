#include "onlinethread.h"
#include "pcloudapp.h"

OnlineThread::OnlineThread(PCloudApp *a, QObject *parent) :
    QThread(parent)
{
    app=a;
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
    app->setOnlineStatus(true);
}
