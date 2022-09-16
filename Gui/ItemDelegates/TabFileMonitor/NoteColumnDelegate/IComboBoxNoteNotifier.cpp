#include "IComboBoxNoteNotifier.h"

IComboBoxNoteNotifier::IComboBoxNoteNotifier(QWidget *parent)
    : QWidget(parent)
{

}

IComboBoxNoteNotifier::~IComboBoxNoteNotifier()
{

}

QString IComboBoxNoteNotifier::defaultNoNoteText()
{
    return "No note";
}

const QHash<int, QString> &IComboBoxNoteNotifier::getNoteMap() const
{
    return noteMap;
}
