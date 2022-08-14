#ifndef COMBOBOXNOTE_H
#define COMBOBOXNOTE_H

#include <QComboBox>

class ComboBoxNote : public QComboBox
{
    Q_OBJECT
public:
    ComboBoxNote(QWidget *parent = nullptr);

public slots:
    void slotOnItemAdded(const QStringList &itemList);
    void slotOnItemRemoved(const QStringList &itemList, const QString &removedItem);
};

#endif // COMBOBOXNOTE_H
