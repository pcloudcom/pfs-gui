#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H
#include "pcloudwindow.h"
#include "pcloudapp.h"

#include <QObject>
#include <QString>

class PCloudWindow;

class SettingsPage: public QObject
{
    Q_OBJECT
public:
  explicit SettingsPage(PCloudWindow *w, PCloudApp *a, QObject *parent = 0);

private:
    PCloudWindow *win;
    PCloudApp *app;
    QString dir;
    void writeToFile(const QString &path, const QString &data);
public slots:
    void dirChange();
    void saveSettings(); 
};

#endif // SETTINGSPAGE_H
