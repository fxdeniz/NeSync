#include "AppConfig.h"

#include <QDir>
#include <QReadLocker>
#include <QWriteLocker>
#include <QCoreApplication>

QReadWriteLock AppConfig::lock;

AppConfig::AppConfig()
{
    QString settingsFilePath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
    settingsFilePath += QDir::separator();
    settingsFilePath += "settings.ini";

    settings = new QSettings(settingsFilePath, QSettings::Format::IniFormat);
}

AppConfig::~AppConfig()
{
    delete settings;
}

bool AppConfig::isDisclaimerAccepted() const
{
    QReadLocker readLocker(&lock);

    if(settings->value(KeyDisclaimerAccepted).toString() == "true")
        return true;

    return false;
}

void AppConfig::setDisclaimerAccepted(bool newDisclaimerAccepted)
{
    QWriteLocker writeLocker(&lock);

   settings->setValue(KeyDisclaimerAccepted, newDisclaimerAccepted);
}

bool AppConfig::isTrayIconInformed() const
{
    QReadLocker readLocker(&lock);

    if(settings->value(KeyTrayIconInformed).toString() == "true")
        return true;

    return false;
}

void AppConfig::setTrayIconInformed(bool newTrayIconInformed)
{
    QWriteLocker writeLocker(&lock);

    settings->setValue(KeyTrayIconInformed, newTrayIconInformed);
}

bool AppConfig::isBackupFolderPathValid() const
{
    QReadLocker readLocker(&lock);

    QString readValue = settings->value(KeyBackupFolderPath).toString();

    if(readValue.isNull() || readValue.isEmpty())
        return false;

    if(!QDir(readValue).exists())
        return false;

    return true;
}

QString AppConfig::getBackupFolderPath() const
{
    QReadLocker readLocker(&lock);

    QString readValue = settings->value(KeyBackupFolderPath).toString();
    readValue = QDir::toNativeSeparators(readValue);

    if(!readValue.endsWith(QDir::separator()))
        readValue.append(QDir::separator());

    return readValue;
}

void AppConfig::setBackupFolderPath(const QString &newBackupFolderPath)
{
    QWriteLocker writeLocker(&lock);

    QString value = QDir::toNativeSeparators(newBackupFolderPath);

    if(!value.endsWith(QDir::separator()))
        value.append(QDir::separator());

    settings->setValue(KeyBackupFolderPath, value);
}
