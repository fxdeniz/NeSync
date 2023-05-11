#include <QSettings>
#include <QMessageBox>
#include <QApplication>
#include <QStandardPaths>

#include "Gui/MainWindow.h"
#include "Utility/AppConfig.h"

bool askAcceptenceForDisclaimer();
void showStorageLocationMessage();

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AppConfig config;

    if(!config.isDisclaimerAccepted())
    {
        bool isAccepted = askAcceptenceForDisclaimer();

        if(isAccepted)
            config.setDisclaimerAccepted(true);
        else
            return 0;
    }

    if(!config.isStorageFolderPathValid())
        showStorageLocationMessage();

    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;
    w.show();

    return app.exec();
}

bool askAcceptenceForDisclaimer()
{
    QString title = QObject::tr("Disclaimer !");
    QString message = QObject::tr("You're about the use Pre-Alpha version of <b>NeSync</b>."
                                  "<br><br>"
                                  "Pre-Alpha version means, this software <b>IS NOT complete yet</b> and contains bugs."
                                  "<br>"
                                  "Developer of this software <b>DOES NOT recommed</b> you to use this software for critical things."
                                  "<br><br>"
                                  "By pressing <b>Yes</b> you accept all the risks associated with using in-complete software."
                                  " Such as data loss, system crashes and security breaches."
                                  "<br><br>"
                                  "Do you accept the accept the risks ?");

    QMessageBox::StandardButton result = QMessageBox::question(nullptr, title, message);

    if(result == QMessageBox::StandardButton::Yes)
        return true;
    else
        return false;
}

void showStorageLocationMessage()
{
    QString storagePath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::HomeLocation);
    storagePath = QDir::toNativeSeparators(storagePath) + QDir::separator();
    storagePath += "nesync_";
    storagePath += QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces).mid(0, 8);
    storagePath += QDir::separator();

    QDir().mkpath(storagePath);
    AppConfig().setStorageFolderPath(storagePath);

    QString title = QObject::tr("Auto generated storage folder !");

    QString message = QObject::tr("NeSync uses file database which stores your files behind the scenes.<br>"
                                  "This may require a lot of storage space (depending on size of your files).<br>"
                                  "<b>Auto</b> generated storage folder is at:<br>"
                                  "<center><b>%1</b></center><br>"
                                  "If you want, you can change storage folder location from settings.").arg(storagePath);

    QMessageBox::information(nullptr, title, message);
}
