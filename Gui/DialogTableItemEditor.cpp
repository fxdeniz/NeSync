#include "DialogTableItemEditor.h"
#include "ui_DialogTableItemEditor.h"

DialogTableItemEditor::DialogTableItemEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTableItemEditor)
{
    ui->setupUi(this);
}

DialogTableItemEditor::~DialogTableItemEditor()
{
    delete ui;
}
