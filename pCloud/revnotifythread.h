#ifndef REVNOTIFYTHREAD_H
#define REVNOTIFYTHREAD_H
#include <QThread>

#ifdef Q_OS_WIN
#include <windows.h>

class PCloudApp;

class RevNotifyThread : public QThread
{
    Q_OBJECT
private:
    PCloudApp *app;
    HANDLE hPipe;
public:
    RevNotifyThread(PCloudApp *a);
    void killPipe();
    ~RevNotifyThread();
protected:
    void run();
};
#endif

#endif // REVNOTIFYTHREAD_H
