#ifndef MONITORINGTHREAD_H
#define MONITORINGTHREAD_H

#include <QThread>

class PCloudApp;

class MonitoringThread : public QThread
{
    Q_OBJECT
private:
    PCloudApp *app;
public:
    bool doRun;
    MonitoringThread(PCloudApp *a);
protected:
     void run();
signals:
    void sendMessageSignal(QString, QString);
};

#endif // MONITORINGTHREAD_H
