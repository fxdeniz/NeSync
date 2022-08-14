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
