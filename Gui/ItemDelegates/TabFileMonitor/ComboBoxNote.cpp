#include "ComboBoxNote.h"

ComboBoxNote::ComboBoxNote(QWidget *parent)
    : QComboBox{parent}
{

}

void ComboBoxNote::slotOnItemAdded(const QStringList &itemList)
{
    auto currentIndex = QComboBox::currentIndex();

    QComboBox::clear();
    QComboBox::addItem(QComboBox::placeholderText());
    QComboBox::addItems(itemList);

    if(currentIndex < 0) // If nothing selected in combobox.
        QComboBox::setCurrentIndex(0);
    else
        QComboBox::setCurrentIndex(currentIndex);
}

void ComboBoxNote::slotOnItemRemoved(const QStringList &itemList, const QString &removedItem)
{
    auto currentIndex = QComboBox::currentIndex();
    auto currentText = QComboBox::currentText();

    QComboBox::clear();
    QComboBox::addItem(QComboBox::placeholderText());
    QComboBox::addItems(itemList);

    if(currentIndex < 0) // If nothing selected in combobox.
    {
        QComboBox::setCurrentIndex(0);
        return;
    }

    int indexToSet;


    if(currentText == removedItem) // If selected number deleted.
        indexToSet = 0;
    else
    {
        if(currentText.toInt() < removedItem.toInt())
            indexToSet = currentIndex;
        else
            indexToSet = currentIndex - 1;
    }

    QComboBox::setCurrentIndex(indexToSet);
}
