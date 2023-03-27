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
