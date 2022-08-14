#ifndef ICOMBOBOXNOTENOTIFIER_H
#define ICOMBOBOXNOTENOTIFIER_H

#include <QWidget>

class IComboBoxNoteNotifier : public QWidget
{
    Q_OBJECT
public:
    explicit IComboBoxNoteNotifier(QWidget *parent = nullptr);
    virtual ~IComboBoxNoteNotifier();

    static QString defaultNoNoteText();

signals:
    void signalNoteNumberAdded(const QStringList &itemList);
    void signalNoteNumberDeleted(const QStringList &itemList, const QString &removedItem);

};

#endif // ICOMBOBOXNOTENOTIFIER_H
