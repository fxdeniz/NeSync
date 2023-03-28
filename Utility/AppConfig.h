#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QSettings>
#include <QReadWriteLock>

class AppConfig
{
public:
    AppConfig();
    ~AppConfig();

    bool isDisclaimerAccepted() const;
    void setDisclaimerAccepted(bool newDisclaimerAccepted);

    bool isTrayIconInformed() const;
    void setTrayIconInformed(bool newTrayIconInformed);

    bool isBackupFolderPathValid() const;
    QString getBackupFolderPath() const;
    void setBackupFolderPath(const QString &newBackupFolderPath);

private:
    static const inline QString KeyDisclaimerAccepted = "disclaimer_accepted";
    static const inline QString KeyTrayIconInformed = "tray_icon_informed";
    static const inline QString KeyBackupFolderPath = "backup_folder_path";

    static QReadWriteLock lock;

private:
    QSettings *settings;
};

#endif // APPCONFIG_H
