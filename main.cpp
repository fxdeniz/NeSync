#include <QApplication>
#include <QMessageBox>
#include <QSettings>

#include "Gui/MainWindow.h"
#include "Utility/AppConfig.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AppConfig config;

    if(!config.isDisclaimerAccepted())
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

        if(result != QMessageBox::StandardButton::Yes)
            return 0;
    }

    config.setDisclaimerAccepted(true);
    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;
    w.show();

    return app.exec();
}
