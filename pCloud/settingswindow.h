#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>

namespace Ui {
class SettingsWindow;
}

class PCloudApp;

class SettingsWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit SettingsWindow(PCloudApp *a, QWidget *parent = 0);
    ~SettingsWindow();
protected:
    void showEvent(QShowEvent * event);
private:
    PCloudApp *app;
    QString dir;
    Ui::SettingsWindow *ui;
    void closeEvent(QCloseEvent *event);
    void writeToFile(const QString &path, const QString &data);
public slots:
    void dirChange();
    void saveSettings();
};

#endif // SETTINGSWINDOW_H
