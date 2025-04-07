#include "AppConfig.h"

#include <QDir>
#include <QReadLocker>
#include <QWriteLocker>

QReadWriteLock AppConfig::lock;
QString AppConfig::storageFolderPath;

bool AppConfig::isStorageFolderPathValid()
{
    QReadLocker readLocker(&lock);

    if(storageFolderPath.isNull() || storageFolderPath.isEmpty())
        return false;

    if(!QDir(storageFolderPath).exists())
        return false;

    return true;
}

QString AppConfig::getStorageFolderPath()
{
    QReadLocker readLocker(&lock);

    QString readValue = storageFolderPath;
    readValue = QDir::toNativeSeparators(readValue);

    if(!readValue.endsWith(QDir::separator()))
        readValue.append(QDir::separator());

    return readValue;
}

void AppConfig::setStorageFolderPath(const QString &newPath)
{
    QWriteLocker writeLocker(&lock);

    QString value = QDir::toNativeSeparators(newPath);

    if(!value.endsWith(QDir::separator()))
        value.append(QDir::separator());

    storageFolderPath = newPath;
}
