#ifndef ONLINETHREAD_H
#define ONLINETHREAD_H

#include <QThread>

class PCloudApp;

class OnlineThread : public QThread
{
    Q_OBJECT
private:
    PCloudApp *app;
public:
    explicit OnlineThread(PCloudApp *a, QObject *parent = 0);
protected:
    void run();
};

#endif // ONLINETHREAD_H
