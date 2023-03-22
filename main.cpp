#include <QApplication>
#include <QMessageBox>
#include <QSettings>

#include "Gui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString settingsFilePath = QDir::toNativeSeparators(app.applicationDirPath()) + QDir::separator() + "settings.ini";
    QSettings settings(settingsFilePath, QSettings::Format::IniFormat);
    QString settingKey = "disclaimer_accepted";

    if(settings.value(settingKey).toString() != "true") // Value can't read or not accepted
    {
        QString title = QObject::tr("Disclaimer !");
        QString message = QObject::tr("You're about the use Pre-Alpha version of <b>NeSync</b>."
                                      "<br><br>"
                                      "Pre-Alpha version means, this software <b>IS NOT complete yet</b> and contains bugs."
                                      "<br>"
                                      "Developer of this software <b>DOES NOT recommed</b> you to use this software for critical things."
                                      "<br><br>"
                                      "By pressing <b>Yes</b> you accept all the risks associated with using in-complete software. Such as data loss or system crashes."
                                      "<br><br>"
                                      "Do you accept the accept the risks ?");

        QMessageBox::StandardButton result = QMessageBox::question(nullptr, title, message);

        if(result != QMessageBox::StandardButton::Yes)
            return 0;
    }

    settings.setValue(settingKey, true);

    MainWindow w;
    w.show();

    return app.exec();
}
