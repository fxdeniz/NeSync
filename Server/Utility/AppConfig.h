#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QReadWriteLock>

class AppConfig
{
public:
    AppConfig() = delete;

    static bool isStorageFolderPathValid();
    static QString getStorageFolderPath();
    static void setStorageFolderPath(const QString &newPath);

private:
    static QReadWriteLock lock;
    static QString storageFolderPath;
};

#endif // APPCONFIG_H
