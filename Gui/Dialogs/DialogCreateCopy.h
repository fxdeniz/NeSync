#ifndef DIALOGCREATECOPY_H
#define DIALOGCREATECOPY_H

#include <QDialog>

namespace Ui {
class DialogCreateCopy;
}

class DialogCreateCopy : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCreateCopy(QWidget *parent = nullptr);
    ~DialogCreateCopy();

    void show(const QString &fileSymbolPath, qlonglong versionNumber);

private slots:
    void on_buttonSelectLocation_clicked();

private:
    Ui::DialogCreateCopy *ui;
    QString currentFileSymbolPath;

};

#endif // DIALOGCREATECOPY_H
