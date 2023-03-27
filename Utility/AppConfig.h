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

private:
    static const inline QString KeyDisclaimerAccepted = "disclaimer_accepted";
    static const inline QString KeyTrayIconInformed = "tray_icon_informed";

    static QReadWriteLock lock;

private:
    QSettings *settings;
};

#endif // APPCONFIG_H
