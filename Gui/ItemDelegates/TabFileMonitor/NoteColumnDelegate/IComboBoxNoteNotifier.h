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
    const QHash<int, QString> &getNoteMap() const;

signals:
    void signalNoteNumberAdded(const QStringList &itemList);
    void signalNoteNumberDeleted(const QStringList &itemList, const QString &removedItem);

protected:
    QHash<int, QString> noteMap;

};

#endif // ICOMBOBOXNOTENOTIFIER_H
